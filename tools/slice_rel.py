#!/usr/bin/env python3
# Slices a REL file into .o files

import argparse
from pathlib import Path
from relfile import REL

from elffile import *
from elfconsts import *
from slices import *

class RelocSym:
    # Models a symbol referenced by a relocation entry
    def __init__(self, mod_num, section, addend):
        self.mod_num = mod_num
        self.section = section
        self.addend = addend
    
    def __repr__(self):
        return f'R_{self.mod_num:0x}_{self.section:0x}_{self.addend:0x}'

    def __str__(self):
        return self.__repr__()
        

reloc_syms: set[RelocSym] = set()
module_relocs: dict[int, dict[RelocSym, tuple[RelocSym, PPC_RELOC_TYPE]]] = dict()

def read_reloc_refs(rel_file: REL, idx: int):
    assert idx not in module_relocs
    module_relocs[idx] = dict()
    for mod_num in rel_file.relocations:
        curr_pos = 0
        curr_section = 0
        for reloc in rel_file.relocations[mod_num]:
            curr_pos += reloc.offset
            if reloc.reloc_type == PPC_RELOC_TYPE.R_RVL_SECT:
                curr_pos = 0
                curr_section = reloc.section
                continue
            if reloc.reloc_type.value < 201:
                this_loc = RelocSym(idx, curr_section, curr_pos)
                dest_sym = RelocSym(mod_num, reloc.section, reloc.addend)
                reloc_syms.add(dest_sym)
                module_relocs[idx][this_loc] = (dest_sym, reloc.reloc_type)

def extract_slice(rel_file: REL, slice: Slice):
    elf_file = ElfFile(ET.ET_REL, EM.EM_PPC)

    reloc_secs: list[ElfRelaSec] = []

    symtab_sec = ElfSymtab('.symtab')
    shstrtab_sec = ElfStrtab('.shstrtab')
    strtab_sec = ElfStrtab('.strtab')

    for sec in slice.slice_secs:
        if sec.sec_name == '.bss':
            elf_sec = ElfSection(sec.sec_name)
            elf_sec.header.sh_type = SHT.SHT_NOBITS
            elf_sec.header.sh_size = sec.end_offs - sec.start_offs
        else:
            sec_data = rel_file.sections[sec.sec_idx].get_data()[sec.start_offs:sec.end_offs]
            elf_sec = ElfSection(sec.sec_name, bytes(sec_data))
            elf_sec.header.sh_type = SHT.SHT_PROGBITS

        elf_sec.header.sh_addralign = sec.alignment
        elf_file.add_section(elf_sec)

    for sec in slice.slice_secs:
        relocs_in_section = [x for x in module_relocs[rel_file.index] if sec.contains(x.section, x.addend)]
        print(f'{slice.slice_name}: Section {sec.sec_name} contains {len(relocs_in_section)} relocs')
        if len(relocs_in_section) > 0:
            reloc_sec = ElfRelaSec(f'.rela{sec.sec_name}')
            reloc_secs.append(reloc_sec)
            for reloc_origin in relocs_in_section:
                reloc_dest = module_relocs[rel_file.index][reloc_origin]
                sym = ElfSymbol(str(reloc_dest[0]), st_info_bind=STB.STB_GLOBAL)
                sym_idx = symtab_sec.add_symbol(sym)
                r_offset = reloc_origin.addend - sec.start_offs
                elf_reloc = ElfRela(r_offset, sym_idx, reloc_dest[1], 0)
                reloc_sec.add_reloc(elf_reloc)

            reloc_sec.header.sh_info = elf_file.get_section_index(sec.sec_name)
            elf_file.add_section(reloc_sec)
    
    symtab_sec_idx = elf_file.add_section(symtab_sec)
    _shstrtab_sec_idx = elf_file.add_section(shstrtab_sec)
    strtab_sec_idx = elf_file.add_section(strtab_sec)

    symtab_sec.header.sh_link = strtab_sec_idx

    for rs in reloc_secs:
        rs.header.sh_link = symtab_sec_idx

    return elf_file

idx = 1

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Slices REL files')
    parser.add_argument('rel_files', type=Path, nargs='+')
    parser.add_argument('--out_path', '-o', default=Path('bin/sliced'), type=Path)
    args = parser.parse_args()

    rel: Path
    for rel in args.rel_files:
        if not rel.is_file():
            print(f'Invalid file {rel}')
            continue
        with open(rel, 'rb') as f:
            print(f'Processing module {idx} ({f.name})...')
            rel_file = REL(idx, file=f)
            read_reloc_refs(rel_file, idx)

            # Read slices
            with open(f'slices/{rel.with_suffix(".json").name}', 'r') as slice_file:
                slice_file = load_slice_file(slice_file)
                
                for slice in slice_file.slices:
                    elf = extract_slice(rel_file, slice)
                    args.out_path.mkdir(parents=True, exist_ok=True)
                    with open(args.out_path.joinpath(slice.slice_name), 'wb') as ef:
                        ef.write(bytes(elf))
                    idx += 1

    sorted_uniques = sorted(reloc_syms, key=lambda tup: (tup.mod_num, tup.section, tup.addend))
    with open('reloc_names.csv', 'w') as rf:
        rf.write('\n'.join([str(x) for x in sorted_uniques]))