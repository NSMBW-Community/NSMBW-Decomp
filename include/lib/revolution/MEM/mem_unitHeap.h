#pragma once
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MEMiUntHeapMBlockHead MEMiUntHeapMBlockHead;
struct MEMiUntHeapMBlockHead {
  MEMiUntHeapMBlockHead* succ;
};

typedef struct MEMiUntHeapHead {
  MEMiUntHeapMBlockHead* free_list;
  u32 unit_size;
} MEMiUntHeapHead;

#ifdef __cplusplus
}
#endif
