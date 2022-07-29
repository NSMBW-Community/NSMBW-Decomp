# Contributing

## Guidelines

### General

- Lines should not exceed `100` characters, these can be split into multiple lines.
- Use `nullptr` instead of `0` when assigning / comparing a pointer.
- Use the `.cpp`/`.hpp` extension for C++ files, the `.c`/`.h` extension for C files and the `.s` extension for ASM files
- The preferred indentation style is 1TBS/OTBS.

### Headers

- Use forward-declared types when possible
- At the top of every header place:

    ```c++
    #pragma once
    ```

### Includes

- For includes use:

    ```c++
    #include <...>
    ```

    (These are relative to the `include` folder.)

### Names

- Names for known symbols should match **exactly**, even including typos in the symbol.
- Member variables must be prefixed with `m`.
- Arguments for functions must be prefixed with `p` for pointers.
- Functions with no symbols must use **camelCase**.

### Classes

- When referencing a class member, do **not** use `this->`, unless it is required for compilation.
- Functions for classes must be put in the following order:  
  - Constructor  
  - Destructor  
  - Operators  
  - Virtual Functions  
  - Member Functions

  If the virtual functions are not in the order that they are in the vtable, then the rule above can be ignored as these functions must be placed in order.

### Nonmatching Code

If your code does **not** match, use the `NONMATCHING` macro, and explain in a comment why it does not match.

### Types

- Pointer/reference types must have the asterisk/ampersand near the variable name.
- Use C style casts to cast a type to another.
