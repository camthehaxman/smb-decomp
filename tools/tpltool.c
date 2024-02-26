#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lodepng.h"
#define STB_DXT_IMPLEMENTATION
#include "stb_dxt.h"

#define ARRAY_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ROUND_UP(val, n) (((val) + (n) - 1) / (n) * (n))
#define VERIFY_ENCODER

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef enum
{
    GX_TF_I4 = 0x0,
    GX_TF_I8 = 0x1,
    GX_TF_IA4 = 0x2,
    GX_TF_IA8 = 0x3,
    GX_TF_RGB565 = 0x4,
    GX_TF_RGB5A3 = 0x5,
    GX_TF_RGBA8 = 0x6,
    GX_TF_CMPR = 0xE,
} GXTexFmt;

struct Color { u8 r, g, b, a; };

static u32 bswap32(u32 val)
{
	return ((val >> 24) & 0xFF) | ((val >> 8) & 0xFF00) | ((val << 8) & 0xFF0000) | ((val << 24) & 0xFF000000);
}

static u16 bswap16(u16 val)
{
	return ((val >> 8) & 0xFF) | ((val << 8) & 0xFF00);
}

static bool read_u32(FILE *f, u32 *out)
{
	if (fread(out, sizeof(*out), 1, f) == 1)
	{
		*out = bswap32(*out);
		return true;
	}
	return false;
}

static bool read_u16(FILE *f, u16 *out)
{
	if (fread(out, sizeof(*out), 1, f) == 1)
	{
		*out = bswap16(*out);
		return true;
	}
	return false;
}

static bool write_u32(FILE *f, u32 val)
{
	val = bswap32(val);
	return (fwrite(&val, sizeof(val), 1, f) == 1);
}

static bool write_u16(FILE *f, u16 val)
{
	val = bswap16(val);
	return (fwrite(&val, sizeof(val), 1, f) == 1);
}

// TODO: see if we can get automatic mipmap generation to match!
/*
static void *generate_mipmap(u8 *pixels, int width, int height, int lod)
{
	int outWidth = width >> lod;
	int outHeight = height >> lod;
	int sampWidth = width / outWidth;
	int sampHeight = height / outHeight;
	u8 *output = malloc(outWidth * outHeight * 4);
	memset(output, 0xFF, outWidth * outHeight * 4);

	for (int y = 0; y < outHeight; y++)
	{
		for (int x = 0; x < outWidth; x++)
		{
			uint32_t r = 0;
			uint32_t g = 0;
			uint32_t b = 0;
			uint32_t a = 0;

			// sample pixels
			for (int sampY = y * sampHeight; sampY < (y+1) * sampHeight; sampY++)
			{
				for (int sampX = x * sampWidth; sampX < (x+1) * sampWidth; sampX++)
				{
					u8 *in = pixels + (sampY * width + sampX) * 4;
					r += *in++;
					g += *in++;
					b += *in++;
					a += *in++;
				}
			}
			// average them
			r /= sampWidth * sampHeight;
			g /= sampWidth * sampHeight;
			b /= sampWidth * sampHeight;
			a /= sampWidth * sampHeight;

			r &= ~0x7;
			g &= ~0x3;
			b &= ~0x7;

			u8 *out = output + (y * outWidth + x) * 4;
			*out++ = r;
			*out++ = g;
			*out++ = b;
			*out++ = a;
		}
	}
	return output;
}

static void fix_mipmap(u8 *pixels, int width, int height, int lod, u8 *origMip)
{
	u8 *mip = generate_mipmap(pixels, width, height, lod);

	width >>= lod;
	height >>= lod;

	for (int i = 0; i < width * height; i++)
	{
		if (width <= 4)

		//if (origMip[i] < mip[i])
			printf("old: %i, new: %i, i = %i\n", origMip[i], mip[i], i);
		assert(origMip[i] >= mip[i]);
	}
	free(mip);
}
*/

