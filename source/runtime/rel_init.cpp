#include <runtime/rel.h>
/// @file

extern "C" {

__declspec(section ".ctors") extern voidFctPtr _ctors[];
__declspec(section ".dtors") extern voidFctPtr _dtors[];

/// @brief Module initialization function.
/// @details Runs all the module's static initializers.
void _prolog() {
    ModuleConstructorsX(_ctors);
    finalizeProlog();
}

/// @brief Module deinitialization function.
/// @details Runs all the module's static destructors.
void _epilog() {
    finalizeEpilog();
    ModuleDestructorsX(_dtors);
}

/// @brief Fall-back function for failed/incomplete module relocations.
void _unresolved() {
    ModuleUnresolved();
}

} // extern "C"
