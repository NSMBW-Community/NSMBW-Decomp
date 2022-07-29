#!/usr/bin/env python3
# Decomp.me scratch generator

import json
from pathlib import Path
import sys
from typing import Union
import urllib.parse, urllib.request

toolsfolder = 'tools'
codesections = ['.init', '.text']
slicefolder: Path = Path('slices')
originalfolder: Path = Path('original')
includefolder: Path = Path('include')
sys.path.append(toolsfolder)

from color_term import *
from dolfile import Dol, DolSection
from relfile import Rel, RelSection


def get_code(rawdata: Union[Dol, Rel], idx: int, startaddr: int, endaddr: int, base_rel_addr: int) -> bytes:
    section: Union[DolSection, RelSection] = rawdata.sections[idx]

    if type(section) is DolSection:
        if section.virt_addr > startaddr or section.virt_addr + section.sec_len < endaddr:
            return b''
        return bytes(section.data[startaddr-section.virt_addr:endaddr-section.virt_addr+4])
    else:
        virt_addr = base_rel_addr + sum(rawdata.sections[i].data_length() for i in range(idx))
        if virt_addr > startaddr or virt_addr + section.data_length() < endaddr:
            return b''
        return bytes(section.get_data()[startaddr-virt_addr:endaddr-virt_addr+4])


def process_header(include: Path, contextfiles: set[Path]):
    # Make path relative
    include = includefolder.joinpath(include)

    # Check if already read
    if include in contextfiles:
        return ''
    contextfiles.add(include)

    # Check if file exists
    if not include.is_file():
        print_warn('File', str(include), 'not found! Skipping...')
        return ''

    ret = ''
    with open(include) as f:
        for line in f:
            if line.startswith('#include'):
                ret += process_header(line[line.find('<')+1:line.find('>')], contextfiles)
            elif not line.startswith('#pragma once'):
                ret += line

    return ret


def main(source_file: str, startaddr: int, endaddr: int, includes: list[str]) -> None:

    # Check if the slice file for source exists
    slicefile: Path = slicefolder.joinpath(source_file + '.json')
    if not slicefile.is_file():
        print_err('Fatal error: File', str(slicefile), 'not found!')
        return

    # Load the JSON file
    with open(slicefile) as f:
        slicejson: dict = json.load(f)

    # Get the file type
    type: str = slicejson['meta']['type'].lower()

    # Check if the original file for source exists
    originalfile: Path = originalfolder.joinpath(source_file + f'.{type}')
    if not originalfile.is_file():
        print_err('Fatal error: File', str(originalfile), 'not found!')
        return

    # Read the file in memory
    with open(originalfile, 'rb') as f:
        rawdata: Union[Dol, Rel] = None
        base_rel_addr: int = 0
        if type == 'rel':
            rawdata = Rel(0, file=f)
            base_rel_addr = int(slicejson['meta']['baseAddr'], 16)
        elif type == 'dol':
            rawdata = Dol(f)

    # If the file is not valid, bail
    if not rawdata:
        print_err('Invalid file provided!')
        return

    # Check that the startaddr is less then endaddr
    if startaddr > endaddr:
        t = endaddr
        endaddr = startaddr
        startaddr = t

    # Align them by 4
    startaddr &= ~3
    endaddr += endaddr % 4

    # Get the code according to the section index for the file
    for section in codesections:
        if section in slicejson['meta']['sections']:
            idx: int = slicejson['meta']['sections'][section]['index']
            raw_code = get_code(rawdata, idx, startaddr, endaddr, base_rel_addr)
            if raw_code:
                break

    # If the corresponding code was not found or is empty, bail
    if not raw_code:
        print_err('Fatal error: Invalid address range provided!')
        return

    # Create the asm string
    splitcode = '\n'.join('.long 0x%s' % raw_code[i:i+4].hex() for i in range(0, len(raw_code), 4))

    # Generate context
    context = ''
    contextfiles = set()
    for include in includes:
        context += process_header(Path(include.lstrip('/').lstrip('\\')), contextfiles)

    # Send request
    data = urllib.parse.urlencode({'target_asm': splitcode, 'context': context, 'platform': 'gc_wii', 'compiler': 'mwcc_43_151'}).encode('ascii')
    try:
        with urllib.request.urlopen('https://decomp.me/api/scratch', data) as f:
            result = json.loads(f.read().decode('utf-8'))
            print_success(f'Scratch created successfully at https://decomp.me{result["html_url"]}!')
    except Exception as e:
        print_err('Failed to request scratch:', e)


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser(description='Creates a decomp.me scratch from the given info.')
    parser.add_argument('source_file', type=str, help='Source file, without the extension.')
    parser.add_argument('-s', '--startaddr', type=str, help='Address to start reading the asm from.', required=True)
    parser.add_argument('-e', '--endaddr', type=str, help='Address where the asm reading ends (included).', required=True)
    parser.add_argument('-i', '--include', type=str, help='One or more files to be used as context. Path is relative to the include folder.', nargs='*', action='extend')
    args = parser.parse_args()
    main(args.source_file, int(args.startaddr, 16), int(args.endaddr, 16), args.include if args.include else [])