static struct Color rgb565_to_color(u16 pixel)
{
	struct Color color;
	color.r = ((pixel >> 11) & 0x1F) << 3;
	color.g = ((pixel >> 5) & 0x3F) << 2;
	color.b = ((pixel >> 0) & 0x1F) << 3;
	color.a = 255;
	return color;
}

static struct Color rgb5a3_to_color(u16 pixel)
{
	struct Color color;
	if (pixel & (1 << 15))  // no alpha
	{
		color.r = ((pixel >> 10) & 0x1F) << 3;
		color.g = ((pixel >> 5) & 0x1F) << 3;
		color.b = ((pixel >> 0) & 0x1F) << 3;
		color.a = 255;
	}
	else
	{
		color.r = ((pixel >> 8) & 0xF) << 4;
		color.g = ((pixel >> 4) & 0xF) << 4;
		color.b = ((pixel >> 0) & 0xF) << 4;
		color.a = ((pixel >> 12) & 0x7) << 5;
	}
	return color;
}

static u16 color_to_rgb5a3(struct Color color)
{
	u16 pixel = 0;
	if (color.a == 255)
	{
		pixel |= 1 << 15;
		pixel |= ((color.r >> 3) & 0x1F) << 10;
		pixel |= ((color.g >> 3) & 0x1F) << 5;
		pixel |= ((color.b >> 3) & 0x1F) << 0;
	}
	else
	{
		pixel |= ((color.a >> 5) & 0x7) << 12;
		pixel |= ((color.r >> 4) & 0xF) << 8;
		pixel |= ((color.g >> 4) & 0xF) << 4;
		pixel |= ((color.b >> 4) & 0xF) << 0;
	}
	return pixel;
}

static void verify_encoder(const char *name, void *(*encode)(const u8 *, int, int, u32 *), const void *orig, const void *decoded, int width, int height)
{
	u32 size;
	void *encoded = encode(decoded, width, height, &size);
	if (memcmp(orig, encoded, size) != 0)
	{
		fprintf(stderr, "%s encoder does not match!!!\n", name);
		FILE *f;
		f = fopen("orig.bin", "wb");
		fwrite(orig, width * height * 2, 1, f);
		fclose(f);
		f = fopen("new.bin", "wb");
		fwrite(encoded, width * height * 2, 1, f);
		fclose(f);
		exit(1);
	}
	free(encoded);
}

//------------------------------------------------------------------------------
// I8
//------------------------------------------------------------------------------

static void *encode_i8(const u8 *pixels, int width, int height, u32 *size)
{
	*size = width * height;
	u8 *buffer = malloc(*size);
	u8 *out = buffer;

	for (int y = 0; y < height; y += 4)
    {
        for (int x = 0; x < width; x += 8)
        {
			// encode 8x4 tile
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 8; tx++)
				{
					if (y + ty >= height || x + tx >= width)
						continue;
					const u8 *in = pixels + 4 * ((y + ty) * width + (x + tx));
					u8 r = *in++;
					u8 g = *in++;
					u8 b = *in++;
					in++;  // alpha not used
					*out++ = (r + g + b) / 3;
				}
			}
		}
	}
	return buffer;
}

static void *decode_i8(const void *pixels, int width, int height)
{
	u8 *buffer = malloc(width * height * 4);
    const u8 *in = pixels;

	for (int y = 0; y < height; y += 4)
    {
        for (int x = 0; x < width; x += 8)
        {
			// decode 8x4 tile
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 8; tx++)
				{
					if (y + ty >= height || x + tx >= width)
						continue;
					u8 pixel = *in++;
					u8 *out = buffer + 4 * ((y + ty) * width + (x + tx));
					*out++ = pixel;
					*out++ = pixel;
					*out++ = pixel;
					*out++ = 255;
				}
			}
		}
	}
#ifdef VERIFY_ENCODER
	verify_encoder("I8", encode_i8, pixels, buffer, width, height);
