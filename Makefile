ifneq (,$(findstring Windows,$(OS)))
  EXE := .exe
else
  WINE ?= wine
endif

COMPILER_VERSION ?= 1.1

VERBOSE ?= 0

# Don't echo build commands unless VERBOSE is set
ifeq ($(VERBOSE),0)
  QUIET := @
endif

COMPILER ?= mwcc

# default recipe
default: all

#-------------------------------------------------------------------------------
# Tools
#-------------------------------------------------------------------------------

DEVKITPPC ?=

COMPILER_DIR := mwcc_compiler/$(COMPILER_VERSION)
AS      := $(DEVKITPPC)/bin/powerpc-eabi-as
MWCC    := $(WINE) $(COMPILER_DIR)/mwcceppc.exe
LD      := $(WINE) $(COMPILER_DIR)/mwldeppc.exe
OBJCOPY := $(DEVKITPPC)/bin/powerpc-eabi-objcopy
OBJDUMP := $(DEVKITPPC)/bin/powerpc-eabi-objdump
GCC     := $(DEVKITPPC)/bin/powerpc-eabi-gcc
HOSTCC  := cc
SHA1SUM := sha1sum
ELF2DOL := tools/elf2dol$(EXE)
ELF2REL := tools/elf2rel$(EXE)
LZSS    := tools/lzss$(EXE)

# Game include directories
INCLUDE_DIRS := src data
SYSTEM_INCLUDE_DIRS := include

ASFLAGS     := -mgekko -I asm

# Metrowerks compiler flags
MWCC_CFLAGS      := -sym on -O4,p -inline auto -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn pragmas -pragma 'cats off'
#MWCC_CFLAGS      := -opt level=0 -nodefaults -proc gekko -fp hard -Cpp_exceptions off -enum int -warn pragmas -pragma 'cats off' -DDEBUG
MWCC_CPPFLAGS     = $(addprefix -i ,$(INCLUDE_DIRS) $(dir $^)) -I- $(addprefix -i ,$(SYSTEM_INCLUDE_DIRS))
# GNU compiler flags
GCC_CFLAGS       := -O2 -Wall -Wextra -Wno-unused -Wno-switch -Wno-main -Wno-unknown-pragmas \
                    -Wno-unused-variable -Wno-unused-parameter -Wno-sign-compare -Wno-strict-aliasing \
                    -Wno-missing-field-initializers -Wno-char-subscripts -Wno-empty-body \
                    -fno-jump-tables -fno-builtin -fsigned-char -fno-asynchronous-unwind-tables -mno-gnu-attribute
GCC_CPPFLAGS     := -nostdinc $(addprefix -I ,$(INCLUDE_DIRS) $(SYSTEM_INCLUDE_DIRS)) -DNONMATCHING -DC_ONLY

ifeq ($(COMPILER),mwcc)
  CC        = $(MWCC)
  CFLAGS    = $(MWCC_CFLAGS)
  CPPFLAGS  = $(MWCC_CPPFLAGS)
  REL_FLAGS := -sdata 0 -sdata2 0 -g
else
  CC        = $(GCC)
  CFLAGS    = $(GCC_CFLAGS)
  CPPFLAGS  = $(GCC_CPPFLAGS)
  REL_FLAGS := -mno-sdata
endif

DOL_LDFLAGS := -nodefaults -fp hard
REL_LDFLAGS := -nodefaults -fp hard -r1 -m _prolog -g

HOSTCFLAGS   := -Wall -O3 -s

CC_CHECK     := $(GCC) $(GCC_CFLAGS) -fsyntax-only $(GCC_CPPFLAGS)

#-------------------------------------------------------------------------------
# Files
#-------------------------------------------------------------------------------

BASEROM  := baserom.bin
DOL      := supermonkeyball.dol
ELF      := $(DOL:.dol=.elf)
MAP      := $(DOL:.dol=.map)

DOL_LCF := static.lcf
REL_LCF := partial.lcf

