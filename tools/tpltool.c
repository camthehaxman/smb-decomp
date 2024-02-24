#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "lodepng.h"

#define ARRAY_COUNT(arr) (sizeof(arr)/sizeof(arr[0]))

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
						continue;
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
	return buffer;
}

static void *encode_rgb565(const u8 *pixels, int width, int height, u32 *size)
{
	*size = width * height * 2;
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
					if (y + ty >= height || x + tx >= width)
						continue;
					const u8 *in = pixels + 4 * ((y + ty) * width + (x + tx));
					u8 r = *in++;
					u8 g = *in++;
					u8 b = *in++;
					u16 pixel = ((r >> 3) << 11) | ((g >> 2) << 5) | ((b >> 3) & 0x1F);
					*out++ = bswap16(pixel);
				}
			}
		}
	}
	return buffer;
}

struct TextureCodec
{
	const char *name;
	void *(*decode)(const void *, int, int);
	void *(*encode)(const u8 *, int, int, u32 *);
};

static const struct TextureCodec codecs[] =
{
	[GX_TF_RGB565] = { "rgb565", decode_rgb565, encode_rgb565 },
};

static const struct TextureCodec *get_codec(unsigned int fmt)
{
	if (fmt >= ARRAY_COUNT(codecs))
		return NULL;
	if (codecs[fmt].name == NULL)
		return NULL;
	return &codecs[fmt];
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

		if (!read_u32(tplFile, &format)
		 || !read_u32(tplFile, &imageOffset)
		 || !read_u16(tplFile, &width)
		 || !read_u16(tplFile, &height)
		 || !read_u16(tplFile, &mipmaps)
		 || !read_u16(tplFile, &dummy))
			goto read_error;
		printf("format: %u, offset: 0x%X, width: %u, height: %u, mipmaps: %u\n", format, imageOffset, width, height, mipmaps);
		filePos = ftell(tplFile);
		fseek(tplFile, imageOffset, SEEK_SET);
		for (int j = 0; j < mipmaps; j++)
		{
			printf("mip %i: %ix%i\n", j, width, height);
			switch (format)
			{
			case GX_TF_RGB565:
				size = width * height * 2;
				break;
			default:
				abort();
			}
			buffer = malloc(size);
			if (buffer == NULL)
			{
				fputs("Out of memory!\n", stderr);
				return false;
			}
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
	fprintf(stderr, "Error reading file: %s\n", strerror(errno));
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

	if (inputsCount == 0)
	{
		fputs("No image files specified\n", stderr);
		return false;
	}
	if (!write_u32(tplFile, inputsCount))
		goto write_error;

	for (int i = 0; i < inputsCount; i++, input++)
	{
		u32 headerOffset = 4 + i * 0x10;
		imageOffset = (imageOffset + 0x1F) & ~0x1F;
		const struct TextureCodec *codec = get_codec(input->format);

		assert(codec != NULL);

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
					printf("at 0x%X\n", imageOffset);
					imageOffset += size;
					printf("added %s\n", filename);
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
			imageOffset += 0x20;  // hmm there's some extra data here
		}
	}

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
