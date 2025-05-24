#!/usr/bin/env python3

# json_parser.py
# Basic JSON to dataclass parser

from dataclasses import is_dataclass, fields, MISSING
from typing import cast, get_origin, get_args, TypeVar
from enum import Enum

T = TypeVar('T')
def from_json(cls: type[T], data: dict) -> T:

    # Handle lists
    if isinstance(data, list):
        return cast(T, [from_json(get_args(cls)[0], item) for item in data])

    # Handle base types
    if not is_dataclass(cls):
        if isinstance(cls, type) and issubclass(cls, Enum):
            return cls[data] if isinstance(data, str) else cls(data)
        return cast(T, data)

    # Handle nested dataclasses
    kwargs = {}
    for field_ in fields(cls):

        # Skip non-init fields
        if not field_.init:
            continue

        # Get base data
        key = field_.name
        field_type = field_.type
        origin = get_origin(field_type)
        value = data.get(key, None)

        # Handle missing data
        if value is None:
            if field_.default != MISSING:
                kwargs[key] = field_.default
            elif field_.default_factory != MISSING:
                kwargs[key] = field_.default_factory()
            elif (field_type):
                assert isinstance(field_type, type)
                kwargs[key] = from_json(field_type, {})
            else:
                kwargs[key] = None

        # Handle lists
        elif origin is list:
            inner_type = get_args(field_type)[0]
            kwargs[key] = [from_json(inner_type, item) for item in value]

        # Handle dicts
        elif origin is dict:
            _, val_type = get_args(field_type)
            kwargs[key] = {k: from_json(val_type, v) for k, v in value.items()}

        # Handle nested dataclasses
        elif is_dataclass(field_type):
            assert isinstance(field_type, type)
            kwargs[key] = from_json(field_type, value)

        # Handle enums
        elif isinstance(field_type, type) and issubclass(field_type, Enum):
            kwargs[key] = field_type[value] if isinstance(value, str) else field_type(value)

        # Handle base data types
        else:
            kwargs[key] = value

    # Create class
    return cls(**kwargs)
