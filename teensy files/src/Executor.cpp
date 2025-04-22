#include "Executor.h"

/*
States:
 1- Empty: A program is not loaded or running.
 2- Loaded: The program is loaded into memory, but not yet running.
 3- Running: The program is running.

*/

ProgramExecutor::ProgramExecutor() 
    {
        currentState = 1;
        programMemory = nullptr;
        programSize = 0;
    }

bool ProgramExecutor::loadProgram(const uint8_t* program, size_t size) {

    delay(1000);
    SCB_DisableICache();
    SCB_DisableDCache();

    if (currentState != 1) {
        return false;
    }
    
    if (!allocateMemory(size)) {
        return false;
    }

    memcpy(programMemory, program, size);
    programSize = size;
    currentState = 2;

    return true;
}

typedef void (*RamEntryFunc)(RamContext*);

void ProgramExecutor::execute() {
    if (currentState != 2) {
        return;
    }
    
    currentState = 3;
    
    typedef void (*BootFunc)(RamApiWrapper*);
    BootFunc boot = (BootFunc)((uintptr_t)programMemory | 1);
    boot(&GlobalRamApi);

    currentState = 1;
}

bool ProgramExecutor::isEmpty() {
    if (currentState == 1) {
        return true;
    }
    return false;
}

bool ProgramExecutor::isLoaded() {
    if (currentState == 2) {
        return true;
    }
    return false;
}

bool ProgramExecutor::isRunning() {
    if (currentState == 3) {
        return true;
    }
    return false;
}

void ProgramExecutor::stop() {
    if (currentState == 3) {
        currentState = 2;
    }
}

bool ProgramExecutor::allocateMemory(size_t size) {
    freeMemory();

    programMemory = (uint8_t*)memalign(16, size);
    return programMemory != nullptr;
}

void ProgramExecutor::freeMemory() {
    if (programMemory) {
        free(programMemory);
        programMemory = nullptr;
    }
    programSize = 0;
}

void ProgramExecutor::setRamContext(RamContext* ctx) {
    context = ctx;
}