#endif
	return buffer;
}

//------------------------------------------------------------------------------
// IA4
//------------------------------------------------------------------------------

static void *encode_ia4(const u8 *pixels, int width, int height, u32 *size)
{
	*size = width * height;
	u8 *buffer = malloc(*size);
	u8 *out = buffer;

	for (int y = 0; y < height; y += 4)
    {
        for (int x = 0; x < width; x += 8)
        {
			// encode 8x4 tile
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 8; tx++)
				{
					if (y + ty >= height || x + tx >= width)
						continue;
					const u8 *in = pixels + 4 * ((y + ty) * width + (x + tx));
					u8 r = *in++;
					u8 g = *in++;
					u8 b = *in++;
					u8 a = *in++;
					u8 i = (r + g + b) / 3;
					*out++ = ((i / 0x11) & 0xF) | ((a / 0x11) << 4);
				}
			}
		}
	}
	return buffer;
}

static void *decode_ia4(const void *pixels, int width, int height)
{
	u8 *buffer = malloc(width * height * 4);
    const u8 *in = pixels;

	for (int y = 0; y < height; y += 4)
    {
        for (int x = 0; x < width; x += 8)
        {
			// decode 8x4 tile
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 8; tx++)
				{
					if (y + ty >= height || x + tx >= width)
						continue;
					u8 pixel = *in++;
					u8 i = (pixel & 0xF) * 0x11;
					u8 a = (pixel >> 4) * 0x11;

					u8 *out = buffer + 4 * ((y + ty) * width + (x + tx));
					*out++ = i;
					*out++ = i;
					*out++ = i;
					*out++ = a;
				}
			}
		}
	}
#ifdef VERIFY_ENCODER
	verify_encoder("IA4", encode_ia4, pixels, buffer, width, height);
#endif
	return buffer;
}

//------------------------------------------------------------------------------
// RGB565
//------------------------------------------------------------------------------

static void *encode_rgb565(const u8 *pixels, int width, int height, u32 *size)
{
	*size = ROUND_UP(width * height * 2, 4 * 4 * 2);
	u16 *buffer = malloc(*size);
	u16 *out = buffer;
	u16 filler = 0;

	for (int y = 0; y < height; y += 4)
	{
		for (int x = 0; x < width; x += 4)
		{
			// encode 4x4 tile
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 4; tx++)
				{
					if (y + ty < height && x + tx < width)
					{
						const u8 *in = pixels + 4 * ((y + ty) * width + (x + tx));
						u8 r = *in++;
						u8 g = *in++;
						u8 b = *in++;
						u16 pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3) & 0x1F);
						*out++ = bswap16(pixel);

						// fill the rest of the block with the first pixel?
						if (tx == 0 && ty == 0)
							filler = bswap16(pixel);
					}
					else
					{
						*out++ = filler;
					}
				}
			}
		}
	}
	return buffer;
}

static void *decode_rgb565(const void *pixels, int width, int height)
{
	u8 *buffer = malloc(width * height * 4);
	const u16 *in = pixels;

	for (int y = 0; y < height; y += 4)
	{
		for (int x = 0; x < width; x += 4)
		{
			// decode 4x4 tile
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 4; tx++)
				{
					if (y + ty >= height || x + tx >= width)
					{
						in++;
					}
					else
					{
						u16 pixel = bswap16(*in++);
						u8 r = ((pixel >> 11) & 0x1F) << 3;
						u8 g = ((pixel >> 5) & 0x3F) << 2;
						u8 b = ((pixel >> 0) & 0x1F) << 3;
						u8 *out = buffer + 4 * ((y + ty) * width + (x + tx));
						*out++ = r;
						*out++ = g;
						*out++ = b;
						*out++ = 255;
					}
				}
			}
		}
	}

#ifdef VERIFY_ENCODER
	verify_encoder("RGB565", encode_rgb565, pixels, buffer, width, height);
