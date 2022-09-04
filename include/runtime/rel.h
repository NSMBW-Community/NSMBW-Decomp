#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*voidFctPtr) (void);

void ModuleConstructorsX(voidFctPtr *);
void ModuleDestructorsX(voidFctPtr *);
void ModuleUnresolved();

#ifdef __cplusplus
}
#endif

extern void finalizeProlog();
extern void finalizeEpilog();
