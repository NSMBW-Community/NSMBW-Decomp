#!/usr/bin/env python3
# Colored print utility

def print_warn(*args, **kwargs) -> None:
    print('\033[33m', **(kwargs | {'end': ''}))
    print(*args, '\033[39m', **kwargs)

def print_err(*args, **kwargs) -> None:
    print('\033[31m', **(kwargs | {'end': ''}))
    print(*args, '\033[39m', **kwargs)

def print_success(*args, **kwargs) -> None:
    print('\033[32m', **(kwargs | {'end': ''}))
    print(*args, '\033[39m', **kwargs)
