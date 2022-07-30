#include "dol/rel/rel_init.h"
#include "dol/rel/rel.h"

extern "C" {

typedef void (*voidFctPtr) (void);
__declspec(section ".ctors") extern voidFctPtr _ctors[];
__declspec(section ".dtors") extern voidFctPtr _dtors[];

void _prolog() {
    ModuleConstructorsX(_ctors);
    finalizeProlog();
}

void _epilog() {
    finalizeEpilog();
    ModuleDestructorsX(_dtors);
}

void _unresolved() {
    ModuleUnresolved();
}

}
