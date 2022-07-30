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
    def __init__(self, slice_name: str, source: str, slice_secs: list[SliceSection], cc_flags: str, force_active: list[str]) -> None:
        self.slice_name = slice_name
        self.slice_src = source
        self.slice_secs = slice_secs
        self.cc_flags = cc_flags.split(' ') if cc_flags else None
        self.force_active = force_active

    def __repr__(self) -> str:
        return f'<Slice {self.slice_name}, {self.slice_secs}>'


class SliceSectionInfo:
    def __init__(self, index: int, align: int) -> None:
        self.index = index
        self.align = align


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
            secs[sec] = SliceSectionInfo(meta['sections'][sec]['index'], meta['sections'][sec]['align'])
        sm = SliceMeta(secs)
        sm.type = SliceType.REL if meta['type'] == 'REL' else SliceType.DOL
        sm.name = meta['fileName']
        sm.mod_num = meta['moduleNum']
        dcf = meta['defaultCompilerFlags'] if 'defaultCompilerFlags' in meta else None
        sm.default_compiler_flags = dcf.split(' ')
        return sm


class SliceFile:
    def __init__(self, slices: list[Slice], meta: SliceMeta) -> None:
        self.meta: SliceMeta = meta
        self.slices: list[Slice] = slices


def load_slice_file(file: typing.TextIO) -> SliceFile:
    slice_json = json.load(file)
    slice_meta = SliceMeta.from_meta(slice_json['meta'])
    slices: list[Slice] = []
    slice: JSONSliceData

    for slice in slice_json['slices']:
        slice_name = slice['name']
        slice_sections: list[SliceSection] = []

        for sec_name in slice['memoryRanges']:
            range_str = slice['memoryRanges'][sec_name]
            begin, end = [int(x, 16) for x in range_str.split('-')]
            sec_info = slice_meta.secs[sec_name]
            slice_sections.append(SliceSection(sec_name, sec_info.index, begin, end, sec_info.align))

        src = slice['source'] if 'source' in slice else None
        flags = slice['compilerFlags'] if 'compilerFlags' in slice else None
        fa = slice['forceActive'] if 'forceActive' in slice else None
        slices.append(Slice(slice_name, src, slice_sections, flags, fa))

    return SliceFile(slices, slice_meta)
