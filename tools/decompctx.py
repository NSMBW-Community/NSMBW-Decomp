#!/usr/bin/env python3
# Creates a decomp context for decomp.me

from pathlib import Path
import re

from project_settings import *
from slicelib import *

def create_decomp_context(target_file: Path, out_path: Path):
    context_headers = []
    header_files = dict()

    include_regex = re.compile(r'#include\s+["<](.*)[">]')
    pragma_once_regex = re.compile(r'#pragma\s+once')

    c_file = target_file.read_text(errors='ignore')
    context_headers = include_regex.findall(c_file)
    # Expand headers until we no expansions are left
    while True:
        no_changes = True
        for i in range(len(context_headers)):
            h_name = context_headers[i]
            if h_name in header_files:
                continue

            h_file = (INCDIR / h_name).read_text(errors='ignore')
            header_files[h_name] = pragma_once_regex.sub('', include_regex.sub('', h_file))
            referenced_h_files = [x for x in include_regex.findall(h_file) if x not in context_headers[:i]]
            context_headers = \
                context_headers[:i] + \
                referenced_h_files + \
                context_headers[i:]
            no_changes = False

            # Break since indices have changed and we need to start over
            break

        if no_changes:
            break

    # Only keep first instance of each header
    deduped_headers = []
    for h_name in context_headers:
        if h_name not in deduped_headers:
            deduped_headers.append(h_name)

    out_path.write_text('\n'.join([header_files[x] for x in deduped_headers]))

if __name__ == '__main__':
    # Parse arguments separately so this file can be imported from other ones
    import argparse
    parser = argparse.ArgumentParser(description='Creates a decomp.me context file.')
    parser.add_argument('cpp_file', type=Path, help='C++ file to create the context for.')
    parser.add_argument('-o', '--output', type=Path, help='Path to store the context in.')
    args = parser.parse_args()
    create_decomp_context(args.cpp_file, args.output)
