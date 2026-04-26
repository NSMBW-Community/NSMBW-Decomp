#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*voidFctPtr) (void);
typedef unsigned int (*u32FctPtr) (void);

void ModuleConstructorsX(voidFctPtr *);
void ModuleDestructorsX(voidFctPtr *);
void ModuleUnresolved();

#ifdef __cplusplus
}
#endif

extern void finalizeProlog();
extern void finalizeEpilog();
