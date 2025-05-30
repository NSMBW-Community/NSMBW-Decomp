#!/usr/bin/env python3
# Slice definitions

import json
from dataclasses import dataclass, field
from enum import Enum
from pathlib import Path
from typing import Optional
from utils.json_parser import from_json

def parse_int_str(value: str | int) -> int:
    if isinstance(value, str):
        return int(value, 16)
    return value


@dataclass
class SliceData:
    source: str
    memoryRanges: dict[str, str]
    compilerFlags: str = field(default='')
    nonMatching: bool = field(default=False)


@dataclass
class SliceSectionInfo:
    index: int
    align: int
    size: int
    secAlign: int = field(default=-1)
    offset: int = field(default=0)
    addr: int = field(default=0)

    def __post_init__(self):
        self.size = parse_int_str(self.size)
        self.addr = parse_int_str(self.addr)
        self.offset = parse_int_str(self.offset)


class SliceType(Enum):
    REL = 0
    DOL = 1


@dataclass
class SliceMeta:
    sections: dict[str, SliceSectionInfo]
    type: SliceType
    fileName: str
    moduleNum: int
    defaultCompilerFlags: str
    baseAddr: int = field(default=0)

    def __post_init__(self):
        self.baseAddr = parse_int_str(self.baseAddr)


@dataclass
class SliceSection:
    sec_name: str
    sec_idx: int
    start_offs: int
    end_offs: int
    alignment: int

    def contains(self, section: int, addend: int) -> bool:
        return section == self.sec_idx and self.start_offs <= addend < self.end_offs


@dataclass
class Slice:
    sliceName: str
    source: str
    sliceSecs: list[SliceSection] = field(default_factory=list)
    ccFlags: str = field(default='')
    nonMatching: bool = field(default=False)


@dataclass
class SliceFile:
    meta: SliceMeta
    slices: list[SliceData]
    deadstrip: list[str] = field(default_factory=list)
    keepWeak: list[str] = field(default_factory=list)
    parsed_slices: list[Slice] = field(init=False, default_factory=list)
    path: Path = field(init=False, default=Path())

    def unit_name(self) -> str:
        return Path(self.meta.fileName).stem


def make_filler_slice(slice_name: str, sec_range: dict[str, tuple[int, int]], slice_meta: SliceMeta) -> Optional[Slice]:
    slice_sections: list[SliceSection] = []
    for section_name, section in sec_range.items():
        start, end = section
        if start == end:
            continue

        section_info = slice_meta.sections[section_name]
        slice_sections.append(SliceSection(section_name, section_info.index, start, end, section_info.align))

    if len(slice_sections) > 0:
        return Slice(slice_name, '', slice_sections)


def load_slice_file(src: Path) -> SliceFile:

    # Load slice file
    slice_file = from_json(SliceFile, json.loads(src.read_text()))
    slice_file.path = src
    slice_meta = slice_file.meta

    # Initialize loop
    filler_slice_idx = 0
    curr_sec_positions = {name: section.offset for name, section in slice_meta.sections.items() if section.size != 0}
    for slice in slice_file.slices:

        # Create parsed slice
        filler_sec_range: dict[str, tuple] = {section: (0, 0) for section in curr_sec_positions}
        slice_name = str(Path(slice.source).with_suffix('.o'))
        parsed_slice = Slice(slice_name, slice.source, ccFlags=slice.compilerFlags, nonMatching=slice.nonMatching)

        # Parse slice sections
        slice_sections = slice.memoryRanges
        for section, addrRange in slice_sections.items():

            # Create parsed section
            section_info = slice_meta.sections[section]
            begin, end = (int(x, 16) for x in addrRange.split('-'))
            parsed_slice_section = SliceSection(section, section_info.index, begin, end, section_info.align)
            parsed_slice.sliceSecs.append(parsed_slice_section)

            # Set memory range of filler slice
            filler_sec_range[section] = (curr_sec_positions[section], begin)
            curr_sec_positions[section] = end

        # Generate and add filler slice if applicable
        filler_slice = make_filler_slice(f'filler_{filler_slice_idx}.o', filler_sec_range, slice_meta)
        if filler_slice is not None:
            slice_file.parsed_slices.append(filler_slice)
            filler_slice_idx += 1

        # Add created slice
        slice_file.parsed_slices.append(parsed_slice)

    # Add last slice which extends to the end of each section, if applicable
    filler_sec_range = {section: (0, 0) for section in curr_sec_positions}
    for name, offset in curr_sec_positions.items():
        section_end = slice_meta.sections[name].size + slice_meta.sections[name].offset
        filler_sec_range[name] = (offset, section_end)

    filler_slice = make_filler_slice(f'filler_{filler_slice_idx}.o', filler_sec_range, slice_meta)
    if filler_slice is not None:
        slice_file.parsed_slices.append(filler_slice)

    # Return parsed data
    return slice_file
