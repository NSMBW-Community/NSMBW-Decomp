#!/usr/bin/env python3
# REL definitions and helper functions, by RootCubed

import struct
import typing

# See https://wiki.tockdom.com/wiki/DOL_(File_Format)

class DolSection:
    def __init__(self, file: typing.BinaryIO = None, info_offset: int = 0, unused: bool = False):
        self.unused = unused
        if file:
            self._read(file, info_offset)
        else:
            self.sec_len: int = 0
            self.data: bytearray = bytearray()
            self.virt_addr: int = 0

    def __repr__(self):
        return f'<Section: load address 0x{self.virt_addr:0x} length {self.sec_len:0x}>'

    def set_data(self, data: bytearray):
        self.data = data
        self.sec_len = len(data)

    def _read(self, file: bytearray, info_offset: int):
        data_offs = int.from_bytes(file[info_offset:info_offset+4], byteorder='big')
        self.sec_len = int.from_bytes(file[info_offset+0x90:info_offset+0x90+4], byteorder='big')
        self.virt_addr = int.from_bytes(file[info_offset+0x48:info_offset+0x48+4], byteorder='big')
        
        self.data = file[data_offs:data_offs+self.sec_len]
        assert len(self.data) == self.sec_len


class Dol:
    def __init__(self, file: typing.BinaryIO = None):
        
        self.bss_addr: int = 0
        self.bss_len: int = 0
        self.entry: int = 0
        self.sections: list[DolSection] = []
        
        if file:
            self.read(file)

    def read(self, file: typing.BinaryIO):
        bytes = bytearray(file.read())
        for i in range(18):
            self.sections.append(DolSection(bytes, i * 4))

        (
            self.bss_addr,
            self.bss_len,
            self.entry
        ) = struct.unpack('>III', bytes[0xd8:0xe4])
    
    def add_section(self, section: DolSection):
        self.sections.append(section)

    def write(self, file: typing.BinaryIO):
        section_data_locs: list[int] = []
        pos = 0x100
        for sec in self.sections:
            if not sec.unused:
                section_data_locs.append(pos)
            else:
                section_data_locs.append(0)
            pos += sec.sec_len
        
        # Section offsets
        for i in range(18):
            file.write(section_data_locs[i].to_bytes(4, 'big'))

        # Section addresses
        for i in range(18):
            file.write(self.sections[i].virt_addr.to_bytes(4, 'big'))

        # Section lengths
        for i in range(18):
            file.write(self.sections[i].sec_len.to_bytes(4, 'big'))

        file.write(struct.pack('>III28x', self.bss_addr, self.bss_len, self.entry))

        for sec in self.sections:
            if not sec.unused:
                file.write(sec.data)