# main dol sources
SOURCES := \
	asm/c++_exception_data.s \
	src/main.c \
	src/init.c \
	src/init_2.c \
	src/mathutil.c \
	src/mode.c \
	src/pause_menu.c \
	src/event.c \
	src/polydisp.c \
	src/adv.c \
	src/code_5.c \
	src/sel.c \
	src/game.c \
	src/camera.c \
	src/frustum.c \
	src/light.c \
	src/gxsync.c \
	src/info.c \
	src/code_7.c \
	src/input.c \
	src/bitmap.c \
	src/bmp_list_com.c \
	src/bmp_list_adv.c \
	src/bmp_list_end.c \
	src/bmp_list_rnk.c \
	src/bmp_list_sel.c \
	src/bmp_list_nml.c \
	src/bmp_list_bwl.c \
	src/bmp_list_rac.c \
	src/bmp_list_bil.c \
	src/bmp_list_fgt.c \
	src/bmp_list_glf.c \
	src/bmp_list_tgt.c \
	src/bmp_list_how.c \
	src/bmp_list_cmd.c \
	src/trig_tables.c \
	src/perf.c \
	src/sound.c \
	src/window.c \
	src/pool.c \
	src/nl2ngc.c \
	src/motload.c \
	src/mot_joint.c \
	src/motload_3.c \
	src/motload_4.c \
	src/ball.c \
	src/stcoli.c \
	src/world.c \
	src/interpolate_keyframes.c \
	src/stage.c \
	src/code_8.c \
	src/recplay.c \
	src/recplay_cmpr.c \
	src/effect.c \
	src/background.c \
	src/bg_old_bluesky.c \
	src/bg_old_cave.c \
	src/bg_old_extramaster.c \
	src/bg_old_night.c \
	src/bg_old_space.c \
	src/bg_old_sunset.c \
	src/bg_old_bonus.c \
	src/bg_old_ice.c \
	src/bg_old_sand.c \
	src/bg_old_storm.c \
	src/bg_old_water.c \
	src/bg_jungle.c \
	src/bg_sand.c \
	src/bg_water.c \
	src/bg_space.c \
	src/bg_sunset.c \
	src/bg_bonus.c \
	src/bg_storm.c \
	src/bg_master.c \
	src/bg_pilot.c \
	src/bg_end.c \
	src/course.c \
	src/item.c \
	src/item_coin.c \
	src/item_pilot.c \
	src/obj_collision.c \
	src/stobj.c \
	src/stobj_goal.c \
	src/sprite.c \
	src/textbox.c \
	src/hud.c \
	src/ape_icon.c \
	src/help.c \
	src/unk_anim_data.c \
	src/minimap.c \
	src/ord_tbl.c \
	src/code_3.c \
	src/ranking_screen.c \
	src/mot_ape.c \
	src/spline.c \
	src/thread.c \
	src/mt_effect.c \
	src/lzs_decompress.c \
	src/avdisp.c \
	src/load.c \
	src/shadow.c \
	src/mini.c \
	src/lens_flare.c \
	src/code_9.c \
	src/poly_shadow.c \
	src/mouse.c \
	src/rend_efc.c \
	src/rend_efc_mirror.c \
	src/rend_efc_3.c \
	src/relocation.c \
	src/gxutil.c \
	src/mini_commend.c \
	src/gxcache.c \
	src/memcard.c \
	src/DEMOPuts.c \
	src/view.c \
	src/code_6.c \
	asm/mini_ranking.s \
	src/dvd.c \
	src/preview.c \
	src/name_entry.c \
	asm/credits.s \
	src/vibration.c \
	src/ending.c \
	src/ending_camera.c \
	src/ending_dialogue.c \
	libraries/base/asm/PPCArch.s \
	libraries/os/__start.c \
	libraries/os/asm/OS.s \
	libraries/os/asm/OSAlarm.s \
	libraries/os/OSAlloc.c \
	libraries/os/OSArena.c \
	libraries/os/OSAudioSystem.c \
	libraries/os/asm/OSCache.s \
	libraries/os/asm/OSContext.s \
	libraries/os/OSError.c \
	libraries/os/asm/OSExi.s \
	libraries/os/asm/OSFont.s \
	libraries/os/asm/OSInterrupt.s \
	libraries/os/asm/OSLink.s \
	libraries/os/OSMemory.c \
	libraries/os/OSMutex.c \
	libraries/os/asm/OSReboot.s \
	libraries/os/asm/OSReset.s \
	libraries/os/asm/OSResetSW.s \
	libraries/os/asm/OSRtc.s \
	libraries/si/asm/SIBios.s \
	libraries/os/OSSync.c \
	libraries/os/OSThread.c \
	libraries/os/asm/OSTime.s \
	libraries/exi/asm/EXIUart.s \
	libraries/os/__ppc_eabi_init.c \
	libraries/db/db.c \
	libraries/mtx/asm/mtx.s \
	libraries/mtx/mtx44.c \
	libraries/mtx/asm/vec.s \
	libraries/dvd/asm/dvdlow.s \
	libraries/dvd/asm/dvdfs.s \
	libraries/dvd/asm/dvd.s \
	libraries/dvd/dvdqueue.c \
	libraries/dvd/asm/dvderror.s \
	libraries/dvd/fstload.c \
	libraries/vi/asm/vi.s \
	libraries/demo/DEMOFont.c \
	libraries/pad/Pad.c \
	libraries/ai/ai.c \
	libraries/ar/asm/ar.s \
	libraries/ar/asm/arq.s \
	libraries/dsp/dsp.c \
	libraries/dsp/dsp_debug.c \
	libraries/dsp/dsp_task.c \
	libraries/card/CARDBios.c \
	libraries/card/CARDUnlock.c \
	libraries/card/CARDRdwr.c \
	libraries/card/CARDBlock.c \
	libraries/card/CARDDir.c \
	libraries/card/CARDCheck.c \
	libraries/card/CARDMount.c \
	libraries/card/CARDFormat.c \
	libraries/card/CARDOpen.c \
	libraries/card/CARDCreate.c \
	libraries/card/CARDRead.c \
	libraries/card/CARDWrite.c \
	libraries/card/CARDDelete.c \
	libraries/card/CARDStat.c \
	libraries/card/CARDRename.c \
	libraries/hio/hio.c \
	libraries/gx/GXInit.c \
	libraries/gx/GXFifo.c \
	libraries/gx/asm/GXAttr.s \
	libraries/gx/asm/GXMisc.s \
	libraries/gx/GXGeometry.c \
	libraries/gx/asm/GXFrameBuf.s \
	libraries/gx/GXLight.c \
	libraries/gx/asm/GXTexture.s \
	libraries/gx/asm/GXBump.s \
	libraries/gx/GXTev.c \
	libraries/gx/asm/GXPixel.s \
	libraries/gx/asm/GXDraw.s \
	libraries/gx/GXStubs.c \
	libraries/gx/GXDisplayList.c \
	libraries/gx/GXTransform.c \
	libraries/gx/GXPerf.c \
	libraries/perf/asm/perf.s \
	libraries/perf/asm/perfdraw.s \
	libraries/musyx/asm/seq.s \
	libraries/musyx/asm/synth.s \
	libraries/musyx/seq_api.c \
	libraries/musyx/snd_synthapi.c \
	libraries/musyx/asm/stream.s \
	libraries/musyx/asm/synthdata.s \
	libraries/musyx/asm/synthmacros.s \
	libraries/musyx/asm/synthvoice.s \
	libraries/musyx/synth_ac.c \
	libraries/musyx/asm/synth_adsr.s \
	libraries/musyx/synth_dbtab.c \
	libraries/musyx/asm/s_data.s \
	libraries/musyx/asm/hw_dspctrl.s \
	libraries/musyx/hw_volconv.c \
	libraries/musyx/snd3d.c \
	libraries/musyx/asm/synth_2.s \
	libraries/musyx/snd_math.c \
	libraries/musyx/asm/snd_midictrl.s \
	libraries/musyx/snd_service.c \
	libraries/musyx/HARDWARE.c \
	libraries/musyx/hw_aramdma.c \
	libraries/musyx/hw_dolphin.c \
	libraries/musyx/hw_memory.c \
	libraries/musyx/reverb_fx.c \
	libraries/musyx/asm/reverb.s \
	libraries/musyx/asm/chorus_fx.s \
	libraries/dtk/asm/dtk.s \
	libraries/PowerPC_EABI_Support/Runtime/Src/__mem.c \
	libraries/PowerPC_EABI_Support/Runtime/Src/__va_arg.c \
	libraries/PowerPC_EABI_Support/Runtime/Src/global_destructor_chain.c \
	libraries/PowerPC_EABI_Support/Runtime/Src/ExceptionPPC.cp \
	libraries/PowerPC_EABI_Support/asm/Runtime/Src/runtime.s \
	libraries/PowerPC_EABI_Support/Runtime/Src/__init_cpp_exceptions.cpp \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/Src/abort_exit.c \
	libraries/PowerPC_EABI_Support/asm/Msl/MSL_C/MSL_Common_Embedded/Src/ansi_fp.s \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/Src/buffer_io.c \
	libraries/PowerPC_EABI_Support/asm/Msl/MSL_C/MSL_Common/Src/char_io.s \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/PPC_EABI/SRC/critical_regions.ppc_eabi.c \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/Src/ctype.c \
	libraries/PowerPC_EABI_Support/asm/Msl/MSL_C/MSL_Common/Src/direct_io.s \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/Src/mbstring.c \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/Src/mem.c \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/Src/mem_funcs.c \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/Src/misc_io.c \
	libraries/PowerPC_EABI_Support/asm/Msl/MSL_C/MSL_Common/Src/printf.s \
	libraries/PowerPC_EABI_Support/asm/Msl/MSL_C/MSL_Common/Src/float.s \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/Src/rand.c \
	libraries/PowerPC_EABI_Support/asm/Msl/MSL_C/MSL_Common/Src/string.s \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/Src/wchar_io.c \
	libraries/PowerPC_EABI_Support/math/s_fabs.c \
	libraries/PowerPC_EABI_Support/math/scalbn.c \
	libraries/PowerPC_EABI_Support/math/e_rem_pio2.c \
	libraries/PowerPC_EABI_Support/math/fminmaxdim.c \
	libraries/PowerPC_EABI_Support/math/k_cos.c \
	libraries/PowerPC_EABI_Support/math/k_rem_pio2.c \
	libraries/PowerPC_EABI_Support/math/k_sin.c \
	libraries/PowerPC_EABI_Support/math/s_copysign.c \
	libraries/PowerPC_EABI_Support/math/s_cos.c \
	libraries/PowerPC_EABI_Support/math/s_floor.c \
	libraries/PowerPC_EABI_Support/math/s_frexp.c \
	libraries/PowerPC_EABI_Support/math/s_ldexp.c \
	libraries/PowerPC_EABI_Support/math/s_sin.c \
	libraries/PowerPC_EABI_Support/asm/math/inverse_trig.s \
	libraries/PowerPC_EABI_Support/asm/math/trigf.s \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common_Embedded/Math/Single_precision/common_float_tables.c \
	libraries/PowerPC_EABI_Support/asm/Msl/MSL_C/MSL_Common_Embedded/Math/Single_precision/exponentialsf.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/mainloop.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/nubevent.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/nubinit.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/msg.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/msgbuf.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/serpoll.s \
	libraries/TRK_MINNOW_DOLPHIN/usr_put.c \
	libraries/TRK_MINNOW_DOLPHIN/asm/dispatch.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/msghndlr.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/support.s \
	libraries/TRK_MINNOW_DOLPHIN/mutex_TRK.c \
	libraries/TRK_MINNOW_DOLPHIN/asm/notify.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/flush_cache.s \
	libraries/TRK_MINNOW_DOLPHIN/Portable/mem_TRK.c \
	libraries/TRK_MINNOW_DOLPHIN/asm/targimpl.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/__exception.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/dolphin_trk.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/mpc_7xx_603e.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/main_TRK.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/dolphin_trk_glue.s \
	libraries/TRK_MINNOW_DOLPHIN/asm/targcont.s \
	libraries/amcExi2/AmcExi.c \
	libraries/amcExi2/AmcExi2Comm.c \
	libraries/odemustubs/asm/odemustubs.s \
	libraries/amcnotstub/amcnotstub.c
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES := $(O_FILES)
$(ELF): $(O_FILES)

