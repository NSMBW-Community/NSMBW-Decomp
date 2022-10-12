# Contributing

## Code Guidelines

### General

- Lines should not exceed `100` characters, these can be split into multiple lines.
- Use `nullptr` instead of `0` when assigning / comparing a pointer (unless strictly necessary).
- Use the `.cpp`/`.hpp` extension for C++ files, the `.c`/`.h` extension for C files and the `.s` extension for ASM files.
- The preferred indentation style is [1TBS/OTBS](https://en.wikipedia.org/wiki/Indent_style#Variant:_1TBS_(OTBS)).

### Nonmatching Code

- If your code does **not** match, use the `NONMATCHING` macro, and explain in a comment why it does not match.

### Headers

- Use forward-declared types when possible.
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

- Names for known symbols should match **exactly**, including typos.
- Member variables must be prefixed with `m` (and `p` if they're a pointer).
- Static member variables must be prefixed with `s` (and `p` if they're a pointer).
- The above rule can be ignored if the existing symbol differs.
- Functions with no known symbol must use **camelCase**.

### Types

- Pointer/reference types must have the asterisk/ampersand near the variable name.
- Use C style casts to cast a type to another.

### Constants

- Constants with a heavy impact on the game must be declared and used properly. If such a constant appears in more than one compilation unit, it must be placed in `constants/game_constants.h`. Else, place it in the correct header file.
- Japanese text strings **must** be placed in `constants/sjis_constants.h`.

### Comments

- Code comments should begin with an uppercase letter and usually not end in a period.
- Place comments that could not have been in the original code between square brackets. Examples include:
   - `// Parameters that can be set to configure the behaviour of the base. [These are the sprite data fields in Reggie].`
   - `// [TODO: is this an int or an enum?]`

### Classes

- When referencing a class member, do **not** use `this->` unless required for compilation.
- Class members must be placed in the following order:
  - Nested Classes/Structures/Enumerations
  - Functions
  - Variables (place static ones last)
  - Friends

- Functions for classes must be placed in the following order:
  - Constructor
  - Destructor
  - Operators
  - Virtual Functions
  - Member Functions (place static ones last)

- Set appropriate access modifiers for each member. Within each category listed above, place the entries in the following order:
  - Public
  - Protected
  - Private

- If the virtual functions do not follow the ordering conventions, then those rules can be ignored.

## Documentation Guidelines

Doxygen is being used for generating documentation:
- Use `@` to begin Doxygen commands.
- In general, always start a documentation comment with an uppercase letter and terminate it with a period. Try to refer to variables/arguments using articles and try to use the third person when documenting functions. See below for examples.
- For functions which require a decently long explanation, and/or documentation for the parameters and return values, use multiline comments, like this:
  ```cpp
  /**
   * @brief Instantiates a base under a parent base.
   *
   * @param profName The profile name.
   * @param parent The base's parent.
   * @param param The base's parameters.
   * @param groupType The base's group type.
   * @return A pointer to the instantiated base.
   */
  ```

- If the functionality is rather obvious, please still write a short comment. You can use single-line comments:
  ```cpp
  /// @brief Requests deletion of the base.
  void deleteRequest();
  ```

  Or, if it looks better, you may also use inline comments:
  ```cpp
  virtual int create(); ///< Creates the base.
  virtual int preCreate(); ///< Code to be executed before ::create.
  virtual void postCreate(MAIN_STATE_e state); ///< Code to be executed after ::create.
  ```

- If no official symbol has been cracked for a class or a function, report this information using the `@note` command. For unknown class names, adding the note to each member function is not necessary.
- Do **not** document inline getters/setters unless their logic is complex, document the corresponding variables instead.
- Documenting not yet decompiled code is not necessary.