#endif
	return buffer;
}

//------------------------------------------------------------------------------
// RGB5A3
//------------------------------------------------------------------------------

static void *encode_rgb5a3(const u8 *pixels, int width, int height, u32 *size)
{
	*size = ROUND_UP(width * height * 2, 4 * 4 * 2);
	u16 *buffer = malloc(*size);
	u16 *out = buffer;

	for (int y = 0; y < height; y += 4)
	{
		for (int x = 0; x < width; x += 4)
		{
			// encode 4x4 tile
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 4; tx++)
				{
					// TODO: handle small images
					const u8 *in = pixels + 4 * ((y + ty) * width + (x + tx));
					struct Color color;
					color.r = *in++;
					color.g = *in++;
					color.b = *in++;
					color.a = *in++;
					*out++ = bswap16(color_to_rgb5a3(color));
				}
			}
		}
	}
	return buffer;
}

static void *decode_rgb5a3(const void *pixels, int width, int height)
{
	u8 *buffer = malloc(width * height * 4);
	const u16 *in = pixels;

	for (int y = 0; y < height; y += 4)
	{
		for (int x = 0; x < width; x += 4)
		{
			// decode 4x4 tile
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 4; tx++)
				{
					if (y + ty >= height || x + tx >= width)
						continue;
					u8 *out = buffer + 4 * ((y + ty) * width + (x + tx));
					struct Color color = rgb5a3_to_color(bswap16(*in++));
					*out++ = color.r;
					*out++ = color.g;
					*out++ = color.b;
					*out++ = color.a;
				}
			}
		}
	}

#ifdef VERIFY_ENCODER
	verify_encoder("RGB5A3", encode_rgb5a3, pixels, buffer, width, height);
#endif
	return buffer;
}

//------------------------------------------------------------------------------
// RGBA8
//------------------------------------------------------------------------------

static void *decode_rgba8(const void *pixels, int width, int height)
{
	u8 *buffer = malloc(width * height * 4);
	const u8 *in = pixels;

	for (int y = 0; y < height; y += 4)
	{
		for (int x = 0; x < width; x += 4)
		{
			// decode 4x4 tile
			for (int ty = 0; ty < 4; ty++)
			{
				for (int tx = 0; tx < 4; tx++)
				{
					if (tx < width && ty < height)
					{
						u8 *out = buffer + 4 * ((y + ty) * width + (x + tx));
						*out++ = in[1];  // r
						*out++ = in[32];  // g
						*out++ = in[33];  // b
						*out++ = in[0];  // a
					}
					in += 2;
				}
			}
			in += 32;
		}
	}
	return buffer;
}

//------------------------------------------------------------------------------
// CMPR
//------------------------------------------------------------------------------

static u8 swap_color_bits(u8 x)
{
	return (
		((x & 0x03) << 6) | // XXXXXXDD -> DDXXXXXX
		((x & 0x0C) << 2) | // XXXXCCXX -> XXCCXXXX
		((x & 0x30) >> 2) | // XXBBXXXX -> XXXXBBXX
		((x & 0xC0) >> 6));  // AAXXXXXX -> XXXXXXAA
}

