#!/usr/bin/env python3
# Build verification & Progress checker script

import argparse
import hashlib
import math
import subprocess
import sys
from pathlib import Path

sys.path.append('tools')

from color_term import *
from project_settings import *
from slicelib import SliceFile, load_slice_file
from elffile import ElfFile

parser = argparse.ArgumentParser(description='Tool to verify built binaries and generate progress files for the website.\
    Terminates with a non-zero exit code if an error was encountered.')

parser.add_argument('--verify-bin', help='Verifies that the output binaries match the original files.', action='store_true')
parser.add_argument('--verify-obj', help='Verifies that the compiled object file sections are of the expected length.', action='store_true')
parser.add_argument('--progress-summary', help='Prints out a summary of the project\'s progress.', action='store_true')
parser.add_argument('--progress-csv', nargs='?', default=False, const=True, type=Path,
    help='Outputs a comma-separated string of progress information, used for the website.\
        Specify a path to load the previous progress file, the new progress string is only written back to it if the progress amount has changed.')
parser.add_argument('--create-badges', help='Creates progress badge data. Outputs to badge_<name>.json', action='store_true')
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

def get_git_commit_list() -> list[dict[str, any]]:
    out = subprocess.check_output(['git', '--no-pager', 'log', '--format=%H %at']).decode().strip().split('\n')
    return list(map(lambda x: { 'timestamp': int(x.split(' ')[1]), 'hash': x.split(' ')[0] }, out))

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
                    elf = ElfFile.read(open(compiled_path, 'rb').read())
                    for slice_sec in slice.slice_secs:
                        sec_name = slice_sec.sec_name
                        elf_sec = elf.get_section(sec_name)
                        if elf_sec is None:
                            # Check for $ section
                            was_found = False
                            for elf_sec in elf.sections:
                                if elf_sec.name.split('$')[0] == sec_name:
                                    was_found = True
                                    break
                            if not was_found:
                                errors.append(f'Section {sec_name} not found!')
                        else:
                            al_start = align_addr(slice_sec.start_offs, slice_sec.alignment)
                            al_end = align_addr(slice_sec.end_offs, slice_sec.alignment)
                            exp_len = al_end - al_start
                            obj_len = align_addr(elf_sec.size(), slice_sec.alignment)
                            if obj_len != exp_len:
                                add_note = ''
                                # if slice.deadstrip or slice.no_deadstrip:
                                #     add_note = '\n  This is to be expected with classes that have deadstripped symbols.'
                                warnings.append(f'Length of section {sec_name} not matching (expected {exp_len}, got {obj_len}){add_note}')
                    for elf_sec in elf.sections:
                        if elf_sec.name not in slice_file.meta.secs or slice_file.meta.secs[elf_sec.name].size == 0:
                            continue
                        if elf_sec.name not in [i.sec_name for i in slice.slice_secs]:
                            errors.append(f'Section {elf_sec.name} not included in slice file!')

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


slicefile_names = ['wiimj2d.dol', 'd_profileNP.rel', 'd_basesNP.rel', 'd_enemiesNP.rel', 'd_en_bossNP.rel']
code_sec_names = ['.init', '.text']

def read_progress_csv_line(line: str) -> dict[str, any]:
    splitted = line.split(',')
    return {
        'timestamp': int(splitted[0]),
        'hash': splitted[1],
        'progress_vals': [int(x) for x in splitted[2:]]
    }

