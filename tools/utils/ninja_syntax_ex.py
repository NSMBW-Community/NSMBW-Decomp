#!/usr/bin/env python3

# ninja_syntax_ex.py
# ninja_syntax extension for extra flexibility.

from collections.abc import Iterable
from io import StringIO

def escape_path(word: str) -> str:
    return word.replace('$ ', '$$ ').replace(' ', '$ ').replace(':', '$:')

def is_iterable(value) -> bool:
    return isinstance(value, Iterable) and not isinstance(value, str)

def as_list(value) -> list:
    if input is None:
        return []
    if is_iterable(value):
        return list(value)
    return [value]

class Writer():
    def __init__(self):
        self.output = StringIO()

    def newline(self, count: int = 1):
        if count:
            self.output.write('\n' * count)

    def indent(self, indent: int = 0):
        if indent:
            self.output.write(' ' * indent)

    def comment(self, text: str):
        self.output.write('# ')
        self.output.write(text)
        self.newline()

    def variable(self, key, value, indent: int = 0):
        if value is None:
            return

        if is_iterable(value):
            sep = f' $\n{" " * (indent + 2)}'
            value = sep.join(filter(None, value))  # Filter out empty strings

        self.indent(indent)
        self.output.write(f'{key} = {value}')
        self.newline()

    def rule(self, rule, command, **kwargs):
        if rule is None:
            return

        if is_iterable(command):
            command = ' && '.join(command)

        self.output.write(f'rule {rule}')
        self.newline()
        self.variable('command', command, 2)
        for key, value in kwargs.items():
            self.variable(key, value, 2)
        self.newline()

    def build(self, rule, outputs, inputs=None, implicit_inputs=None, implicit_outputs=None,
              order_only_inputs=None, **kwargs):

        if rule is None or not outputs:
            return

        all_inputs = [escape_path(str(x)) for x in as_list(inputs)]
        all_outputs = [escape_path(str(x)) for x in as_list(outputs)]

        if implicit_inputs:
            implicit_inputs = [escape_path(str(x)) for x in as_list(implicit_inputs)]
            all_inputs.append('|')
            all_inputs.extend(implicit_inputs)

        if order_only_inputs:
            order_only_inputs = [escape_path(str(x)) for x in as_list(order_only_inputs)]
            all_inputs.append('||')
            all_inputs.extend(order_only_inputs)

        if implicit_outputs:
            implicit_outputs = [escape_path(str(x)) for x in as_list(implicit_outputs)]
            all_outputs.append('|')
            all_outputs.extend(implicit_outputs)

        self.output.write('build')
        for out in all_outputs:
            self.output.write(' ')
            self.output.write(out)

        self.output.write(f': {rule}')
        for src in all_inputs:
            self.output.write(' ')
            self.output.write(src)

        self.newline()
        for key, value in kwargs.items():
            self.variable(key, value, 2)
        self.newline()

    def pool(self, name, depth: int = 1, indent: int = 0):
        self.output.write(f'pool {name}\n')
        self.newline()
        self.variable('depth', depth, indent)

    def flush(self, path):
        with open(path, 'w', encoding='utf-8') as f:
            f.write(self.output.getvalue())
