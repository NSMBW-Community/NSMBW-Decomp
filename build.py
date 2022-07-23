from pathlib import Path
import subprocess

from tools.color_term import *
from tools.slices import SliceFile, SliceType, load_slice_file

# Step 1: compile sources

# TODO: compile, outfiles go to bin/compiled/

rel_names: list[str] = ['d_profileNP.plf', 'd_basesNP.plf', 'd_enemiesNP.plf', 'd_en_bossNP.plf']

count_compiled_used = 0
count_sliced_used = 0

for file in Path('slices').glob('*'):
    with open(file, 'r') as sf:
        # Step 2: slice main.dol and RELs

        slice_file: SliceFile = load_slice_file(sf)

        slice_name_stem = Path(slice_file.meta.name).stem

        prog_to_use: str
        if (slice_file.meta.type == SliceType.REL):
            prog_to_use = 'tools/slice_rel.py'
        else:
            prog_to_use = 'tools/slice_dol.py' # TODO
        subprocess.call(['python', prog_to_use, f'original/{slice_file.meta.name}', '-o', f'bin/sliced/{slice_name_stem}'])
        print_success(f'Sliced {slice_file.meta.name}.')

        # Step 3: link object files
        ldpath = 'compilers/Wii/1.1/mwldeppc.exe'

        ldflags_dol = '-proc gekko -fp hard'
        ldflags_rel = '-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -opt_partial'

        # TODO: build lcf
        lcf_file = 'rel.lcf' #slice_name_stem + '.lcf'
        out_file = slice_name_stem + '.plf' if slice_file.meta.type == SliceType.REL else '.elf'

        # Select files
        file_names: list[str] = []
        for slice in slice_file.slices:
            try_paths = [Path(f'bin/{x}/{slice_name_stem}/{slice.slice_name}') for x in ['compiled', 'sliced']]

            if try_paths[0].exists():
                file_names.append(try_paths[0])
                count_compiled_used += 1
            elif try_paths[1].exists():
                file_names.append(try_paths[1])
                count_sliced_used += 1

        ldflags = ldflags_rel if slice_file.meta.type == SliceType.REL else ldflags_dol
        subprocess.call([ldpath, *ldflags.split(' '), *file_names, '-lcf', lcf_file, '-o', f'bin/{out_file}'])


# Step 4: build main.dol (TODO)


# Step 5: build RELs

fake_path = 'd:\\home\\Project\\WIIMJ2D\\EU\\PRD\\RVL\\bin\\'

out_rel_names = [f'bin/{x}' for x in rel_names]
subprocess.call(['python', 'tools/build_rel.py', 'bin/wiimj2d.elf', *out_rel_names, '--alias_file', 'alias_db.txt', '-p', fake_path])

print_success('Successfully built binaries!')
print('Statistics:')
perc = count_compiled_used / (count_compiled_used + count_sliced_used) * 100
print(f'Compiled vs sliced files: {count_compiled_used}/{count_compiled_used + count_sliced_used} ({perc}%)')