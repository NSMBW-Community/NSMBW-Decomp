#!/usr/bin/env python3
# Setup project for use with objdiff
# Uses dtk to split the dol and rel files and creates objdiff.json

import argparse
from collections import defaultdict
import json
import os
from pathlib import Path
import platform
import re
import shutil
import stat
import subprocess
import sys
from typing import cast
import urllib.request
import yaml

sys.path.append('tools')

from color_term import *
from project_settings import *
from slicelib import SliceFile, SliceType, load_slice_file
from elffile import ElfFile, ElfSymtab
from elfconsts import STB, STT

parser = argparse.ArgumentParser(description='Sets up the project for use with objdiff.')
parser.add_argument('--skip-download', action='store_true', help='Skip downloading symbols file')
args = parser.parse_args()

def text_to_syms(original_file: str):
    orig_syms = dict()
    count = defaultdict(int)
    for line in original_file.splitlines():
        m = re.match(r'(.+) = (.+):(.+); // (.+)', line)
        assert m
        sym, sec, addr, comment = m.group(1), m.group(2), int(m.group(3), 0), m.group(4)
        count[sym] += 1
        comment_dict = dict(part.split(':', 1) for part in comment.split())
        if re.match(r'^\@\d+$', sym):
            comment_dict['scope'] = 'local'
        orig_syms[(sec, addr)] = (sym, comment_dict)
    for _, (sym, comment_dict) in orig_syms.items():
        if count[sym] > 1:
            comment_dict['scope'] = 'local'
    return orig_syms

def syms_to_text(syms: dict):
    res = ''
    keys = list(syms.keys())
    for sec, addr in keys:
        sym, comment = syms[(sec, addr)]
        comment_str = ' '.join(f'{k}:{v}' for k, v in comment.items())
        res += f'{sym} = {sec}:0x{addr:08X}; // {comment_str}\n'
    return res


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

slices: list[SliceFile] = []

for file in SLICEDIR.glob('*.json'):
    slices.append(load_slice_file(file))

# Ensure correct order of slices
slices = sorted(slices, key=lambda x: x.meta.moduleNum)

config_yaml = {
    'modules': [],
    'write_asm': False
}

symbols_url = 'https://github.com/RootCubed/NSMBW-Maps/releases/download/v1.0/wiimj2d_symbols.txt'
symbols_path = BUILDDIR / 'dtk/wiimj2d_symbols.txt'
if not symbols_path.exists() or not args.skip_download:
    symbols_path.parent.mkdir(parents=True, exist_ok=True)
    print(f'Downloading symbols from {symbols_url}')
    req = urllib.request.Request(symbols_url, headers={'User-Agent': 'Mozilla/5.0'})
    with urllib.request.urlopen(req) as response:
        symbols_path.write_text(response.read().decode('utf-8'))
config_yaml['symbols'] = symbols_path.as_posix()

