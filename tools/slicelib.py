#!/usr/bin/env python3
# Slice definitions

from enum import Enum
import json
import typing

class JSONSliceData(typing.TypedDict):
    name: str
    compilerFlags: str
    memoryRanges: dict[str, str]


class SliceSection:
    def __init__(self, sec_name: str, sec_idx: int, start_offs: int, end_offs: int, alignment: int) -> None:
        self.sec_name = sec_name
        self.sec_idx = sec_idx
        self.start_offs = start_offs
        self.end_offs = end_offs
        self.alignment = alignment

    def __repr__(self) -> str:
        return f'<SliceSection {self.sec_name} (#{self.sec_idx}) {self.start_offs:0x}-{self.end_offs:0x}, align={self.alignment}>'

    def contains(self, section: int, addend: int) -> bool:
        return section == self.sec_idx and self.start_offs <= addend < self.end_offs


class Slice:
    def __init__(self, slice_name: str, source: str, slice_secs: list[SliceSection], cc_flags: str, deadstrip: list[str], no_deadstrip: list[str]) -> None:
        self.slice_name = slice_name
        self.slice_src = source
        self.slice_secs = slice_secs
        self.cc_flags = cc_flags.split(' ') if cc_flags else None
        self.deadstrip = deadstrip
        self.no_deadstrip = no_deadstrip

    def __repr__(self) -> str:
        return f'<Slice {self.slice_name}, {self.slice_secs}>'


class SliceSectionInfo:
    def __init__(self, index: int, align: int, size: int, offset: int) -> None:
        self.index = index
        self.align = align
        self.size = size
        self.offset = offset


class SliceType(Enum):
    REL = 0
    DOL = 1


class SliceMeta:
    def __init__(self, secs: dict[str, SliceSectionInfo]) -> None:
        self.secs = secs
        self.type: str
        self.name: str
        self.mod_num: int
        self.default_compiler_flags: list[str]
        self.force_active: list[str]

    def from_meta(meta: dict) -> 'SliceMeta':
        secs: dict[str, SliceSectionInfo] = {}
        for sec in meta['sections']:
            sec_meta = meta['sections'][sec]
            s_size = int(sec_meta['size'], 16)
            s_offset = int(sec_meta['offset'], 16) if 'offset' in sec_meta else 0
            secs[sec] = SliceSectionInfo(sec_meta['index'], sec_meta['align'], s_size, s_offset)
        sm = SliceMeta(secs)
        sm.type = SliceType.REL if meta['type'] == 'REL' else SliceType.DOL
        sm.name = meta['fileName']
        sm.mod_num = meta['moduleNum']
        dcf = meta.get('defaultCompilerFlags', None)
        sm.default_compiler_flags = dcf.split(' ')
        return sm


class SliceFile:
    def __init__(self, slices: list[Slice], meta: SliceMeta) -> None:
        self.meta: SliceMeta = meta
        self.slices: list[Slice] = slices

def make_filler_slice(name: str, sec_range: dict[str, tuple[int, int]], slice_meta: SliceMeta) -> Slice:
    slice_sections: list[SliceSection] = []
    for sec_name in sec_range:
        start, end = sec_range[sec_name]
        if start == end:
            continue

        sec_info = slice_meta.secs[sec_name]
        start += sec_info.offset
        end += sec_info.offset
        slice_sections.append(SliceSection(sec_name, sec_info.index, start, end, sec_info.align))

    if len(slice_sections) > 0:
        return Slice(name, None, slice_sections, None, None, None)

def load_slice_file(file: typing.TextIO) -> SliceFile:
    slice_json = json.load(file)
    slice_meta = SliceMeta.from_meta(slice_json['meta'])
    slices: list[Slice] = []
    slice: JSONSliceData

    curr_sec_positions = {s: 0 for s in slice_meta.secs if slice_meta.secs[s].size != 0}

    filler_slice_idx = 0

    for slice in slice_json['slices']:
        slice_name = slice['name']
        slice_sections: list[SliceSection] = []
        
        filler_sec_range = {s: (0, 0) for s in curr_sec_positions}

        for sec_name in slice['memoryRanges']:
            range_str = slice['memoryRanges'][sec_name]
            offs = slice_meta.secs[sec_name].offset # so that the slice range is relative to the entire section, not the subsection
            begin, end = [int(x, 16) - offs for x in range_str.split('-')]
            sec_info = slice_meta.secs[sec_name]
            slice_sections.append(SliceSection(sec_name, sec_info.index, begin, end, sec_info.align))

            filler_sec_range[sec_name] = (curr_sec_positions[sec_name], begin)
            curr_sec_positions[sec_name] = end

        src = slice.get('source', None)
        flags = slice.get('compilerFlags', None)
        ds = slice.get('deadstrip', None)
        nds = slice.get('noDeadstrip', None)

        # Generate filler slice
        filler_slice = make_filler_slice(f'filler_{filler_slice_idx}.o', filler_sec_range, slice_meta)
        if filler_slice is not None:
            slices.append(filler_slice)
            filler_slice_idx += 1

        # Add actual slice
        slices.append(Slice(slice_name, src, slice_sections, flags, ds, nds))

    # Ensure filler slices extend to the end
    filler_sec_range = {s: (0, 0) for s in curr_sec_positions}
    for sec_name in curr_sec_positions:
        filler_sec_range[sec_name] = (curr_sec_positions[sec_name], slice_meta.secs[sec_name].size)
    
    filler_slice = make_filler_slice(f'filler_{filler_slice_idx}.o', filler_sec_range, slice_meta)
    if filler_slice is not None:
        slices.append(filler_slice)

    return SliceFile(slices, slice_meta)