# mkbe.sel_ngc.rel sources
SOURCES := \
	src/sel_ngc_rel.c \
	asm/sel_ngc_rel.s
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.sel_ngc.plf: $(O_FILES)
mkbe.sel_ngc.rel: ELF2REL_ARGS := -i 1 -o 0x0 -l 0x1D -c 18
ALL_RELS += mkbe.sel_ngc.rel

# mkbe.sel_stage.rel sources
SOURCES := \
	src/sel_stage_rel.c \
	src/sel_stage_rel_2.c \
	src/sel_stage_rel_3.c
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.sel_stage.plf: $(O_FILES)
mkbe.sel_stage.rel: ELF2REL_ARGS := -i 2 -o 0x1D -l 0x1F
ALL_RELS += mkbe.sel_stage.rel

# mkbe.rel_mini_race.rel sources
SOURCES := \
	src/mini_race.c \
	asm/mini_race.s
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.rel_mini_race.plf: $(O_FILES)
mkbe.rel_mini_race.rel: ELF2REL_ARGS := -i 3 -o 0x3C -l 0x23 -c 18
ALL_RELS += mkbe.rel_mini_race.rel

# mkbe.rel_mini_bowling.rel sources
SOURCES := \
	src/mini_bowling.c \
	asm/mini_bowling.s
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.rel_mini_bowling.plf: $(O_FILES)
mkbe.rel_mini_bowling.rel: ELF2REL_ARGS := -i 4 -o 0x5F -l 0x26 -c 18
ALL_RELS += mkbe.rel_mini_bowling.rel

