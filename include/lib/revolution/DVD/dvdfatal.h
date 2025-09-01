#ifndef RVL_SDK_DVD_FATAL_H
#define RVL_SDK_DVD_FATAL_H
#include <types.h>
#ifdef __cplusplus
extern "C" {
#endif

void __DVDShowFatalMessage(void);
BOOL DVDSetAutoFatalMessaging(BOOL enable);
BOOL __DVDGetAutoFatalMessaging(void);
void __DVDPrintFatalMessage(void);

#ifdef __cplusplus
}
#endif
#endif
