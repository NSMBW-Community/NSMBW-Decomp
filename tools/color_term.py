#!/usr/bin/env python3
# Colored print utility

def print_warn(*args, end='\n', **kwargs) -> None:
    print('\033[33m', end='', **kwargs)
    print(*args, end=end+'\033[39m', **kwargs)

def print_err(*args, end='\n', **kwargs) -> None:
    print('\033[31m', end='', **kwargs)
    print(*args, end=end+'\033[39m', **kwargs)

def print_success(*args, end='\n', **kwargs) -> None:
    print('\033[32m', end='', **kwargs)
    print(*args, end=end+'\033[39m', **kwargs)