# mkbe.rel_mini_fight.rel sources
SOURCES := \
	asm/mini_fight.s
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.rel_mini_fight.plf: $(O_FILES)
mkbe.rel_mini_fight.rel: ELF2REL_ARGS := -i 5 -o 0x85 -l 0x24 -c 17
ALL_RELS += mkbe.rel_mini_fight.rel

# mkbe.rel_mini_pilot.rel sources
SOURCES := \
	src/mini_pilot.c \
	asm/mini_pilot.s
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.rel_mini_pilot.plf: $(O_FILES)
mkbe.rel_mini_pilot.rel: ELF2REL_ARGS := -i 6 -o 0xA9 -l 0x24 -c 18
ALL_RELS += mkbe.rel_mini_pilot.rel

# mkbe.rel_mini_golf.rel sources
SOURCES := \
	src/mini_golf.c \
	asm/mini_golf.s
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.rel_mini_golf.plf: $(O_FILES)
mkbe.rel_mini_golf.rel: ELF2REL_ARGS := -i 7 -o 0xCD -l 0x23 -c 17
ALL_RELS += mkbe.rel_mini_golf.rel

# mkbe.rel_mini_billiards.rel sources
SOURCES := \
	src/mini_billiards.c \
	asm/mini_billiards.s
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.rel_mini_billiards.plf: $(O_FILES)
mkbe.rel_mini_billiards.rel: ELF2REL_ARGS := -i 8 -o 0xF0 -l 0x28 -c 17
ALL_RELS += mkbe.rel_mini_billiards.rel

