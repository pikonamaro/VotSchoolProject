#include <stdint.h>
#include "RamApiWrapper.h"

extern "C" void boot_main(RamApiWrapper* MCRef) __attribute__((section(".boot"), noinline));

RamApiWrapper* MCRefGlobal = nullptr;

static uint8_t taskAStack[1024] __attribute__((aligned(16)));
static uint8_t taskVStack[1024] __attribute__((aligned(16)));


    
__attribute__((section(".funcs"))) void taskA(void* arg) {
    RamApiWrapper* api = (RamApiWrapper*)arg;
    while (true) {
        api->base->digitalWrite(4,1);
        api->base->delay(1000);
        api->base->digitalWrite(4,0);
        api->base->delay(1000);
    }
}

__attribute__((section(".funcs"))) void taskV(void* arg) {
    RamApiWrapper* api = (RamApiWrapper*)arg;
    
}


void boot_main(RamApiWrapper* MCRef) {
    MCRefGlobal = MCRef;
    MCRef->usb->printcln("enter");
    MCRef->base->delay(1000);
    MCRef->threading->addthreadA(taskA, (void*)MCRef, sizeof(taskAStack), taskAStack);
    MCRef->threading->addthreadA(taskV, (void*)MCRef, sizeof(taskVStack), taskVStack);
    MCRef->threading->start(-1);
    MCRef->usb->printcln("done");
    MCRef->base->delay(1000);
}
