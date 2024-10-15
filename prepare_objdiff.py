#!/usr/bin/env python3
# Setup project for use with objdiff
# Uses dtk to split the dol and rel files and creates objdiff.json

import argparse
import os
import platform
import shutil
import subprocess
import sys
from pathlib import Path
import urllib.request
import yaml
import json
import stat

sys.path.append('tools')

from color_term import *
from project_settings import *
from slicelib import SliceFile, SliceType, load_slice_file

parser = argparse.ArgumentParser(description='Sets up the project for use with objdiff.')
parser.add_argument('-m', help='Path to a map file to use')
args = parser.parse_args()

slices: list[SliceFile] = []

for file in SLICEDIR.glob('*.json'):
    with open(file) as sf:
        slices.append(load_slice_file(sf))

# Ensure correct order of slices
slices = sorted(slices, key=lambda x: x.meta.mod_num)

config_yaml = {
    'modules': [],
    'write_asm': False
}

if args.m:
    config_yaml['symbols'] = args.m

SECTION_TYPES = {
    '.init': 'code',
    '.text': 'code',
    '.ctors': 'rodata',
    '.dtors': 'rodata',
    '.rodata': 'rodata',
    '.data': 'data',
    '.bss': 'bss',
    '.sdata': 'data',
    '.sbss': 'bss',
    '.sbss2': 'rodata',
    '.sdata2': 'bss',
}

Path(f'{BUILDDIR}/dtk').mkdir(parents=True, exist_ok=True)

# Step 1: Create config.yaml and splits files

for slice_file in slices:
    if slice_file.meta.type == SliceType.DOL:
        config_yaml['object'] = f'original/{slice_file.meta.name}'
        config_yaml['splits'] = f'{BUILDDIR}/dtk/dtk_splits_{Path(slice_file.meta.name).stem}.txt'
    else:
        config_yaml['modules'].append({
            'object': f'original/{slice_file.meta.name}',
            'splits': f'{BUILDDIR}/dtk/dtk_splits_{Path(slice_file.meta.name).stem}.txt'
        })

    splits_file = 'Sections:\n'

    sec_origin = {}
    for secname, info in slice_file.meta.secs.items():
        if info.size == 0:
            continue
        if not '$' in secname:
            splits_file += f'\t{secname: <15} type:{SECTION_TYPES[secname.split("$")[0]]} align:{info.align}\n'
        sec_origin[secname] = info.addr
        if secname == '.init':
            splits_file += f'\t{"extab": <15} type:rodata align:8\n'
            splits_file += f'\t{"extabindex": <15} type:rodata align:32\n'

    splits_file += '\n'

    for slice in slice_file.slices:
        if slice.slice_name.startswith('filler_'):
            continue

        name = slice.slice_src if slice.slice_src else slice.slice_name
        splits_file += f'{name}:\n'
        for slice_sec in slice.slice_secs:
            start = slice_sec.start_offs + sec_origin[slice_sec.sec_name]
            end = slice_sec.end_offs + sec_origin[slice_sec.sec_name]
            splits_file += f'\t{slice_sec.sec_name: <15} start:0x{start:08X} end:0x{end:08X}\n'
        splits_file += '\n'

    with open(f'{BUILDDIR}/dtk/dtk_splits_{Path(slice_file.meta.name).stem}.txt', 'w') as f:
        f.write(splits_file)


with open(f'{BUILDDIR}/dtk/config.yaml', 'w') as f:
    f.write(yaml.dump(config_yaml))

# Step 2: Generate splits with dtk