static void encode_cmpr_block2(const u8 *restrict src, u8 *restrict dest, int x, int y, int width, int height)
{
	u8 buffer[4 * 4 * 4];
	bool alphaBlock = false;

	// copy the data into a contiguous array
	for (int ty = 0; ty < 4; ty++)
	{
		const u8 *in = src + ((y + ty) * width + x) * 4;
		memcpy(buffer + ty * 4 * 4, in, 4 * 4);
	}
	
	int alphaCount = 0;
	for (int i = 3; i < 64; i += 4)
	{
		if (buffer[i] <= 128)
			alphaCount++;
	}
	
	if (alphaCount >= 3)
	{
		alphaBlock = true;
	}
	else if (alphaCount > 0)
	{
		// If there are alpha pixels in a non-alpha block
		// Change the alpha pixels to a color in the palette
		u8 templateTile[4];
		// Gets a template color to set the Alpha pixels to
		for (int i = 0; i < 64; i += 4)
		{
			if (buffer[i + 3] == 255)
			{
				templateTile[0] = buffer[i];
				templateTile[1] = buffer[i + 1];
				templateTile[2] = buffer[i + 2];
				templateTile[3] = buffer[i + 3];
			}
		}
		// Go through and set the alpha pixels to the template color
		for (int i = 0; i < 64; i += 4)
		{
			if (buffer[i + 3] <= 128)
			{
				buffer[i] = templateTile[0];
				buffer[i + 1] = templateTile[1];
				buffer[i + 2] = templateTile[2];
				buffer[i + 3] = templateTile[3];
			}
		}
	}
	
	stb_compress_dxt_block(dest, buffer, alphaBlock, STB_DXT_HIGHQUAL);
	// convert from standard DXT format to GX format
	u8 temp;
	temp = dest[0];
	dest[0] = dest[1];
	dest[1] = temp;
	temp = dest[2];
	dest[2] = dest[3];
	dest[3] = temp;
	for (int i = 0; i < 4; i++)
		dest[4 + i] = swap_color_bits(dest[4 + i]);
}

static void *encode_cmpr(const u8 *pixels, int width, int height, u32 *size)
{
	*size = width * height / 2;
    u8 *buffer = malloc(*size);
    u8 *out = buffer;

	printf("%i x %i\n", width, height);

    for (int y = 0; y < height; y += 8)
    {
        for (int x = 0; x < width; x += 8)
        {
            // encode tile (split into 4x4 blocks)
            encode_cmpr_block2(pixels, out, x + 0, y + 0, width, height);
            out += 8;
			encode_cmpr_block2(pixels, out, x + 4, y + 0, width, height);
            out += 8;
			encode_cmpr_block2(pixels, out, x + 0, y + 4, width, height);
            out += 8;
			encode_cmpr_block2(pixels, out, x + 4, y + 4, width, height);
            out += 8;
        }
    }
    return buffer;
}

static u8 s3tc_blend(u32 a, u32 b)
{
	// (a * 3 + b * 5) / 8
    return ((((a << 1) + a) + ((b << 2) + b)) >> 3);
}

static u8 half_blend(u32 a, u32 b)
{
    return (a + b) >> 1;
}

// Decodes a 4x4 compressed block
static void decode_cmpr_block(const u8 *restrict src, u8 *restrict dest, int x, int y, int width, int height)
{
    u16 c1 = (src[0] << 8) | src[1];
    u16 c2 = (src[2] << 8) | src[3];
    struct Color colors[4] = {0};
    int tx, ty;

    src += 4;
    colors[0] = rgb565_to_color(c1);
    colors[1] = rgb565_to_color(c2);
    if (c1 > c2)
    {
		// 3/8 of 1, 5/8 of 0
        colors[2].r = s3tc_blend(colors[1].r, colors[0].r);
        colors[2].g = s3tc_blend(colors[1].g, colors[0].g);
        colors[2].b = s3tc_blend(colors[1].b, colors[0].b);
        colors[2].a = 255;
        // 3/8 of 0, 5/8 of 1
        colors[3].r = s3tc_blend(colors[0].r, colors[1].r);
        colors[3].g = s3tc_blend(colors[0].g, colors[1].g);
        colors[3].b = s3tc_blend(colors[0].b, colors[1].b);
        colors[3].a = 255;
    }
    else
    {
		// in between both
        colors[2].r = half_blend(colors[0].r, colors[1].r);
        colors[2].g = half_blend(colors[0].g, colors[1].g);
        colors[2].b = half_blend(colors[0].b, colors[1].b);
        colors[2].a = 255;
        colors[3] = colors[2];
        colors[3].a = 0;
    }

	printf("alpha? %i\n", c1 <= c2);
	printf("palette:\n");
	for (int i = 0; i < 4; i++)
	{
		printf("  %i,%i,%i,%i\n", colors[i].r, colors[i].g, colors[i].b, colors[i].a);
	}

    for (ty = 0; ty < 4; ty++)
    {
        u32 val = *src++;
        for (tx = 0; tx < 4; tx++)
        {
			if (x + tx < width && y + ty < height)
			{
				struct Color color = colors[(val >> 6) & 3];
				int index = (y + ty) * width + (x + tx);
				dest[index*4 + 0] = color.r;
				dest[index*4 + 1] = color.g;
				dest[index*4 + 2] = color.b;
				dest[index*4 + 3] = color.a;
			}
            val <<= 2;
        }
    }
}

