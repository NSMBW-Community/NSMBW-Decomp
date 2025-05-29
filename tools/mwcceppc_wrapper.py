#!/usr/bin/env python3

# mwcceppc_wrapper.py
# A modified version of RoadrunnerWMC's wrapper script with Wibo support and no winepath usage.

import shutil
import subprocess
import sys
from pathlib import Path, PureWindowsPath

PROJECT_DIR = Path(__file__).parents[1]

def windows_to_unix(path: str) -> str:
    win_path = PureWindowsPath(path.strip())
    return win_path.as_posix().lstrip(win_path.anchor)


def fix_makefile(text: str) -> str:
    """
    Converts all Windows paths to Unix paths within a CodeWarrior-generated Makefile.
    """
    split_pattern = ' \\\n\t'
    compiled_path, source_paths = text.split(': ')
    source_paths = map(windows_to_unix, source_paths.split(split_pattern))
    return f'{windows_to_unix(compiled_path)}: {split_pattern.join(source_paths)}'


def main():
    argv = sys.argv
    if len(argv) < 2:
        raise SystemExit(f'usage: {argv[0]} /path/to/mwcceppc.exe [arguments to CodeWarrior]...')

    # Ignore this Python script's own filename
    argv.pop(0)

    # First argument is the path to CodeWarrior
    cw_exe = argv.pop(0)

    # Obtain the makefile path from the object code path
    makefile_path = None
    for i, arg in enumerate(argv):
        prev_arg = argv[i - 1] if i > 0 else None
        if prev_arg == '-o':
            makefile_path = Path(PROJECT_DIR, arg).with_suffix('.d')
            break

    # Failsafe
    if not makefile_path:
        raise SystemExit('No makefile found!')

    # Check if Wibo is installed and use it if so
    wrapper = 'wibo' if shutil.which('wibo') else 'wine'

    # Invoke CodeWarrior
    proc = subprocess.run([wrapper, cw_exe, *argv])
    if proc.returncode != 0:
        exit(proc.returncode)

    # Fix up the generated makefile
    with open(makefile_path, 'r+', encoding='utf-8') as f:
        data = fix_makefile(f.read())
        f.seek(0)
        f.truncate()
        f.write(data)


if __name__ == '__main__':
    main()
