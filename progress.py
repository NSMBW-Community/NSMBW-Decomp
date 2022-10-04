#!/usr/bin/env python3
# Build verification & Progress checker script

import argparse
import hashlib
from math import ceil
import sys
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
parser.add_argument('--progress-csv', help='Appends this commit\'s progress to the progress file for the website.', action='store_true')
parser.add_argument('--create-badges', help='Creates progress badge data.', action='store_true')
args = parser.parse_args()

def print_warnings_and_errors(warnings, errors):
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

def print_banner(title):
    print('┌' + '─' * (len(title) + 4) + '┐')
    print('│' + '  ' + title + '  ' + '│')
    print('└' + '─' * (len(title) + 4) + '┘')

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
    return align * ceil(addr / align)

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
    count_sliced_bytes_total = 0

    for slice_file in slice_files:
        count_compiled_bytes = 0
        count_sliced_bytes = 0
        for slice in slice_file.slices:
            text_section = next((i for i in slice.slice_secs if i.sec_name == '.text'), None)
            init_section = next((i for i in slice.slice_secs if i.sec_name == '.init'), None)
            section_code_bytes = 0
            if text_section:
                section_code_bytes += text_section.end_offs - text_section.start_offs
            if init_section:
                section_code_bytes += init_section.end_offs - init_section.start_offs

            if not slice.slice_src:
                count_sliced_bytes += section_code_bytes
            else:
                count_compiled_bytes += section_code_bytes
                
        perc = (count_compiled_bytes / (count_compiled_bytes + count_sliced_bytes)) * 100
        print(f'{slice_file.meta.name}: Decompiled {count_compiled_bytes}/{count_compiled_bytes + count_sliced_bytes} code bytes ({perc:.3f}%)')

        count_compiled_bytes_total += count_compiled_bytes
        count_sliced_bytes_total += count_sliced_bytes

    sum = count_compiled_bytes_total + count_sliced_bytes_total
    perc = (count_compiled_bytes_total / (sum)) * 100
    print(f'Total: Decompiled {count_compiled_bytes_total}/{sum} code bytes ({perc:.3f}%)')
    return True

def progress_csv():
    return True
def create_badges():
    return True

# Start of main logic

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
        'banner': '',
        'func': progress_csv
    },
    'create_badges': {
        'banner': '',
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