static void *decode_cmpr(const void *pixels, int width, int height)
{
    u8 *buffer = malloc(width * height * 4);
    const u8 *in = pixels;

    for (int y = 0; y < height; y += 8)
    {
        for (int x = 0; x < width; x += 8)
        {
            // decode tile (split into 4x4 blocks)
            decode_cmpr_block(in, buffer, x + 0, y + 0, width, height);
            in += 8;
			decode_cmpr_block(in, buffer, x + 4, y + 0, width, height);
            in += 8;
			decode_cmpr_block(in, buffer, x + 0, y + 4, width, height);
            in += 8;
			decode_cmpr_block(in, buffer, x + 4, y + 4, width, height);
            in += 8;
        }
    }
    return buffer;
}

struct TextureCodec
{
	const char *name;
	int bpp;
	int tileWidth, tileHeight;
	void *(*decode)(const void *, int, int);
	void *(*encode)(const u8 *, int, int, u32 *);
};

static const struct TextureCodec codecs[] =
{
	//                 name       bpp  tile width  tile height  decode         encode
	[GX_TF_I8]     = { "i8",      8,   8,          4,           decode_i8,     encode_i8 },
	[GX_TF_IA4]    = { "ia4",     8,   8,          4,           decode_ia4,    encode_ia4 },
	[GX_TF_RGB565] = { "rgb565",  16,  4,          4,           decode_rgb565, encode_rgb565 },
	[GX_TF_RGB5A3] = { "rgb5a3",  16,  4,          4,           decode_rgb5a3, encode_rgb5a3 },
	[GX_TF_RGBA8]  = { "rgba8",   32,  4,          4,           decode_rgba8,  NULL },
	[GX_TF_CMPR]   = { "cmpr",    4,   8,          8,           decode_cmpr,   encode_cmpr },
};

static const struct TextureCodec *get_codec(unsigned int fmt)
{
	if (fmt < ARRAY_COUNT(codecs) && codecs[fmt].name != NULL)
		return &codecs[fmt];
	return NULL;
}

static const struct TextureCodec *get_codec_by_name(const char *name)
{
	const struct TextureCodec *codec = &codecs[0];
	for (int i = 0; i < ARRAY_COUNT(codecs); i++, codec++)
	{
		if (codec->name != NULL && strcmp(codec->name, name) == 0)
			return codec;
	}
	return NULL;
}

