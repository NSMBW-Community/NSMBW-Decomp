#!/usr/bin/env python3
# Slices a DOL file into .o files

from pathlib import Path

from color_term import *
from dolfile import Dol
from elffile import *
from elfconsts import *
from slicelib import *

def extract_slice(dol_file: Dol, slice: Slice, syms: dict[int, str]) -> ElfFile:
    elf_file = ElfFile(ET.ET_REL, EM.EM_PPC)

    symtab_sec = ElfSymtab('.symtab')
    shstrtab_sec = ElfStrtab('.shstrtab')
    strtab_sec = ElfStrtab('.strtab')

    for sec in slice.slice_secs:
        if sec.sec_name in ['.bss', '.sbss', '.sbss2']:
            elf_sec = ElfSection(sec.sec_name)
            elf_sec.header.sh_type = SHT.SHT_NOBITS
            elf_sec.header.sh_size = sec.end_offs - sec.start_offs
        else:
            sec_data = dol_file.sections[sec.sec_idx].data[sec.start_offs:sec.end_offs]
            elf_sec = ElfSection(sec.sec_name, bytes(sec_data))
            elf_sec.header.sh_type = SHT.SHT_PROGBITS

        elf_sec.header.sh_addralign = sec.alignment
        sec_idx = elf_file.add_section(elf_sec)

        # Does this section contain __start?
        offs = dol_file.entry - dol_file.sections[0].virt_addr
        if sec.contains(0, offs):
            symtab_sec.add_symbol(ElfSymbol('__start', st_value=offs-sec.start_offs, st_size=4, st_info_bind=STB.STB_GLOBAL, st_info_type=STT.STT_FUNC, st_shndx=sec_idx))

        # TODO: loading from a proper map should speed this up, as we also have the section indices there
        for sym in syms:
            for idx, dol_sec in enumerate(dol_file.sections):
                offs = syms[sym] - dol_sec.virt_addr
                if sec.contains(idx, offs):
                    print('Found', sym, f'at 0x{offs:0x}')
                    elfsym = ElfSymbol(sym, st_value=offs-sec.start_offs, st_info_bind=STB.STB_GLOBAL, st_shndx=sec_idx)
                    symtab_sec.add_symbol(elfsym)

    elf_file.add_section(symtab_sec)
    elf_file.add_section(shstrtab_sec)
    strtab_sec_idx = elf_file.add_section(strtab_sec)

    symtab_sec.header.sh_link = strtab_sec_idx
    return elf_file


def slice_dol(dol_file: Path, out_path: Path) -> None:
    if not dol_file.is_file():
        print_err('Fatal error: File', str(dol_file), 'not found!')
        return

    # TODO: use an actual symbol map file
    syms: dict[int, str] = {}
    with open('syms.txt') as sym_file:
        for line in sym_file:
            if line != '\n':
                sym, addr = line.split('=')
                if sym in syms:
                    print_warn('Warning: symbol', sym, 'defined multiple times in syms.txt!')
                syms[sym] = int(addr, 16)

    # Read slices
    with open(dol_file, 'rb') as f:
        with open(f'slices/{dol_file.with_suffix(".json").name}') as sf:
            slice_file: SliceFile = load_slice_file(sf)
            print(f'Slicing module 0 ({f.name})...')
            dol_file = Dol(file=f)
            #print(dol_file.sections)

            for slice in slice_file.slices:
                elf = extract_slice(dol_file, slice, syms)
                out_filepath = out_path.joinpath(slice.slice_name)
                out_filepath.parents[0].mkdir(parents=True, exist_ok=True)
                with open(out_filepath, 'wb') as ef:
                    ef.write(bytes(elf))


if __name__ == '__main__':

    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Slices DOL files.')
    parser.add_argument('dol_file', type=Path, help='DOL file to be sliced.')
    parser.add_argument('--out_path', '-o', default=Path('bin/sliced'), type=Path, help='Path the slices will be stored to.')
    args = parser.parse_args()
    slice_dol(args.dol_file, args.out_path)
