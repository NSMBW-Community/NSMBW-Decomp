#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void ModuleConstructorsX(void (*[])(void));
void ModuleDestructorsX(void (*[])(void));
void ModuleUnresolved();

#ifdef __cplusplus
}
#endif