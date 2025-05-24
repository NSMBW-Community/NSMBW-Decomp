#!/usr/bin/env python3
# Creates a decomp context for decomp.me

import re
from pathlib import Path
from typing import Optional

from project_settings import *

INCLUDE_PATTERN = re.compile(r'#include\s+["<](.*)[">]\n')
PRAGMA_ONCE_PATTERN = re.compile(r'#pragma\s+once\n')

def expand_header(file_data: str, visited: Optional[set[Path]] = None) -> str:
    if visited is None:
        visited = set()

    def replacer(match):
        header_file: Path = INCDIR / match.group(1)
        if header_file in visited:
            return ''

        visited.add(header_file)
        header_data = header_file.read_text(errors='ignore')
        return expand_header(header_data, visited).strip()

    return INCLUDE_PATTERN.sub(replacer, file_data)


def create_decomp_context(target_file: Path, out_path: Path):
    code_file = target_file.read_text(errors='ignore')

    # Only keep include directives because we don't want the source code in the result
    include_pattern = re.compile(r'#include\s+["<].*[">]')
    headers = '\n'.join(include_pattern.findall(code_file))

    # Remove #pragma once directives otherwise compilation will fail
    data = PRAGMA_ONCE_PATTERN.sub('', expand_header(headers)).strip()
    out_path.write_text(data)


if __name__ == '__main__':
    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Creates a decomp.me context file.')
    parser.add_argument('cpp_file', type=Path, help='C++ file to create the context for.')
    parser.add_argument('-o', '--output', type=Path, required=True, help='Path to store the context in.')
    args = parser.parse_args()
    create_decomp_context(args.cpp_file, args.output)
