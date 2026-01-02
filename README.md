# NSMBW-Decomp

![Progress badge DOL](https://img.shields.io/endpoint?url=https%3A%2F%2Fnsmbw-community.github.io%2FNSMBW-Decomp%2Fbadge_wiimj2d.json)
![Progress badge d_profileNP](https://img.shields.io/endpoint?url=https%3A%2F%2Fnsmbw-community.github.io%2FNSMBW-Decomp%2Fbadge_d_profileNP.json)
![Progress badge d_basesNP](https://img.shields.io/endpoint?url=https%3A%2F%2Fnsmbw-community.github.io%2FNSMBW-Decomp%2Fbadge_d_basesNP.json)
![Progress badge d_en_bossNP](https://img.shields.io/endpoint?url=https%3A%2F%2Fnsmbw-community.github.io%2FNSMBW-Decomp%2Fbadge_d_en_bossNP.json)

A decompilation project of New Super Mario Bros. Wii, based on the PALv1 binaries.

## Quick Links
- [Setup](#setup)
- [Building](#building)
- [Documentation](#documentation)
- [Contributing](#contributing)

## Overview
This project aims to produce an accurate reconstruction of New Super Mario Bros. Wii's code by decompiling the original binaries. The build process consists of three major stages:

### 1. Compilation
All decompiled source files are compiled into object files in `bin/compiled/`.

### 2. Slicing
Parts of the binary without corresponding source files are extracted from the original binaries using slicing. This produces object files in `bin/sliced/`, using slice information defined in the `slices/` directory.

### 3. Linking and Rebuilding
Compiled and sliced object files are linked together to reconstruct the final `.dol` and `.rel` files that match the originals byte-for-byte.

### Other Relevant Files
- `alias_db.txt`: Contains relocation helpers for rebuilding the `.rel` files.
- `syms.txt`: Contains known symbol names for `main.dol`. Will eventually be replaced by a proper symbol map.

## Setup
1. Obtain `main.dol` and the four `.rel.LZ` files from a dumped PALv1 disc of New Super Mario Bros. Wii. A tutorial to do so can be found [here](https://horizon.miraheze.org/wiki/Obtain_Original_Game_Files).
2. Use `tools/uncompress_lz.py` to decompress the `.rel.LZ` files into `.rel`.
3. Rename `main.dol` to `wiimj2d.dol`, and place it along with the decompressed `.rel` files into the `original/` directory.
4. Verify the following checksums:

```bash
$ md5sum original/*
17096d0ed441d44a0c31039138a8d7f8  original/d_basesNP.rel
f8cffd634edbec6c1bc210dab02c1e32  original/d_en_bossNP.rel
458f1aa94706dde1a8f2f9d97ff1f35c  original/d_enemiesNP.rel
393bf12ef4254dd1ea1366d5b06fbebb  original/d_profileNP.rel
ddab9e5dca53d8c18bf4051b927e822e  original/wiimj2d.dol
```

5. Download the CodeWarrior for Embedded PowerPC binaries from [here](https://files.decomp.dev/compilers_20230715.zip) and extract it into the `compilers/` directory. The compiler should be located at `compilers/Wii/1.1/mwcceppc.exe`.
   - **Windows users**: No additional setup is required.
   - **Linux/macOS users**: Install [wibo](https://github.com/decompals/WiBo) or use [WINE](https://www.winehq.org/). `wibo` is recommended for faster compilation and is used automatically if detected on `PATH`.

6. Install `ninja` (if not already installed):

```bash
pip install ninja
```

## Building
1. Generate the build script:

```bash
./configure.py
```

2. Run the build using `ninja`:

```bash
ninja
```

3. (Optional) Verify that the rebuilt binaries match the originals:

```bash
./progress.py --verify-bin
```

## Building a modded game version
If you wish to modify any source files and recompile the project into a working game, do the following:

1. Create a matching build of the game as described in the previous section:
```bash
./configure.py
ninja
./progress.py --verify-bin
```

2. Run `prepare_objdiff.py`
```bash
python prepare_objdiff.py
```

*Important: Do not run this command again after modifying game files! Revert back to the original state if you need to.*

3. Modify the game files as much as you want

4. Compile the sources:
```bash
ninja
```

5. Create the modded game executables:
```bash
python build_modded.py
```

Optionally, you can specify the location of an extracted game disk. This way, you don't have to copy over the files each time.
```bash
python build_modded.py --game_folder <...>
```

Repeat steps 4 and 5 after making new changes.

## Documentation
You can generate source documentation using [Doxygen](https://www.doxygen.nl/).

1. Install Doxygen.
2. If not already present, download the [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css) theme and place its contents into the following directory:

```
website/doxygen/doxygen-awesome-css/
```

3. Generate the documentation:

```bash
doxygen
```

The generated HTML documentation will be placed in the `doxygen` directory.

## Contributing
We welcome contributions to this project! Please see [CONTRIBUTING.md](docs/CONTRIBUTING.md) for detailed guidelines.

### Decompilation
Usage of [objdiff](https://github.com/encounter/objdiff) is recommended for decompilation work.

To prepare the repository for use with `objdiff`, run:

```bash
./prepare_objdiff.py
```

This sets up the necessary files and configuration.
