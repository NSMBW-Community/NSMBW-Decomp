#!/usr/bin/env python3
# Build verification & Progress checker script

import argparse
import hashlib
import math
import subprocess
import sys
from urllib.request import urlopen
from pathlib import Path

sys.path.append('tools')

from color_term import *
from project_settings import *
from slicelib import SliceFile, load_slice_file
from elffile import ElfFile

parser = argparse.ArgumentParser(description='Tool to verify built binaries and generate progress files for the website. Terminates with a non-zero exit code if an error was encountered.')
parser.add_argument('--verify-bin', help='Verifies that the output binaries match the original files.', action='store_true')
parser.add_argument('--verify-obj', help='Verifies that the compiled object file sections are of the excepted length.', action='store_true')
parser.add_argument('--progress-summary', help='Prints out a summary of the project\'s progress.', action='store_true')
parser.add_argument('--progress-csv', help='Outputs a comma-separated string of progress information, used for the website.', action='store_true')
parser.add_argument('--create-badges', help='Creates progress badge data. Outputs to bin/badge_<name>.json', action='store_true')
args = parser.parse_args()

##########################
# Begin helper functions #
##########################

def print_warnings_and_errors(warnings: list[str], errors: list[str]) -> None:
    num_warnings = len(warnings)
    num_errors = len(errors)
    if num_warnings == 0 and num_errors == 0:
        print_success('OK')
    else:
        if num_warnings > 0:
            print_warn(num_warnings, 'warning' + ('s' if num_warnings != 1 else ''), end='')
        if num_warnings > 0 and num_errors > 0:
            print(', ', end='')
        if num_errors > 0:
            print_err(num_errors, 'error' + ('s' if num_errors != 1 else ''), end='')

        print()
        for warning in warnings:
            print_warn('  ' + warning)
        for error in errors:
            print_err('  ' + error)

def print_banner(title: str) -> None:
    print('+' + '-' * (len(title) + 4) + '+')
    print('|' + '  ' + title + '  ' + '|')
    print('+' + '-' * (len(title) + 4) + '+')

def color_lerp(col_a: tuple[int, int, int], col_b: tuple[int, int, int], frac: float):
    return [a + (b - a) * frac for a, b in zip(col_a, col_b)]

def get_git_revision_hash() -> dict[str, str]:
    return subprocess.check_output(['git', 'rev-parse', 'HEAD']).decode('ascii').strip()
def get_git_revision_timestamp() -> int:
    return int(subprocess.check_output(['git', 'log', '-1', '--format=%ct']).decode('ascii').strip())

def calculate_decompiled_bytes(slice_file: SliceFile, filter_sections: list[str]) -> tuple[int, int]:
    count_compiled_bytes = 0
    count_total_bytes = 0
    for slice in slice_file.slices:
        for slice_sec in [i for i in slice.slice_secs if i.sec_name in filter_sections]:
            byte_count = slice_sec.end_offs - slice_sec.start_offs

            count_total_bytes += byte_count
            if slice.slice_src:
                count_compiled_bytes += byte_count
        
    return [count_compiled_bytes, count_total_bytes]

########################
# Begin task functions #
########################

# Verifies that all binaries match
def verify_bin(slice_files: list[SliceFile]) -> bool:
    all_matching = True
    for slice_file in slice_files:
        orig_file = Path(f'{ORIGDIR}/{slice_file.meta.name}')
        built_file = Path(f'{BUILDDIR}/{slice_file.meta.name}')

        md5orig = hashlib.md5(open(orig_file, 'rb').read()).hexdigest()
        md5built = hashlib.md5(open(built_file, 'rb').read()).hexdigest()

        print(slice_file.meta.name + ': ', end='')
        if md5orig != md5built:
            all_matching = False
            print_err('Failed')
            print('  md5: ', end='')
            print_err(md5built, end='')
            print(f' (should be {md5orig})')
        else:
            print_success('OK')
            print('  md5: ', end='')
            print_success(md5built)
    return all_matching

# Helper function to align to an address
def align_addr(addr: int, align: int):
    return align * math.ceil(addr / align)

