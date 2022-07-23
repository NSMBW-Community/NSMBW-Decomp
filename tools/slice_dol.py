#!/usr/bin/env python3
# Slices a DOL file into .o files

import argparse
from pathlib import Path
from dolfile import Dol

from elffile import *
from elfconsts import *
from slicelib import *

def extract_slice(dol_file: Dol, slice: Slice):
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
            symtab_sec.add_symbol(ElfSymbol('__start', st_value=offs, st_size=4, st_info_bind=STB.STB_GLOBAL, st_info_type=STT.STT_FUNC, st_shndx=sec_idx))
        
        #if sec.contains(4, 0x2D6308):
            #symtab_sec.add_symbol(ElfSymbol('__destroy_global_chain', st_value=0x2D6308, st_size=4, st_info_bind=STB.STB_GLOBAL, st_info_type=STT.STT_FUNC, st_shndx=4))

    _symtab_sec_idx = elf_file.add_section(symtab_sec)
    _shstrtab_sec_idx = elf_file.add_section(shstrtab_sec)
    strtab_sec_idx = elf_file.add_section(strtab_sec)

    symtab_sec.header.sh_link = strtab_sec_idx

    return elf_file

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Slices DOL files')
    parser.add_argument('dol_file', type=Path)
    parser.add_argument('--out_path', '-o', default=Path('bin/sliced'), type=Path)
    args = parser.parse_args()

    dol: Path = args.dol_file
    if not dol.is_file():
        print(f'Invalid file {dol}')
    else:
        with open(dol, 'rb') as f:
            # Read slices
            with open(f'slices/{dol.with_suffix(".json").name}', 'r') as sf:
                slice_file: SliceFile = load_slice_file(sf)
                print(f'Slicing module 0 ({f.name})...')
                dol_file = Dol(file=f)
                #print(dol_file.sections)
                
                for slice in slice_file.slices:
                    elf = extract_slice(dol_file, slice)
                    args.out_path.mkdir(parents=True, exist_ok=True)
                    with open(args.out_path.joinpath(slice.slice_name), 'wb') as ef:
                        ef.write(bytes(elf))