from pathlib import Path
import subprocess

from tools.color_term import *
from tools.slices import SliceFile, SliceType, load_slice_file

# Step 1: compile sources

# TODO: compile, outfiles go to bin/compiled/

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

        # Select files
        file_names: list[str] = []
        for slice in slice_file.slices:
            if Path(f'bin/compiled/{slice.slice_name}').exists():
                file_names.append(f'bin/compiled/{slice.slice_name}')
            elif Path(f'bin/sliced/{slice.slice_name}').exists():
                file_names.append(f'bin/sliced/{slice.slice_name}')

        ldflags = ldflags_rel if slice_file.meta.type == SliceType.REL else ldflags_dol
        subprocess.call([ldpath, *ldflags.split(' '), *file_names, '-lcf', lcf_file, '-o', f'bin/{out_file}'])

        rel_names.append(out_file)

# Step 4: build main.dol (TODO)


# Step 5: build RELs

out_rel_names = [f'bin/{x}' for x in rel_names]
subprocess.call(['python', 'tools/build_rel.py', 'bin/wiimj2d.elf', *out_rel_names, '--alias_file', 'alias_db.txt'])
