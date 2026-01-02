#!/usr/bin/env python3
# Generates a linker script from a slice file.

from io import StringIO
from pathlib import Path
from typing import cast

from elffile import *
from project_settings import *
from slicelib import *

def make_elf_force_directives(slice_file: SliceFile) -> set[str]:
    unit_name = slice_file.unit_name()

    force_actives = set()
    for slice in slice_file.parsed_slices:
        if not slice.source or slice.nonMatching:
            continue

        # We load the compiled ELF file and go through the symbols to check if any of them
        # are in the deadstrip list. If not, we add them to the FORCEACTIVE directive.
        compiled_path = BUILDDIR_COMPILED / unit_name / slice.sliceName
        elf_file = ElfFile.read(compiled_path.read_bytes())
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

    return force_actives


def make_plf_force_directives(preplfs: list[Path]) -> set[str]:
    force_actives = set()

    # We need the unresolved symbols from all RELs so that the linker does not deadstrip them.
    # We do this by checking the symbols in the .preplf with an undefined section index.
    for preplf in preplfs:
        preplf_file: ElfFile = ElfFile.read(preplf.read_bytes())
        symtab = cast(ElfSymtab, preplf_file.get_section('.symtab'))
        for sym in symtab.syms:
            if sym.st_shndx == 0 and not REL_SYM.match(sym.name):
                force_actives.add(sym.name)

    force_actives.add('_prolog')
    force_actives.add('_epilog')
    force_actives.add('_unresolved')
    return force_actives


def gen_lcf(slice_file_path: Path, preplfs: list[Path], out_path: Path) -> None:
    assert slice_file_path.is_file()

    slice_file: SliceFile = load_slice_file(slice_file_path)
    lcf_file = StringIO()

    # MEMORY section: include base address (hardcoded for now)
    lcf_file.write('MEMORY {\n')
    lcf_file.write('\ttext : origin = 0x80004000\n')
    lcf_file.write('}\n\n')

    # SECTIONS section
    lcf_file.write('SECTIONS {\n')
    lcf_file.write('\tGROUP: {\n')
    for name, section in slice_file.meta.sections.items():
        if '$' in name or section.size == 0:
            continue
        align = section.align if section.secAlign == -1 else section.secAlign
        if slice_file.meta.type == SliceType.DOL:
            align = 0x20
        lcf_file.write(f'\t\t{name} ALIGN(0x{align:0x}) : {{}}\n')
    lcf_file.write('\t} > text\n')
    lcf_file.write('}\n\n')

    # FORCEACTIVE section
    if slice_file.meta.type == SliceType.DOL:
        force_active = make_elf_force_directives(slice_file)
    else:
        force_active = make_plf_force_directives(preplfs)

    lcf_file.write('FORCEACTIVE {\n')
    for force_sym in sorted(force_active):
        lcf_file.write(f'\t{force_sym}\n')
    lcf_file.write('}\n')

    out_path.write_text(lcf_file.getvalue())


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Generates a linker script from a slice file.')
    parser.add_argument('slice_file', type=Path, help='Slice file to generate the linker script from.')
    parser.add_argument('preplf', nargs='*', type=Path, help='PREPLF files to generate the FORCEACTIVE section from.')
    parser.add_argument('-o', '--output', type=Path, required=True, help='Path the linker script will be stored to.')
    args = parser.parse_args()
    gen_lcf(args.slice_file, args.preplf, args.output)