# mkbe.rel_sample.rel sources
SOURCES := \
	src/rel_sample.c
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.rel_sample.plf: $(O_FILES)
mkbe.rel_sample.rel: ELF2REL_ARGS := -i 9 -o 0x118 -l 0x20
ALL_RELS += mkbe.rel_sample.rel

# mkbe.test_mode.rel sources
SOURCES := \
	src/test_mode.c \
	asm/test_mode.s
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.test_mode.plf: $(O_FILES)
mkbe.test_mode.rel: ELF2REL_ARGS := -i 10 -o 0x138 -l 0x1F -c 18
ALL_RELS += mkbe.test_mode.rel

# mkbe.option.rel sources
SOURCES := \
	src/option.c \
	asm/option.s
O_FILES := $(addsuffix .o,$(SOURCES))
ALL_O_FILES += $(O_FILES)
mkbe.option.plf: $(O_FILES)
mkbe.option.rel: ELF2REL_ARGS := -i 11 -o 0x157 -l 0x1C -c 17
ALL_RELS += mkbe.option.rel

#-------------------------------------------------------------------------------
# Recipes
#-------------------------------------------------------------------------------

.PHONY: all default

# remove implicit rules
.SUFFIXES:
MAKEFLAGS += -r

all: $(DOL) $(ALL_RELS)
	$(QUIET) $(SHA1SUM) -c supermonkeyball.sha1

# static module (.dol file)
%.dol: %.elf $(ELF2DOL)
	@echo Converting $< to $@
	$(QUIET) $(ELF2DOL) $(filter %.elf,$^) $@

%.elf: $(DOL_LCF)
	@echo Linking static module $@
	$(QUIET) $(LD) -lcf $(DOL_LCF) $(DOL_LDFLAGS) $(filter %.o,$^) -map $(@:.elf=.map) -o $@

# relocatable module (.rel file)
%.rel: %.plf $(ELF) $(ELF2REL)
	@echo Converting $(filter %.plf,$^) to $@
	$(QUIET) $(ELF2REL) $(filter %.plf,$^) $(ELF) $@ $(ELF2REL_ARGS)

%.plf: $(REL_LCF)
	@echo Linking relocatable module $@
	$(QUIET) $(LD) -lcf $(REL_LCF) $(REL_LDFLAGS) $(filter %.o,$^) -map $(@:.plf=.map) -o $@

