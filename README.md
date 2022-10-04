# NSMBW-Decomp

![Progress badge DOL](https://img.shields.io/endpoint?url=https%3A%2F%2Fclf78.github.io%2FNSMBW-Decomp%2Fbadge_wiimj2d.json)
![Progress badge d_profileNP](https://img.shields.io/endpoint?url=https%3A%2F%2Fclf78.github.io%2FNSMBW-Decomp%2Fbadge_d_profileNP.json)
![Progress badge d_basesNP](https://img.shields.io/endpoint?url=https%3A%2F%2Fclf78.github.io%2FNSMBW-Decomp%2Fbadge_d_basesNP.json)
![Progress badge d_en_bossNP](https://img.shields.io/endpoint?url=https%3A%2F%2Fclf78.github.io%2FNSMBW-Decomp%2Fbadge_d_en_bossNP.json)

A decompilation project of New Super Mario Bros. Wii, based on the PALv1 binaries.

#### Quick links

- [Setup](#setup)
- [Contributing][Contributing]

## Overview

The build essentially consists of 3 steps:

### 1. Compilation

All source files get compiled and the corresponding object files placed in `bin/compiled/`.

### 2. Slicing

Since most of the binary does not have a corresponding decompiled source file, in order to produce a 1:1 match for the generated binaries, these parts must be extracted from the original files. The slices generates object files in `bin/sliced/` from the slice information files in `slices/`.

### 3. Linking and rebuilding

All compiled object files and the remaining sliced object files are linked together to form the final object file, which is then converted into `.dol` or `.rel`.

## Other relevant files

`alias_db.txt`: Contains relocation helpers for rebuilding the `.rel` files.

`syms.txt`: Contains known symbol names for `main.dol`. Will eventually be replaced by a proper symbol map.

## Setup

1. Obtain `main.dol` and the four `.rel.LZ` files from a dumped PALv1 disc of NSMBW.
2. Use `tools/uncompress_lz.py` to decompress the `.rel.LZ` files to `.rel`.
3. Rename `main.dol` to `wiimj2d.dol`, then place it and the four `.rel` files in `original/`.
4. Ensure that the checksums match:

```text
$ md5sum original/*
17096d0ed441d44a0c31039138a8d7f8  original/d_basesNP.rel
f8cffd634edbec6c1bc210dab02c1e32  original/d_en_bossNP.rel
458f1aa94706dde1a8f2f9d97ff1f35c  original/d_enemiesNP.rel
393bf12ef4254dd1ea1366d5b06fbebb  original/d_profileNP.rel
ddab9e5dca53d8c18bf4051b927e822e  original/wiimj2d.dol
```

5. Obtain the CodeWarrior for Embedded PowerPC binaries from [here](https://cdn.discordapp.com/attachments/727918646525165659/917185027656286218/GC_WII_COMPILERS.zip) and place them in `compilers/`.
6. Run `./build.py` to compile the sources and generate the output binaries.

[Contributing]: docs/CONTRIBUTING.md