static bool extract_tpl(FILE *tplFile)
{
	u32 numTextures;
	u32 i;

	if (!read_u32(tplFile, &numTextures))
		goto read_error;

	printf("%u textures\n", numTextures);

	for (i = 0; i < numTextures; i++)
	{
		u32 format;
		u32 imageOffset;
		u16 width;
		u16 height;
		u16 mipmaps;
		u16 dummy;
		u8 *buffer;
		size_t size;
		long int filePos;
		char filename[64];
		const struct TextureCodec *codec;

		printf("header at 0x%lX\n", ftell(tplFile));
		if (!read_u32(tplFile, &format)
		 || !read_u32(tplFile, &imageOffset)
		 || !read_u16(tplFile, &width)
		 || !read_u16(tplFile, &height)
		 || !read_u16(tplFile, &mipmaps)
		 || !read_u16(tplFile, &dummy))
			goto read_error;
		codec = get_codec(format);
		printf("format: %u (%s), offset: 0x%X, width: %u, height: %u, mipmaps: %u\n",
			format, codec ? codec->name : "unknown", imageOffset, width, height, mipmaps);
		// no idea what's the matter with these...
		if (format == 0x100
		 || format == 0x5F677573
		 || format == 0xF2FF27D4
		 || format == 0x6C5F7361
		 || format == 0x73756F79
		 || format == 0x0031315F
		 || format == 0x2E31315F)
			continue;
		if (codec == NULL)
		{
			fprintf(stderr, "Unsupported texture format %u\n", format);
			return false;
		}

		filePos = ftell(tplFile);
		fseek(tplFile, imageOffset, SEEK_SET);
		for (int j = 0; j < mipmaps; j++)
		{
			int paddedWidth = ROUND_UP(width, codec->tileWidth);
			int paddedHeight = ROUND_UP(height, codec->tileHeight);

			size = (paddedWidth * paddedHeight * codec->bpp + 7) / 8;
			buffer = malloc(size);
			if (buffer == NULL)
			{
				fputs("Out of memory!\n", stderr);
				return false;
			}
			printf("mip %i: %i x %i, %li bytes at 0x%lX\n", j, width, height, size, ftell(tplFile));
			//printf("reading %i bytes at 0x%X\n", size, ftell(tplFile));
			if (fread(buffer, size, 1, tplFile) != 1)
				goto read_error;
			sprintf(filename, "img%u.mip%u.png", i, j);
			codec = get_codec(format);
			if (codec == NULL)
			{
				fprintf(stderr, "Invalid texture format %u\n", format);
				return false;
			}
			void *decoded = codec->decode(buffer, width, height);
			free(buffer);
			lodepng_encode32_file(filename, decoded, width, height);
			free(decoded);

			width >>= 1;
			height >>= 1;
		}
		fseek(tplFile, filePos, SEEK_SET);
	}

	return true;

read_error:
	fprintf(stderr, "Error reading file!: %s\n", feof(tplFile) ? "End of file reached" : "Unknown error");
	return false;
}

static bool has_extension(const char *filename, const char *ext)
{
	const char *dot = strrchr(filename, '.');
	return (dot != NULL) && (strcmp(dot, ext) == 0);
}

struct InputDef
{
	const char *filename;
	GXTexFmt format;
};