def get_historical_progress_data(commits, last_tracked) -> list[dict[str, any]]:
    # Get branch name for reverting back to it at the end
    branch_name = subprocess.check_output(['git', 'rev-parse', '--abbrev-ref', 'HEAD']).decode().strip().split('\n')[-1]

    new_progress_lines = []
    last = last_tracked
    for commit in reversed(commits): # skip HEAD too
        print_warn(f'Rolling back to {commit["hash"]}.')
        try:
            subprocess.check_output(['git', 'checkout', commit['hash']])
            line = subprocess.check_output(['python', 'progress.py', '--progress-csv']).decode().strip().split('\n')[-1]
            data = read_progress_csv_line(line)
            if last['progress_vals'] != data['progress_vals']:
                new_progress_lines.append(data)
            last = data
        except subprocess.CalledProcessError as e:
            print_err('Rolling back failed. Skipping rollback step.')
            break

    # Go back to where we were before
    try:
        subprocess.check_output(['git', 'checkout', branch_name])
    except subprocess.CalledProcessError as e:
        print_err('Could not revert to current commit.')
    return new_progress_lines

def progress_csv(slice_files: list[SliceFile]) -> bool:
    # Grab data
    commits = get_git_commit_list()
    hash = commits[0]['hash']
    timestamp = commits[0]['timestamp']

    progress_list = []
    for slicefile_name in slicefile_names:
        slice_file = next((i for i in slice_files if i.meta.name == slicefile_name))
        progress_list.extend(calculate_decompiled_bytes(slice_file, code_sec_names))

    latest_csv = [timestamp, hash, *progress_list]
    latest_commit_csv_str = ','.join([str(i) for i in latest_csv])

    global args
    if vars(args)['progress_csv'] != True:
        # File was supplied, write back to it if progress changed
        last_line_data = None
        with open(vars(args)['progress_csv'], 'r') as f:
            last_line_data = read_progress_csv_line(f.readlines()[-1].strip())

        if progress_list != last_line_data['progress_vals']:
            print_success('Progress detected! Going through previous commits and writing to file.')

            # Trim the list of commits to those which came after the last line in the progress csv
            idx_last = next((i for i, x in enumerate(commits) if x['hash'] == last_line_data['hash']), None)

            # Unfortunate hardcode; first revision where progress script implements correct functionality in all following revisions
            first_good_revision = '29b1a9bf5a11217cb2f60b74c3b4358290470209'
            idx_first_good = next((i for i, x in enumerate(commits) if x['hash'] == first_good_revision), None)

            commits_to_track = commits[1:min(idx_last, idx_first_good)] # Skip HEAD too

            # In case of a rebase, we might have "new" commits that were made before the last tracked commit,
            # so we need to ignore those.
            commits_to_track = [x for x in commits_to_track if x['timestamp'] > last_line_data['timestamp']]

            data = get_historical_progress_data(commits_to_track, last_line_data)
            if len(data) == 0 or latest_csv[2:] != data[-1]['progress_vals']:
                data.append({
                    'timestamp': int(latest_csv[0]),
                    'hash': latest_csv[1],
                    'progress_vals': [int(x) for x in latest_csv[2:]]
                })

            with open(vars(args)['progress_csv'], 'a') as f:
                for line in data:
                    csv = [line['timestamp'], line['hash'], *line['progress_vals']]
                    csv_str = ','.join([str(i) for i in csv])
                    f.write(csv_str + '\n')
        else:
            print('No change in progress detected, not writing to progress file.')

    # Always output to console
    print(latest_commit_csv_str)

    return True

def create_badges(slice_files: list[SliceFile]):
    for slicefile_name in slicefile_names:
        slice_file = next((i for i in slice_files if i.meta.name == slicefile_name))
        compiled, total = calculate_decompiled_bytes(slice_file, code_sec_names)
        perc = str(round(compiled / total * 100, 3))
        slice_stem = slicefile_name.split(".")[0]

        # Interpolate between #93a80b and #29e419
        color = [str(round(x)) for x in color_lerp((147, 168, 11), (41, 228, 25), compiled / total)]

        with open(f'badge_{slice_stem}.json', 'w') as f:
            f.write(f'{{ "schemaVersion": 1, "label": "{slice_stem}", "message": "{perc}%", "color": "rgb({",".join(color)})" }}')
            print(f'Wrote badge_{slice_stem}.json')

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