SECTION_TYPES = {
    '.init': 'code',
    'extab': 'data',
    'extabindex': 'data',
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
        config_yaml['object'] = f'original/{slice_file.meta.fileName}'
        config_yaml['splits'] = f'{BUILDDIR}/dtk/dtk_splits_{Path(slice_file.meta.fileName).stem}.txt'
        config_yaml['mw_comment_version'] = 14
        config_yaml['block_relocations'] = [
            {
                'source': '.data:0x803463E0',
                'end': '.data:0x8034649C'
            }
        ]
    else:
        config_yaml['modules'].append({
            'object': f'original/{slice_file.meta.fileName}',
            'splits': f'{BUILDDIR}/dtk/dtk_splits_{Path(slice_file.meta.fileName).stem}.txt',
            'symbols': f'{BUILDDIR}/dtk/{Path(slice_file.meta.fileName).stem}_symbols.txt',
            'mw_comment_version': 14
        })

    splits_file = 'Sections:\n'

    sec_origin = {}
    for secname, info in slice_file.meta.sections.items():
        if info.size == 0:
            continue
        if not '$' in secname:
            align = info.align if info.secAlign == -1 else info.secAlign
            if slice_file.meta.type == SliceType.DOL:
                align = 32
            splits_file += f'\t{secname: <15} type:{SECTION_TYPES[secname.split("$")[0]]} align:{align}\n'
        sec_origin[secname] = info.addr

    splits_file += '\n'

    for slice in slice_file.parsed_slices:
        if slice.sliceName.startswith('filler_'):
            continue

        name = slice.source if slice.source else slice.sliceName
        splits_file += f'{name}:\n'
        for slice_sec in slice.sliceSecs:
            start = slice_sec.start_offs + sec_origin[slice_sec.sec_name]
            end = slice_sec.end_offs + sec_origin[slice_sec.sec_name]

            if '__init_cpp_exceptions' in name and slice_sec.sec_name in ['.ctors$10', '.dtors']:
                if slice_sec.sec_name == '.ctors$10':
                    splits_file += f'\t{'.ctors': <15} start:0x{start:08X} end:0x{start+4:08X} rename:.ctors$10\n'
                elif slice_sec.sec_name == '.dtors':
                    splits_file += f'\t{slice_sec.sec_name: <15} start:0x{start:08X} end:0x{start+4:08X} rename:.dtors$10\n'
                    splits_file += f'\t{slice_sec.sec_name: <15} start:0x{start+4:08X} end:0x{start+8:08X} rename:.dtors$15\n'
            else:
                splits_file += f'\t{slice_sec.sec_name: <15} start:0x{start:08X} end:0x{end:08X}\n'
        splits_file += '\n'

    with open(f'{BUILDDIR}/dtk/dtk_splits_{Path(slice_file.meta.fileName).stem}.txt', 'w') as f:
        f.write(splits_file)


with open(f'{BUILDDIR}/dtk/config.yaml', 'w') as f:
    f.write(yaml.dump(config_yaml))

# Clear out symbols to start
for i, m in enumerate(config_yaml['modules']):
    Path(m['symbols']).write_text('')

# Step 2: Generate splits with dtk

dtk_path = get_dtk('v0.9.6')
cmd = [dtk_path, 'dol', 'split', f'{BUILDDIR}/dtk/config.yaml', f'{BUILDDIR}/dtkspl']
out = subprocess.run(cmd)
if out.returncode != 0:
    sys.exit()

old_syms = text_to_syms(symbols_path.read_text())

cmd = [dtk_path, 'dol', 'apply', f'{BUILDDIR}/dtk/config.yaml', f'{BUILDDIR}/wiimj2d.elf']
out = subprocess.run(cmd)
if out.returncode != 0:
    sys.exit()

new_syms = text_to_syms(symbols_path.read_text())

for sec, addr in old_syms:
    o_sym, o_attrs = old_syms[(sec, addr)]
    n_sym, n_attrs = new_syms[(sec, addr)]
    if 'size' in o_attrs and o_attrs['size'] != 0:
        if 'size' not in n_attrs or n_attrs['size'] != o_attrs['size']:
            n_attrs['size'] = o_attrs['size']
    if re.match(r'\.\.\..+', n_sym) and n_sym != o_sym:
        new_syms[(sec, addr)] = (o_sym, o_attrs)

symbols_path.write_text(syms_to_text(new_syms))

module_syms = [dict()]
for m in config_yaml['modules']:
    module_syms.append(text_to_syms(Path(m['symbols']).read_text()))

module_sections = [
    list(slice.meta.sections.keys()) for slice in slices
]

# Apply REL symbols from compiled sources
for i, m in enumerate(config_yaml['modules']):
    modpath = BUILDDIR / (Path(m['object']).stem + '.plf')
    sym_dict = module_syms[i + 1]
    main_elf = ElfFile.read(modpath.read_bytes())
    symtab = cast(ElfSymtab, main_elf.get_section('.symtab'))
    for sym in symtab.syms:
        if sym.st_shndx < len(main_elf.sections):
            sec = module_sections[i + 1][sym.st_shndx]
            if sec == '':
                continue
            addr = sym.st_value
            name = sym.name
            if name == '' or name == sec.replace('.', '_') or name.startswith('...' + sec[1:]):
                continue
            orig_comment = {} if (sec, addr) not in sym_dict else sym_dict[(sec, addr)][1]
            orig_comment['type'] = 'function' if sym.st_info_type == STT.STT_FUNC else 'object'
            orig_comment['scope'] = 'local' if sym.st_info_bind == STB.STB_LOCAL else 'global'
            # DTK sometimes incorrectly detects an unnecessary blr at the end as a separate function
            if 'size' in orig_comment and sym.st_size > int(orig_comment['size'], 0):
                if (sec, addr + sym.st_size - 4) in sym_dict:
                    del sym_dict[(sec, addr + sym.st_size - 4)]
            orig_comment['size'] = sym.st_size
            sym_dict[(sec, addr)] = (name, orig_comment)
            if sym.name == '__register_global_object':
                del sym_dict[(sec, addr)]
for i, m in enumerate(config_yaml['modules']):
    Path(m['symbols']).write_text(syms_to_text(module_syms[i + 1]))

cmd = [dtk_path, 'dol', 'split', f'{BUILDDIR}/dtk/config.yaml', f'{BUILDDIR}/dtkspl']
out = subprocess.run(cmd)
if out.returncode != 0:
    sys.exit()

print_success('Successfully generated splits with dtk. Now generating objdiff.json...')

# Step 3: Generate objdiff.json

with open(f'{BUILDDIR}/dtkspl/config.json', 'r') as f:
    objdiff_json = json.load(f)

objdiff_json['min_version'] = '2.0.0'
objdiff_json['custom_make'] = 'ninja'
objdiff_json['watch_patterns'] = [
    '*.c', '*.cpp',
    '*.h', '*.hpp',
    '*.s', '*.S',
    '*.py',
    '*.json'
]
objdiff_json['progress_categories'] = [
    {
        'id': 'dol',
        'name': 'DOL'
    },
    {
        'id': 'modules',
        'name': 'Modules'
    }
]
del objdiff_json['ldscript']
del objdiff_json['module_id']
del objdiff_json['entry']
del objdiff_json['version']

for slice_file in slices:
    for slice in slice_file.parsed_slices:
        stem = Path(slice_file.meta.fileName).stem
        if slice_file.meta.type == SliceType.DOL:
            loc = objdiff_json['units']
        else:
            loc = objdiff_json['modules']
            loc = next(x for x in loc if x['name'] == stem)['units']

        unit = next((u for u in loc if u['name'] == slice.source), None)
        if unit is None:
            if slice.source != '':
                print_err(f'Unit {slice.sliceName} not found in objdiff.json')
            continue

        base_path = BUILDDIR / 'compiled' / stem / slice.sliceName
        unit['base_path'] = base_path.as_posix()
        unit['metadata'] = {
            'complete': slice.source is not None,
            'source_path': f'source/{slice.source}'
        }

        # decomp.me scratch options
        ctx_path = (BUILDDIR / 'decompctx' / stem / slice.sliceName).with_suffix('.hpp')
        c_flags = slice_file.meta.defaultCompilerFlags
        if slice.ccFlags:
            c_flags = slice.ccFlags
        unit['scratch'] = {
            'platform': 'gc_wii',
            'compiler': 'mwcc_43_151',
            'c_flags': c_flags + ' -lang=c++ -DREVOLUTION',
            'ctx_path': ctx_path.as_posix(),
            'build_ctx': True
        }


def patch_unit(unit, progress_categories, module_name):
    if not 'metadata' in unit:
        unit['metadata'] = {}
    unit['metadata']['auto_generated'] = unit['autogenerated']
    del unit['autogenerated']
    unit['metadata']['progress_categories'] = progress_categories
    obj_path = Path(unit['object'])
    unit['target_path'] = obj_path.as_posix()
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
