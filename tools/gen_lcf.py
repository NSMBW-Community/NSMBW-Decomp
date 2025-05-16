#!/usr/bin/env python3
# Generates a linker script from a slice file.

from pathlib import Path
from typing import cast

from color_term import print_err
from elffile import *
from elfconsts import *
from project_settings import *
from slicelibV2 import *

def make_force_directives(slice_file: SliceFile) -> list[str]:
    unit_name = Path(slice_file.meta.fileName).stem

    force_actives = set()
    for slice in slice_file.parsed_slices:
        if not slice.source or slice.nonMatching:
            continue

        # We load the compiled ELF file and go through the symbols to check if any of them
        # are in the deadstrip list. If not, we add them to the FORCEACTIVE directive.
        compiled_path = BUILDDIR / 'compiled' / unit_name / slice.sliceName
        elf_file = ElfFile.read(open(compiled_path, 'rb').read())
        symtab = cast(ElfSymtab, elf_file.get_section('.symtab'))

        # We need to add all symbols except for the ones we want to deadstrip
        # to the FORCEACTIVE directive.
        for sym in symtab.syms:
            if sym.name in slice_file.deadstrip:
                continue
            if sym.st_info_type in [STT.STT_FUNC, STT.STT_OBJECT] and sym.st_info_bind == STB.STB_GLOBAL:
                force_actives.add(sym.name)
            elif sym.name in slice_file.keepWeak:
                # Some symbols might not be referenced yet, but we still want to keep them
                force_actives.add(sym.name)

    return sorted(force_actives)


def gen_lcf(slice_file_path: Path, out_path: Path) -> None:
    if not slice_file_path.is_file():
        print_err('Fatal error: File', str(slice_file_path), 'not found!')
        return

    slice_file: SliceFile = load_slice_file(slice_file_path)

    lcf_file = ''

    # MEMORY section: include base address (hardcoded for now)
    lcf_file += 'MEMORY {\n'
    lcf_file += '\ttext : origin = 0x80004000\n'
    lcf_file += '}\n'

    # SECTIONS section
    lcf_file += 'SECTIONS {\n'
    lcf_file += '\tGROUP: {\n'
    for sec in slice_file.meta.sections:
        s = slice_file.meta.sections[sec]
        if '$' in sec or s.size == 0:
            continue
        align = s.align if s.secAlign == -1 else s.secAlign
        lcf_file += f'\t\t{sec} ALIGN(0x{align:0x}) : {{}}\n'
    lcf_file += '\t} > text\n'
    lcf_file += '}\n'

    # FORCEACTIVE section
    if slice_file.meta.type == SliceType.DOL:
        force_active = make_force_directives(slice_file)
        lcf_file += 'FORCEACTIVE {\n'
        for force_sym in force_active:
            lcf_file += f'\t{force_sym}\n'
        lcf_file += '}\n'

    out_path.write_text(lcf_file)

if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Generates a linker script from a slice file.')
    parser.add_argument('slice_file', type=Path, help='Slice file to generate the linker script from.')
    parser.add_argument('-o', '--output', type=Path, help='Path the linker script will be stored to.')
    args = parser.parse_args()
    gen_lcf(args.slice_file, args.output)
