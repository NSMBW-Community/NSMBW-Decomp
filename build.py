from pathlib import Path
import subprocess

from tools.color_term import *
from tools.slices import SliceFile, SliceType, load_slice_file

# Step 1: compile sources

# TODO

rel_names: list[str] = []

for file in Path('slices').glob('*'):
    with open(file, 'r') as sf:
        # Step 2: slice main.dol and RELs

        slice_file: SliceFile = load_slice_file(sf)
        prog_to_use: str
        if (slice_file.meta.type == SliceType.REL):
            prog_to_use = 'tools/slice_rel.py'
        else:
            prog_to_use = 'tools/slice_dol.py' # TODO
        subprocess.call(['python', prog_to_use, f'original/{slice_file.meta.name}'])
        print_success(f'sliced {slice_file.meta.name}')

        # Step 3: link object files
        ldpath = 'compilers/Wii/1.1/mwldeppc.exe'

        ldflags_dol = '-proc gekko -fp hard'
        ldflags_rel = '-proc gekko -fp hard -sdata 0 -sdata2 0 -m _prolog -opt_partial'

        # TODO: build lcf
        lcf_file = slice_file.meta.name.replace('.dol', '.lcf').replace('.rel', '.lcf')
        out_file = slice_file.meta.name.replace('.dol', '.elf').replace('.rel', '.plf')

        file_names = [f'build/{x.slice_name}' for x in slice_file.slices]

        ldflags = ldflags_rel if slice_file.meta.type == SliceType.REL else ldflags_dol
        subprocess.call([ldpath, *ldflags.split(' '), *file_names, '-lcf', lcf_file, '-o', out_file])

        rel_names.append(out_file)

# Step 4: build RELs
    
subprocess.call(['python', 'tools/build_rel.py', 'wiimj2d.elf', *rel_names])

        

