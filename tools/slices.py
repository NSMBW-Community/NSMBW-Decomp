from enum import Enum
import json
import typing

class JSONSliceData(typing.TypedDict):
    name: str
    compilerFlags: str
    memoryRanges: dict[str, str]

class SliceSection:
    def __init__(self, sec_name: str, sec_idx: int, start_offs: int, end_offs: int, alignment):
        self.sec_name = sec_name
        self.sec_idx = sec_idx
        self.start_offs = start_offs
        self.end_offs = end_offs
        self.alignment = alignment

    def __repr__(self) -> str:
        return f'<SliceSection {self.sec_name} (#{self.sec_idx}) {self.start_offs:0x}-{self.end_offs:0x}, align={self.alignment}>'

    def contains(self, section: int, addend: int):
        return section == self.sec_idx and self.start_offs <= addend <= self.end_offs

class Slice:
    def __init__(self, slice_name: str, slice_secs: list[SliceSection]):
        self.slice_name = slice_name
        self.slice_secs = slice_secs

    def __repr__(self) -> str:
        return f'<Slice {self.slice_name}, {self.slice_secs}>'

class SliceSectionInfo:
    def __init__(self, index: int, align: int):
        self.index = index
        self.align = align

class SliceType(Enum):
    REL = 0
    DOL = 1

class SliceMeta:
    def __init__(self, secs: dict[str, SliceSectionInfo], type: SliceType, name: str):
        self.secs = secs
        self.type = type
        self.name = name
    
    def from_meta(meta: dict) -> "SliceMeta":
        secs: dict[str, SliceSectionInfo] = dict()
        for sec in meta['sections']:
            secs[sec] = SliceSectionInfo(meta['sections'][sec]['index'], meta['sections'][sec]['align'])
        st = SliceType.REL if meta['type'] == 'REL' else SliceType.DOL
        return SliceMeta(secs, st, meta['fileName'])

class SliceFile:
    def __init__(self, slices: list[Slice], meta: SliceMeta):
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
        slices.append(Slice(slice_name, slice_sections))
    return SliceFile(slices, slice_meta)