def get_dtk(tag: str) -> str:
    uname = platform.uname()
    suffix = ''
    system = uname.system.lower()
    if system == 'darwin':
        system = 'macos'
    elif system == 'windows':
        suffix = '.exe'
    arch = uname.machine.lower()
    if arch == 'amd64':
        arch = 'x86_64'

    repo = 'https://github.com/encounter/decomp-toolkit'
    exe_name = f'dtk-{system}-{arch}{suffix}'
    bin_path = f'bin/{exe_name}'
    url = f'{repo}/releases/download/{tag}/{exe_name}'

    # Check if the tool is already downloaded
    if not Path(bin_path).exists():
        print(f'Downloading {url} to {bin_path}')
        req = urllib.request.Request(url, headers={'User-Agent': 'Mozilla/5.0'})
        with urllib.request.urlopen(req) as response:
            with open(bin_path, 'wb') as f:
                shutil.copyfileobj(response, f)
            st = os.stat(bin_path)
            os.chmod(bin_path, st.st_mode | stat.S_IEXEC)

    return bin_path

dtk_path = get_dtk('v0.9.6')
cmd = [dtk_path, 'dol', 'split', f'{BUILDDIR}/dtk/config.yaml', f'{BUILDDIR}/dtk/splits']
out = subprocess.run(cmd)
if out.returncode != 0:
    sys.exit()

print_success('Successfully generated splits with dtk. Now generating objdiff.json...')

# Step 3: Generate objdiff.json

with open(f'{BUILDDIR}/dtk/splits/config.json', 'r') as f:
    objdiff_json = json.load(f)

objdiff_json['min_version'] = '2.0.0'
objdiff_json['custom_make'] = 'python'
objdiff_json['custom_args'] = ['build.py', '--objdiff-o']
objdiff_json['watch_patterns'] = [
    '*.c', '*.cpp',
    '*.h', '*.hpp',
    '*.s', '*.S',
    '*.py',
    '*.json'
]
objdiff_json['progress_categories'] = [
    {
        "id": "dol",
        "name": "DOL"
    },
    {
        "id": "modules",
        "name": "Modules"
    }
]
del objdiff_json['ldscript']
del objdiff_json['module_id']
del objdiff_json['entry']
del objdiff_json['version']

for slice_file in slices:
    for slice in slice_file.slices:
        stem = Path(slice_file.meta.name).stem
        if slice_file.meta.type == SliceType.DOL:
            loc = objdiff_json['units']
        else:
            loc = objdiff_json['modules']
            loc = next(x for x in loc if x['name'] == stem)['units']

        unit = next((u for u in loc if u['name'] == slice.slice_src), None)
        if unit is None:
            if slice.slice_src is not None:
                print_err(f'Unit {slice.slice_name} not found in objdiff.json')
            continue

        unit['base_path'] = f'{BUILDDIR}/compiled/{stem}/{slice.slice_name}'
        unit['metadata'] = {
            'complete': slice.slice_src is not None,
            'source_path': f'source/{slice.slice_src}'
        }


def patch_unit(unit, progress_categories, module_name):
    if not 'metadata' in unit:
        unit['metadata'] = {}
    unit['metadata']['auto_generated'] = unit['autogenerated']
    del unit['autogenerated']
    unit['metadata']['progress_categories'] = progress_categories
    unit['target_path'] = unit['object']
    del unit['object']

for unit in objdiff_json['units']:
    patch_unit(unit, ['dol'], 'dol')

for module in objdiff_json['modules']:
    for unit in module['units']:
        patch_unit(unit, ['modules'], module['name'])

# Merge modules into regular units
for module in objdiff_json['modules']:
    objdiff_json['units'].extend(module['units'])
del objdiff_json['modules']

# Preserve symbol mappings
if Path('objdiff.json').exists():
    objdiff_prev = json.load(open('objdiff.json'))
    for unit in objdiff_prev['units']:
        if 'symbol_mappings' not in unit:
            continue
        idx = None
        for i, prev_unit in enumerate(objdiff_json['units']):
            if prev_unit['name'] == unit['name']:
                idx = i
                break
        if idx is not None:
            objdiff_json['units'][idx]['symbol_mappings'] = unit['symbol_mappings']

with open(f'objdiff.json', 'w') as f:
    f.write(json.dumps(objdiff_json, indent=2))