static bool create_tpl(FILE *tplFile, const struct InputDef *inputs, int inputsCount)
{
	u32 imageOffset = 4 + inputsCount * 0x10;  // after headers
	const struct InputDef *input = &inputs[0];

	if (!write_u32(tplFile, inputsCount))
		goto write_error;

	for (int i = 0; i < inputsCount; i++, input++)
	{
		u32 headerOffset = 4 + i * 0x10;
		//imageOffset = (imageOffset + 0x1F) & ~0x1F;
		const struct TextureCodec *codec = get_codec(input->format);

		assert(codec != NULL);

		imageOffset = ROUND_UP(imageOffset, codec->tileWidth * codec->tileHeight * codec->bpp / 8);

		fseek(tplFile, headerOffset, SEEK_SET);
		if (!write_u32(tplFile, input->format)
		 || !write_u32(tplFile, imageOffset))
			goto write_error;

		if (has_extension(input->filename, ".png"))
		{
			// TODO: load file without mipmaps
		}
		else  // basename of file
		{
			char filename[256];
			unsigned int error = 0;
			unsigned int baseWidth, baseHeight;
			unsigned int lod = 0;

			while (true)
			{
				u8 *image;
				unsigned int width, height;

				sprintf(filename, "%s.mip%u.png", inputs[i].filename, lod);
				error = lodepng_decode32_file(&image, &width, &height, filename);
				if (error == 0)
				{
					if (lod == 0)
					{
						// TODO: check pow 2
						baseWidth = width;
						baseHeight = height;
						if (!write_u16(tplFile, width) || !write_u16(tplFile, height))
							goto write_error;
					}
					else
					{
						if (width != baseWidth >> lod || height != baseHeight >> lod)
						{
							fprintf(stderr, "Error: mipmap level %u must be %u x %u\n", lod, baseWidth >> lod, baseHeight >> lod);
							return false;
						}
					}
					u32 size;
					u8 *encoded = codec->encode(image, width, height, &size);
					fseek(tplFile, imageOffset, SEEK_SET);
					if (fwrite(encoded, size, 1, tplFile) != 1)
						goto write_error;
					free(encoded);
					printf("added %s (%i bytes) %i x %i at 0x%X\n", filename, size, width, height, imageOffset);
					imageOffset += size;

					imageOffset = ROUND_UP(imageOffset, codec->tileWidth * codec->tileHeight * codec->bpp / 8);
				}
				else if (error != 78)  // some error other than failing to open the file
					fprintf(stderr, "Error decoding %s: %s\n", inputs[i].filename, lodepng_error_text(error));
				free(image);
				if (error != 0)  // failed to open file
					break;
				lod++;
			}
			printf("lod %u\n", lod);
			if (lod == 0)
			{
				fprintf(stderr, "could not find any images matching %s.mip*.png\n", inputs[i].filename);
				return false;
			}
			// finish writing header
			fseek(tplFile, headerOffset + 0xC, SEEK_SET);
			if (!write_u16(tplFile, lod) || !write_u16(tplFile, 0x1234))
				goto write_error;
			//imageOffset += 0x20;  // hmm there's some extra data here
		}
	}

	// fill empty space (needed for matching)
	u32 headerOffset = 4 + inputsCount * 0x10;
	int numBytes = ROUND_UP(headerOffset, 32) - headerOffset;
	fseek(tplFile, headerOffset, SEEK_SET);
	for (int i = 0; i < numBytes; i++)
		fputc(i, tplFile);

	return true;

write_error:
	fprintf(stderr, "Error writing file: %s\n", strerror(errno));
	return false;
}

int main(int argc, char **argv)
{
	bool succeeded;
	bool extract = false;
	struct InputDef inputs[argc - 1];
	int inputsCount = 0;
	const char *tplFilename = NULL;

	for (int i = 1; i < argc; i++)
	{
		const char *arg = argv[i];

		if (arg[0] == '-')
		{
			if (strcmp(arg, "-x") == 0)
			{
				extract = true;
				continue;
			}
			// check if a codec option was specified
			const struct TextureCodec *codec = get_codec_by_name(arg + 1);
			if (codec != NULL && i + 1 < argc)
			{
				arg = argv[++i];
				if (arg[0] != '-')
				{
					inputs[inputsCount].format = codec - codecs;
					inputs[inputsCount].filename = arg;
					inputsCount++;
					continue;
				}
			}
			goto usage;
		}
		else if (tplFilename == NULL)
		{
			tplFilename = arg;
			continue;
		}
		goto usage;
	}

	if (tplFilename == NULL)
		goto usage;

	if (!extract && inputsCount == 0)
	{
		fputs("No image files specified\n", stderr);
		return 1;
	}

	puts(tplFilename);

	const char *mode = extract ? "rb" : "wb";
	FILE *tplFile = fopen(tplFilename, mode);
	if (tplFile == NULL)
	{
		fprintf(stderr, "Could not open %s: %s\n", tplFilename, strerror(errno));
		return 1;
	}
	succeeded = extract ? extract_tpl(tplFile) : create_tpl(tplFile, inputs, inputsCount);
	fclose(tplFile);

	return succeeded ? 0 : 1;

usage:
	printf("usage: %s [-x] tpl_file ...\n", argv[0]);
	return 1;
}
