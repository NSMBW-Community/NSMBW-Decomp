#!/usr/bin/env python3
# Build script

import argparse
import subprocess
import sys
from pathlib import Path

sys.path.append('tools')

from build_dol import build_dol
from build_rel import build_rel
from color_term import *
from slice_dol import slice_dol
from slice_rel import slice_rel
from slicelib import SliceFile, SliceType, load_slice_file

def print_cmd(*nargs, **kwargs):
    global args
    if args.v:
        print(*nargs, **kwargs)


parser = argparse.ArgumentParser(description='Builds the game executables.')
parser.add_argument('-v', help='Prints commands for debugging', action='store_true')
args = parser.parse_args()

slices: list[SliceFile] = []

ldpath = 'compilers/Wii/1.1/mwldeppc.exe'
ccpath = 'compilers/Wii/1.1/mwcceppc.exe'

for file in Path('slices').glob('*'):
    with open(file, 'r') as sf:
        slices.append(load_slice_file(sf))

# Ensure correct order of slices
slices = sorted(slices, key=lambda x: x.meta.mod_num)

# Step 1: compile sources
for slice_file in slices:
    for slice in slice_file.slices:
        unit_name = Path(slice_file.meta.name).stem
        if slice.slice_src:
            ccflags = slice_file.meta.default_compiler_flags
            if slice.cc_flags:
                ccflags = slice.cc_flags

            Path(f'bin/compiled/{unit_name}/{slice.slice_src}').parents[0].mkdir(parents=True, exist_ok=True)

            cmd = [] if sys.platform == 'win32' else ['wine']
            cmd.extend([ccpath, '-c', *ccflags, f'source/{slice.slice_src}'])
            cmd.extend(['-o', f'bin/compiled/{unit_name}/{slice.slice_name}'])
            cmd.extend(['-I-', '-i', 'include'])
            print_cmd(*cmd)
            subprocess.call(cmd)

count_compiled_used = 0
count_sliced_used = 0

for slice_file in slices:

    # Step 2: slice main.dol and RELs
    slice_name_stem = Path(slice_file.meta.name).stem
    slice_is_rel = slice_file.meta.type == SliceType.REL

    if slice_is_rel:
        slice_rel(Path(f'original/{slice_file.meta.name}'), Path(f'bin/sliced/{slice_name_stem}'))
    else:
        slice_dol(Path(f'original/{slice_file.meta.name}'), Path(f'bin/sliced/{slice_name_stem}'))
    print_success('Sliced', slice_file.meta.name, end='.\n')

    # Step 3: link object files
    ldflags_dol = '-proc gekko -fp hard'
    ldflags_rel = '-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -opt_partial'
    out_file = slice_name_stem + ('.plf' if slice_is_rel else '.elf')

    # Select files
    file_names: list[str] = []
    lcf_force_files: list[str] = []
    for slice in slice_file.slices:
        try_paths = [Path(f'bin/{x}/{slice_name_stem}/{slice.slice_name}') for x in ['compiled', 'sliced']]

        use_file: Path = None
        if try_paths[0].exists():
            use_file = try_paths[0]
            count_compiled_used += 1
        elif try_paths[1].exists():
            use_file = try_paths[1]
            count_sliced_used += 1
        
        if use_file:
            file_names.append(use_file)
            if not slice.force_active:
                lcf_force_files.append(use_file)
    
    base_lcf_file: str = 'template_rel.lcf' if slice_is_rel else 'template_dol.lcf'
    out_lcf_file = f'bin/{slice_name_stem}.lcf'
    with open(base_lcf_file, 'r') as f:
        base_lcf_contents = f.read()
    with open(out_lcf_file, 'w') as f:
        f.write('FORCEFILES {\n\t')
        f.write('\n\t'.join(['\\'.join(path.parts) for path in lcf_force_files])) # The linker requires backslashes
        f.write('\n}\n')
        force_actives = []
        for slice in [x.force_active for x in slice_file.slices if x.force_active]:
            force_actives.extend(slice)
        f.write('FORCEACTIVE {\n\t')
        f.write('\n\t'.join(force_actives))
        f.write('\n}\n\n')
        f.write(base_lcf_contents)

    ldflags = ldflags_rel if slice_file.meta.type == SliceType.REL else ldflags_dol
    cmd = [] if sys.platform == 'win32' else ['wine']
    cmd.extend([ldpath, *ldflags.split(' '), *file_names, '-lcf', out_lcf_file, '-o', f'bin/{out_file}'])
    print_cmd(*cmd)
    subprocess.call(cmd)

# Step 4: build main.dol
build_dol(Path('bin/wiimj2d.elf'))

# Step 5: build RELs
fake_path = 'd:\\home\\Project\\WIIMJ2D\\EU\\PRD\\RVL\\bin\\'
out_rel_names = [Path(f'bin/{x.meta.name.replace(".rel", ".plf").replace(".dol", ".elf")}') for x in slices]
build_rel(out_rel_names[0], out_rel_names[1:], Path('alias_db.txt'), fake_path)

# Done!
print_success('Successfully built binaries!')
total_used = count_compiled_used + count_sliced_used
perc = count_compiled_used / total_used
print(total_used, 'object files used, of which', count_compiled_used, 'were compiled files.', f'({round(perc*100, 1)}% compiled files)')
