#!/usr/bin/env python3
# Slices a REL file into .o files

from pathlib import Path

from color_term import print_err
from elffile import *
from elfconsts import *
from project_settings import *
from relfile import Rel
from slicelib import *

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


def read_reloc_refs(rel_file: Rel, idx: int, reloc_syms: set[RelocSym], module_relocs: dict[int, dict[RelocSym, tuple[RelocSym, PPC_RELOC_TYPE]]]) -> None:
    assert idx not in module_relocs
    module_relocs[idx] = {}

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


def extract_slice(rel_file: Rel, slice: Slice, module_relocs: dict[int, dict[RelocSym, tuple[RelocSym, PPC_RELOC_TYPE]]]) -> ElfFile:
    elf_file = ElfFile(ET.ET_REL, EM.EM_PPC)

    reloc_secs: list[ElfRelaSec] = []

    symtab_sec = ElfSymtab('.symtab')

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
        if len(relocs_in_section) > 0:
            reloc_sec = ElfRelaSec(f'.rela{sec.sec_name}', symtab=symtab_sec)
            reloc_secs.append(reloc_sec)
            for reloc_origin in relocs_in_section:
                reloc_dest = module_relocs[rel_file.index][reloc_origin]
                sym = ElfSymbol(str(reloc_dest[0]), st_info_bind=STB.STB_GLOBAL)
                symtab_sec.add_symbol(sym)
                r_offset = reloc_origin.addend - sec.start_offs
                elf_reloc = ElfRela(sym, r_offset, reloc_dest[1], 0)
                reloc_sec.add_reloc(elf_reloc)

            reloc_sec.header.sh_info = elf_file.get_section_index(sec.sec_name)
            elf_file.add_section(reloc_sec)

    symtab_sec_idx = elf_file.add_section(symtab_sec)

    for rs in reloc_secs:
        rs.header.sh_link = symtab_sec_idx

    return elf_file


def slice_rel(rel_file: Path, out_path: Path) -> None:
    if not rel_file.is_file():
        print_err('Fatal error: File', str(rel_file), 'not found!')
        return

    reloc_syms: set[RelocSym] = set()
    module_relocs: dict[int, dict[RelocSym, tuple[RelocSym, PPC_RELOC_TYPE]]] = {}

    # Read slices
    with open(rel_file, 'rb') as f:
        with open(f'{SLICEDIR}/{rel_file.with_suffix(".json").name}') as sf:
            slice_file: SliceFile = load_slice_file(sf)
            mod_num = slice_file.meta.mod_num
            print(f'Slicing module {mod_num} ({f.name})...')
            rel_file = Rel(mod_num, file=f)
            read_reloc_refs(rel_file, mod_num, reloc_syms, module_relocs)

            for slice in slice_file.slices:
                elf = extract_slice(rel_file, slice, module_relocs)
                out_filepath = out_path.joinpath(slice.slice_name)
                out_filepath.parents[0].mkdir(parents=True, exist_ok=True)
                with open(out_filepath, 'wb') as ef:
                    ef.write(bytes(elf))


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Slices REL files.')
    parser.add_argument('rel_file', type=Path, help='REL file to be sliced.')
    parser.add_argument('--out_path', '-o', default=Path('bin/sliced'), type=Path, help='Path the slices will be stored to.')
    args = parser.parse_args()
    slice_rel(args.rel_file, args.out_path)
