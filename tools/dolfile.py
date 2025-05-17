#!/usr/bin/env python3
# DOL definitions and helper functions
# See https://wiki.tockdom.com/wiki/DOL_(File_Format)

import math
import struct
from typing import BinaryIO, Optional

class DolSection:
    def __init__(self, file: Optional[bytearray]=None, info_offset: int=0, unused: bool=False) -> None:
        self.unused = unused

        if file:
            self._read(file, info_offset)
        else:
            self.sec_len: int = 0
            self.data: bytearray = bytearray()
            self.virt_addr: int = 0

    def __repr__(self) -> str:
        return f'<Section: load address 0x{self.virt_addr:0x} length {self.sec_len:0x}>'

    def set_data(self, data: bytearray) -> None:
        self.data = data
        self.sec_len = len(data)

    def _read(self, file: bytearray, info_offset: int) -> None:
        data_offs = int.from_bytes(file[info_offset:info_offset+4], byteorder='big')
        self.sec_len = int.from_bytes(file[info_offset+0x90:info_offset+0x90+4], byteorder='big')
        self.virt_addr = int.from_bytes(file[info_offset+0x48:info_offset+0x48+4], byteorder='big')

        self.data = file[data_offs:data_offs+self.sec_len]
        assert len(self.data) == self.sec_len


class Dol:
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
            self.sections.append(DolSection(bytes, i * 4))

        self.bss_addr, self.bss_len, self.entry = struct.unpack('>III', bytes[0xd8:0xe4])

    def add_section(self, section: DolSection) -> None:
        self.sections.append(section)

    def write(self, file: BinaryIO) -> None:
        assert len(self.sections) == 18

        section_data_locs: list[int] = []
        pos = 0x100
        for sec in self.sections:
            # Align to 0x20
            pos = math.ceil(pos / 0x20) * 0x20
            section_data_locs.append(pos if not sec.unused else 0)
            pos += sec.sec_len

        # Write section offsets
        for sec_offs in section_data_locs:
            file.write(sec_offs.to_bytes(4, 'big'))

        # Write section addresses
        for x in self.sections:
            file.write(x.virt_addr.to_bytes(4, 'big'))

        # Write section lengths (aligned by 0x20)
        for x in self.sections:
            rounded_len = math.ceil(x.sec_len / 0x20) * 0x20
            file.write(rounded_len.to_bytes(4, 'big'))

        # Write bss info and entry point
        file.write(struct.pack('>III28x', self.bss_addr, self.bss_len, self.entry))

        # Write sections
        for i, sec in enumerate(self.sections):
            if not sec.unused:
                file.seek(section_data_locs[i])
                file.write(sec.data)
