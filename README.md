# Super Monkey Ball Decompilation

This repo contains a WIP disassembly/decompilation of the program code of Super Monkey Ball NTSC-U version (GMBE8P).

It builds the following file:

supermonkeyball.dol: `sha1: 424e8ce10135686de0709a147e6a3a5a3fda02f1`

## Build Instructions

### Required Tools

* [devkitPro](https://devkitpro.org/wiki/Getting_Started)
* CodeWarrior for GameCube compiler (version 1.0, 1.1, or 1.2.5)
* gcc
* make
* wine (only on non-Windows systems)

### Steps

* Create a directory called `mwcc_compiler/<VERSION>` where `<VERSION>` is the version of CodeWarrior for GameCube you have (1.0, 1.1, or 1.2.5).
* Place the following CodeWarrior compiler executables in the aforementioned directory:
  - mwcceppc.exe
  - mwldeppc.exe
  - lmgr326b.dll
* Run `make` from the repository root directory. If you are using a version of CodeWarrior besides 1.1, you must run `make COMPILER_VERSION=<VERSION>` (where `<VERSION>` is your CodeWarrior version).

## Build Instructions (for PC port)

* Install gcc and OpenGL libraries. On Windows, this requires MSYS2.
* Run `make -f Makefile_pc` and wait for it to finish compiling
* Create a folder called GMBE8P and extract the files (entire disc) of your Super Monkey Ball game disc into that directory using Dolphin. Your directory structure should be something like this:
```
smb-decomp/GMBE8P +-- files/ +-- test/ +-- ape/
                  |          |         +-- bg/
                  |          |         +-- bmp/
                  |          +-- opening.bnr
                  +-- sys/
```
