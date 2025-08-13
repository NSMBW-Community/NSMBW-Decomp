#include <types.h>
#include <game/bases/d_main.hpp>
#include <game/bases/d_system.hpp>

extern "C" {
void DVDDiskCheck(); ///< @unofficial
}

OSThread dMain::mainThread;
s64 dMain::g_InitialTime;

void dMain::Create() {
    dSys_c::create();
    dSys_c::setBlack(false);
}

void dMain::Execute() {
    while (true) {
        dSys_c::execute();
    }
}

void *dMain::main01(void *arg) {
    Create();
    Execute();
    return nullptr;
}

extern "C" {

#define STACK_SIZE 0xf000

void main(int argc, char *argv[]) {
    u8 stack[STACK_SIZE] ALIGNED(32);
    DVDDiskCheck();
    dMain::g_InitialTime = OSGetTime();
    dSystem::fixArena();
    OSThread *thread = OSGetCurrentThread();
    int priority = OSGetThreadPriority();
    OSCreateThread(&dMain::mainThread, dMain::main01, nullptr, stack + STACK_SIZE, STACK_SIZE, priority, 0);
    OSResumeThread(&dMain::mainThread);
    OSSetThreadPriority(thread, 0x1f);
    OSSuspendThread(thread);
}

} // extern "C"
