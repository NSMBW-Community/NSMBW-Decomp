#include <game/sLib/s_printf.hpp>
#include <revolution/OS/OSError.h>
#include <MSL/cstdio>

vprintfFunc sPrintf::GetVPrintfFunc() {
    return sLib::p_VPrintfFuncPtr;
}

int sPrintf::vprintf(const char *format, va_list *args) {
    return GetVPrintfFunc()(format, args);
}

// Override the default OSReport and OSVReport functions

extern "C" void OSReport(const char *msg, ...) {
    va_list list;
    va_start(list, msg);
    OSVReport(msg, &list);
    va_end(list);
}

extern "C" void OSVReport(const char *msg, va_list *list) {
    sPrintf::vprintf(msg, list);
}
