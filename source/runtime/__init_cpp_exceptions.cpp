#include <types.h>
#include <runtime/__init_cpp_exceptions.hpp>

extern "C" {

__declspec(section ".init") extern __eti_init_info 	_eti_init_info[];

extern void __destroy_global_chain();
extern int __register_fragment(void *, void *);
extern void __unregister_fragment(int);

static int fragmentID = -2;

void __init_cpp_exceptions(void) {
    if (fragmentID == -2) {
        register char *temp; // r4
        asm {mr temp, r2;};
        void *r2 = temp;

        __eti_init_info *info = _eti_init_info;
        fragmentID = __register_fragment(info, r2);
    }
}

void __fini_cpp_exceptions(void) {
    if (fragmentID != -2) {
        __unregister_fragment(fragmentID);
        fragmentID = -2;
    }
}

} // extern "C"

#pragma section ".ctors$10"
__declspec(section ".ctors$10")
    extern void * const __init_cpp_exceptions_reference = __init_cpp_exceptions;
#pragma section ".dtors$10"
__declspec(section ".dtors$10")
    extern void * const __destroy_global_chain_reference = __destroy_global_chain;
#pragma section ".dtors$15"
__declspec(section ".dtors$15")
    extern void * const __fini_cpp_exceptions_reference = __fini_cpp_exceptions;