# Verify lengths of section lengths
def verify_obj(slice_files: list[SliceFile]) -> bool:
    no_errors = True
    for slice_file in slice_files:
        for slice in slice_file.slices:
            unit_name = Path(slice_file.meta.name).stem
            print_name = f'{unit_name}/{slice.slice_name}'
            if slice.slice_src:
                warnings = []
                errors = []

                compiled_path = Path(f'{BUILDDIR}/compiled/{unit_name}/{slice.slice_name}')

                if not compiled_path.is_file():
                    errors.append(f'File {compiled_path} not found!')
                else:
                    for slice_sec in slice.slice_secs:
                        elf = ElfFile.read(open(compiled_path, 'rb').read())
                        sec_name = slice_sec.sec_name
                        elf_sec = elf.get_section(sec_name)
                        if elf_sec is None:
                            warnings.append(f'Section {sec_name} not found!')
                        else:
                            al_start = align_addr(slice_sec.start_offs, slice_sec.alignment)
                            al_end = align_addr(slice_sec.end_offs, slice_sec.alignment)
                            exp_len = al_end - al_start
                            obj_len = align_addr(elf_sec.size(), slice_sec.alignment)
                            if obj_len != exp_len:
                                warnings.append(f'Length of section {sec_name} not matching (expected {exp_len}, got {obj_len})')

                # Print result
                print(print_name + ': ', end='')
                print_warnings_and_errors(warnings, errors)
                
                # warnings don't count as errors
                if len(errors) > 0:
                    no_errors = False
    return no_errors

# Outputs progress summary
def progress_summary(slice_files: list[SliceFile]) -> bool:
    count_compiled_bytes_total = 0
    count_total_bytes_total = 0

    for slice_file in slice_files:
        count_compiled_bytes, count_total_bytes = calculate_decompiled_bytes(slice_file, ['.init', '.text'])
                
        perc = (count_compiled_bytes / count_total_bytes) * 100
        print(f'{slice_file.meta.name}: Decompiled {count_compiled_bytes}/{count_total_bytes} code bytes ({perc:.3f}%)')

        count_compiled_bytes_total += count_compiled_bytes
        count_total_bytes_total += count_total_bytes

    perc = (count_compiled_bytes_total / count_total_bytes_total) * 100
    print(f'Total: Decompiled {count_compiled_bytes_total}/{count_total_bytes_total} code bytes ({perc:.3f}%)')
    return True


slicefile_names = ['wiimj2d.dol', 'd_profileNP.rel', 'd_basesNP.rel', 'd_en_bossNP.rel']
code_sec_names = ['.init', '.text']

def progress_csv(slice_files: list[SliceFile]):
    hash = get_git_revision_hash()
    timestamp = get_git_revision_timestamp()

    progress_list = []
    for slicefile_name in slicefile_names:
        slice_file = next((i for i in slice_files if i.meta.name == slicefile_name))
        progress_list.extend(calculate_decompiled_bytes(slice_file, code_sec_names))

    csv = [
        timestamp, hash,
        *progress_list
    ]

    print(','.join([str(i) for i in csv]))

    return True

def create_badges(slice_files: list[SliceFile]):
    for slicefile_name in slicefile_names:
        slice_file = next((i for i in slice_files if i.meta.name == slicefile_name))
        compiled, total = calculate_decompiled_bytes(slice_file, code_sec_names)
        perc = str(round(compiled / total * 100, 3))
        slice_stem = slicefile_name.split(".")[0]

        # Interpolate between #93a80b and #29e419
        color = [str(round(x)) for x in color_lerp((147, 168, 11), (41, 228, 25), compiled / total)]

        with open(f'bin/badge_{slice_stem}.json', 'w') as f:
            f.write(f'{{ "schemaVersion": 1, "label": "{slice_stem}", "message": "{perc}%", "color": "rgb({",".join(color)})" }}')
            print(f'Wrote bin/badge_{slice_stem}.json')
    
    return True

####################
# Begin main logic #
####################

slice_files: list[SliceFile] = []

for file in SLICEDIR.glob('*.json'):
    with open(file) as sf:
        slice_files.append(load_slice_file(sf))

# Ensure correct order of slices
slice_files = sorted(slice_files, key=lambda x: x.meta.mod_num)

tasks = {
    'verify_obj': {
        'banner': 'Object file section length verification',
        'func': verify_obj
    },
    'verify_bin': {
        'banner': 'Hash verification of generated binaries',
        'func': verify_bin
    },
    'progress_csv': {
        'banner': 'CSV creation',
        'func': progress_csv
    },
    'create_badges': {
        'banner': 'Badge creation',
        'func': create_badges
    },
    'progress_summary': {
        'banner': 'Project progress summary',
        'func': progress_summary
    }
}

has_errors = False

had_any = False
for task in tasks:
    if vars(args)[task]:
        had_any = True
        if tasks[task]['banner'] != '':
            print('')
            print_banner(tasks[task]['banner'])
            print('')
        has_errors |= not tasks[task]['func'](slice_files)

if not had_any:
    print_warn('No tasks to run specified, exiting.')

# Exit with non-zero status code if an error occurred
if has_errors:
    exit(-1)
