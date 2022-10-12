#!/usr/bin/env python3
# Slices a DOL file into .o files

from pathlib import Path

from color_term import *
from dolfile import Dol
from elffile import *
from elfconsts import *
from project_settings import *
from slicelib import *

def extract_slice(dol_file: Dol, slice: Slice, syms: dict[int, str]) -> ElfFile:
    elf_file = ElfFile(ET.ET_REL, EM.EM_PPC)

    symtab_sec = ElfSymtab('.symtab')

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
            elfsym = None

            # -1 = uninitialized data section
            if sec.sec_idx == -1:
                # Search in unitialized sections (i.e. .bss, .sbss and .sbss2)
                # This is stored a bit confusingly in the dol file format as it only has a field for the address of .bss
                unitialized_sec_starts = []
                curr = dol_file.bss_addr
                rem_size = dol_file.bss_len
                for i in range(len(dol_file.sections)):
                    curr_sec = dol_file.sections[i]
                    if curr_sec.virt_addr > curr:
                        unitialized_sec_starts.append(curr)
                        diff = curr_sec.virt_addr - curr
                        rem_size -= diff
                        curr = curr_sec.virt_addr + curr_sec.sec_len
                if rem_size > 0:
                    unitialized_sec_starts.append(curr)

                # Must use correct section
                lookup = {
                    '.bss': unitialized_sec_starts[0],
                    '.sbss': unitialized_sec_starts[1],
                    '.sbss2': unitialized_sec_starts[2],
                }

                offs = syms[sym] - lookup[sec.sec_name]
                if sec.contains(-1, offs):
                    elfsym = ElfSymbol(sym, st_value=offs-sec.start_offs, st_info_bind=STB.STB_GLOBAL, st_shndx=sec_idx)
            else:
                # Search in normal section
                for idx, dol_sec in enumerate(dol_file.sections):
                    offs = syms[sym] - dol_sec.virt_addr
                    if sec.contains(idx, offs):
                        elfsym = ElfSymbol(sym, st_value=offs-sec.start_offs, st_info_bind=STB.STB_GLOBAL, st_shndx=sec_idx)
                        break

            if elfsym != None:
                symtab_sec.add_symbol(elfsym)

    elf_file.add_section(symtab_sec)

    return elf_file


def slice_dol(dol_file: Path, out_path: Path) -> None:
    if not dol_file.is_file():
        print_err('Fatal error: File', str(dol_file), 'not found!')
        return

    # TODO: use an actual symbol map file
    syms: dict[int, str] = {}
    with open(SYMBOL_FILE) as sym_file:
        for line in sym_file:
            if line != '\n':
                sym, addr = line.split('=')
                if sym in syms:
                    print_warn('Warning: symbol', sym, 'defined multiple times in', SYMBOL_FILE.name, end='!\n')
                syms[sym] = int(addr, 16)

    # Read slices
    with open(dol_file, 'rb') as f:
        with open(f'{SLICEDIR}/{dol_file.with_suffix(".json").name}') as sf:
            slice_file: SliceFile = load_slice_file(sf)
            print(f'Slicing module 0 ({f.name})...')
            dol_file = Dol(file=f)

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
