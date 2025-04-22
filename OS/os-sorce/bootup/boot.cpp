// boot.cpp
#include <stdint.h>
#include "RamApiWrapper.h"

extern "C" void boot_main(RamApiWrapper* MCRef) __attribute__((section(".boot"), noinline));

RamApiWrapper* MCRefGlobal = nullptr;

void boot_main(RamApiWrapper* MCRef) {
    MCRefGlobal = MCRef;

    //================ space for configuring the system if needed ================









    //============================================================================
    //loading the kernel
    MCRefGlobal->usb->printcln("Trying SD...\n");
    if (MCRefGlobal->sd->begin(15)) {
        MCRefGlobal->usb->printcln("SD ready");
        uintptr_t file = 0;
        int size = 0;
        file = MCRefGlobal->sd->open("/system/kernel.bin", "r");
        size = MCRefGlobal->sd->size(file);
        MCRefGlobal->usb->printintln(size);
        void* buffer = MCRefGlobal->base->memalign(16, size);
        MCRefGlobal->sd->read(file, buffer, size);
        MCRefGlobal->sd->close(file);
        MCRefGlobal->usb->printcln("Kernel loaded");
        
        typedef void (*BootFunc)(RamApiWrapper*);
        BootFunc boot = (BootFunc)((uintptr_t)buffer | 1);
        boot(MCRefGlobal);

        
    } else {
        MCRefGlobal->usb->printcln("SD init failed");
    }
    
}

