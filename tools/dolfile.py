#!/usr/bin/env python3

# dolfile.py
# DOL definitions and helper functions
# See https://wiki.tockdom.com/wiki/DOL_(File_Format)

import struct
from typing import BinaryIO, Optional
from elffile import ElfSection

def align(value: int, alignment: int) -> int:
    return (value + alignment - 1) & ~(alignment - 1)


class DolSection:
    def __init__(self, unused: bool=False) -> None:
        self.unused = unused
        self.sec_len: int = 0
        self.data: bytearray = bytearray()
        self.virt_addr: int = 0

    def __repr__(self) -> str:
        return f'<Section: load address 0x{self.virt_addr:0x} length {self.sec_len:0x}>'

    def set_data(self, data: bytearray) -> None:
        self.data = data
        self.sec_len = len(data)

    @staticmethod
    def from_dol_file(file: bytearray, info_offset: int) -> 'DolSection':
        sec = DolSection()

        data_offs = int.from_bytes(file[info_offset:info_offset+4], byteorder='big')
        sec.sec_len = int.from_bytes(file[info_offset+0x90:info_offset+0x90+4], byteorder='big')
        sec.virt_addr = int.from_bytes(file[info_offset+0x48:info_offset+0x48+4], byteorder='big')

        sec.data = file[data_offs:data_offs+sec.sec_len]
        assert len(sec.data) == sec.sec_len
        return sec

    @staticmethod
    def from_elf_section(section: ElfSection) -> 'DolSection':
        sec = DolSection()
        sec.virt_addr = section.header.sh_addr
        sec.set_data(bytearray(section.data))
        return sec


class Dol:
    header_struct = struct.Struct('>57I28x')

    def __init__(self, file: Optional[BinaryIO]=None) -> None:
        self.bss_addr: int = 0
        self.bss_len: int = 0
        self.entry: int = 0
        self.sections: list[DolSection] = []

        if file:
            self.read(file)

    def read(self, file: BinaryIO) -> None:
        bytes = bytearray(file.read())
        for i in range(18):
            self.add_section(DolSection.from_dol_file(bytes, i * 4))

        self.bss_addr, self.bss_len, self.entry = struct.unpack_from('>III', bytes, 0xD8)

    def add_section(self, section: DolSection) -> None:
        self.sections.append(section)

    def write(self, file: BinaryIO) -> None:
        assert len(self.sections) == 18

        sec_offsets: list[int] = []
        sec_addresses: list[int] = []
        sec_lengths: list[int] = []

        pos = self.header_struct.size
        for sec in self.sections:
            pos = align(pos, 0x20)
            sec_offsets.append(pos if not sec.unused else 0)
            sec_addresses.append(sec.virt_addr if not sec.unused else 0)
            sec_lengths.append(align(sec.sec_len, 0x20) if not sec.unused else 0)
            pos += sec.sec_len

        # Write bss info and entry point
        file.write(self.header_struct.pack(*sec_offsets, *sec_addresses, *sec_lengths,
                                           self.bss_addr, self.bss_len, self.entry))

        # Write sections
        for i, sec in enumerate(self.sections):
            if not sec.unused:
                file.seek(sec_offsets[i])
                file.write(sec.data)