# Canned recipe for compiling C or C++
# Uses CC_CHECK to check syntax and generate dependencies, compiles the file,
# then disassembles the object file
define COMPILE =
@echo "Compiling " $<
$(QUIET) $(CC_CHECK) -MMD -MF $(@:.o=.dep) -MT $@ $<
$(QUIET) $(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<
endef

# These currently cause problems when compiled with gcc
MWCC_ONLY_OBJECTS := \
	src/mathutil.c.o \
	libraries/os/__start.c.o \
	libraries/os/__ppc_eabi_init.c.o \
	libraries/os/OSSync.c.o \
	libraries/PowerPC_EABI_Support/Runtime/%.o \
	libraries/TRK_MINNOW_DOLPHIN/%.o
$(MWCC_ONLY_OBJECTS): CC       := $(MWCC)
$(MWCC_ONLY_OBJECTS): CFLAGS   := $(MWCC_CFLAGS)
$(MWCC_ONLY_OBJECTS): CPPFLAGS  = $(MWCC_CPPFLAGS)

# relocatable modules must not use the small data sections
%.plf: CFLAGS += $(REL_FLAGS)

%.c.o: %.c
	$(COMPILE)
%.cpp.o: %.cpp
	$(COMPILE)
%.cp.o: %.cp
	$(COMPILE)

# remove 8-byte alignment for this file
src/unk_anim_data.c.o: src/unk_anim_data.c
	$(COMPILE)
	$(OBJCOPY) -j .rodata --set-section-alignment=.rodata=1 $@ $@

%.s.o: %.s
	@echo Assembling $<
	$(QUIET) $(AS) $(ASFLAGS) -o $@ $<

# disassemble object file
%.dump: %.o
	$(OBJDUMP) -Drz $< > $@

clean:
	$(RM) $(DOL) $(ELF) $(MAP) $(ALL_RELS) $(ELF2DOL) $(ELF2REL)
	find . -name '*.o' -exec rm {} +
	find . -name '*.dep' -exec rm {} +
	find . -name '*.dump' -exec rm {} +

#-------------------------------------------------------------------------------
# Test Recipes
#-------------------------------------------------------------------------------

EMULATOR ?= dolphin-emu-nogui
DVD_ROOT := GMBE8P/files
DVD_SYS  := GMBE8P/sys
DVD_FILES := $(DVD_SYS)/main.dol $(addprefix $(DVD_ROOT)/test/,$(ALL_RELS))

$(DVD_ROOT)/test/%.rel: %.rel
	cp $< $@
$(DVD_SYS)/main.dol: $(DOL)
	cp $< $@

test: $(DVD_FILES)
	$(EMULATOR) $(DVD_SYS)/main.dol

# These need an extra include directory and are incompatible with gcc
RUNTIME_OBJECTS := \
	libraries/os/__start.c.o \
	libraries/os/__ppc_eabi_init.c.o \
	libraries/PowerPC_EABI_Support/Runtime/Src/__mem.c.o \
	libraries/PowerPC_EABI_Support/Runtime/Src/ExceptionPPC.cp.o \
	libraries/PowerPC_EABI_Support/Runtime/Src/__init_cpp_exceptions.cpp.o \
	libraries/PowerPC_EABI_Support/Runtime/Src/global_destructor_chain.c.o \
	libraries/PowerPC_EABI_Support/Runtime/Src/__va_arg.c.o \
	libraries/PowerPC_EABI_Support/Msl/MSL_C/MSL_Common/Src/mem_funcs.c.o

$(RUNTIME_OBJECTS): CC_CHECK := true
$(RUNTIME_OBJECTS): SYSTEM_INCLUDE_DIRS += libraries/PowerPC_EABI_Support/Runtime/Inc

libraries/TRK_MINNOW_DOLPHIN/Portable/mem_TRK.c.o: CC_CHECK := true
libraries/PowerPC_EABI_Support/%.o: MWCC_CFLAGS += -fp_contract on

# Automatic dependency files
DEP_FILES := $(addsuffix .dep,$(basename $(ALL_O_FILES)))
-include $(DEP_FILES)

#-------------------------------------------------------------------------------
# Tool Recipes
#-------------------------------------------------------------------------------

$(ELF2DOL): tools/elf2dol.c
	@echo Building tool $@
	$(QUIET) $(HOSTCC) $(HOSTCFLAGS) -o $@ $^

$(ELF2REL): tools/elf2rel.c
	@echo Building tool $@
	$(QUIET) $(HOSTCC) $(HOSTCFLAGS) -o $@ $^

# Original source for lzss can be found at
# https://web.archive.org/web/19990203141013/http://oak.oakland.edu/pub/simtelnet/msdos/arcutils/lz_comp2.zip
$(LZSS): tools/lzss.c
	@echo Building tool $@
	$(QUIET) $(HOSTCC) $(HOSTCFLAGS) -o $@ $^

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true
