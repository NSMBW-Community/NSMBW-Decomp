from pathlib import Path
import subprocess

from tools.color_term import *
from tools.slicelib import SliceFile, SliceType, load_slice_file

def print_cmd(cmd):
    print(' '.join([str(x) for x in cmd]))

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
        unit_name = slice_file.meta.unit_name
        if slice.slice_src:
            ccflags = slice_file.meta.default_compiler_flags
            if slice.cc_flags:
                ccflags = slice.cc_flags

            Path(f'bin/compiled/{unit_name}/{slice.slice_src}').parents[0].mkdir(parents=True, exist_ok=True)

            cmd = [ccpath, '-c', *ccflags, f'source/{slice.slice_src}']
            cmd.extend(['-o', f'bin/compiled/{unit_name}/{slice.slice_name}'])
            cmd.extend(['-I-', '-i', 'include'])
            print_cmd(cmd)
            subprocess.call(cmd)

count_compiled_used = 0
count_sliced_used = 0

for slice_file in slices:
    # Step 2: slice main.dol and RELs

    slice_name_stem = Path(slice_file.meta.name).stem

    slice_is_rel = slice_file.meta.type == SliceType.REL

    prog_to_use: str = 'tools/slice_rel.py' if slice_is_rel else 'tools/slice_dol.py'

    cmd = ['python', prog_to_use, f'original/{slice_file.meta.name}', '-o', f'bin/sliced/{slice_file.meta.unit_name}']
    print_cmd(cmd)
    subprocess.call(cmd)
    print_success(f'Sliced {slice_file.meta.name}.')

    # Step 3: link object files

    ldflags_dol = '-proc gekko -fp hard'
    ldflags_rel = '-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -opt_partial'

    # TODO: build lcf
    lcf_file: str = 'rel.lcf' if slice_is_rel else 'dol.lcf'
    out_file = slice_name_stem + ('.plf' if slice_is_rel else '.elf')

    # Select files
    file_names: list[str] = []
    for slice in slice_file.slices:
        try_paths = [Path(f'bin/{x}/{slice_file.meta.unit_name}/{slice.slice_name}') for x in ['compiled', 'sliced']]

        if try_paths[0].exists():
            file_names.append(try_paths[0])
            count_compiled_used += 1
        elif try_paths[1].exists():
            file_names.append(try_paths[1])
            count_sliced_used += 1

    ldflags = ldflags_rel if slice_file.meta.type == SliceType.REL else ldflags_dol
    cmd = [ldpath, *ldflags.split(' '), *file_names, '-lcf', lcf_file, '-o', f'bin/{out_file}']
    print_cmd(cmd)
    subprocess.call(cmd)


# Step 4: build main.dol
subprocess.call(['python', 'tools/build_dol.py', 'bin/wiimj2d.elf'])

# Step 5: build RELs

fake_path = 'd:\\home\\Project\\WIIMJ2D\\EU\\PRD\\RVL\\bin\\'

out_rel_names = [f'bin/{x.meta.name.replace(".rel", ".plf").replace(".dol", ".elf")}' for x in slices]
cmd = ['python', 'tools/build_rel.py', *out_rel_names, '--alias_file', 'alias_db.txt', '-p', fake_path]
print_cmd(cmd)
subprocess.call(cmd)

print_success('Successfully built binaries!')
print('Statistics:')
perc = count_compiled_used / (count_compiled_used + count_sliced_used) * 100
print(f'Compiled vs sliced files: {count_compiled_used}/{count_compiled_used + count_sliced_used} ({perc}%)')