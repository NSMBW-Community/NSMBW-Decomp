#include <runtime/rel.h>
/// @file

extern "C" {

/// @cond
__declspec(section ".ctors") extern voidFctPtr _ctors[];
__declspec(section ".dtors") extern voidFctPtr _dtors[];
/// @endcond

/// @brief Module initialization function.
void _prolog() {
    ModuleConstructorsX(_ctors);
    finalizeProlog();
}

/// @brief Module deinitialization function.
void _epilog() {
    finalizeEpilog();
    ModuleDestructorsX(_dtors);
}

/// @brief Fall-back function for failed/incomplete module relocations.
void _unresolved() {
    ModuleUnresolved();
}

} // extern "C"
