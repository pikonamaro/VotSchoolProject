#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <Arduino.h>
#include <malloc.h> 
#include <Arduino.h>
#include "imxrt.h"
#include "core_cmInstr.h"
#include "my_core_cm7m.h"
#include "RamApiWrapper.h"
#include "RamApiHostWrapper.h"


class ProgramExecutor {
public:
    ProgramExecutor();
    

    bool loadProgram(const uint8_t* program, size_t size);
    void execute();
    bool isEmpty();
    bool isLoaded();
    bool isRunning();
    void setRamContext(RamContext* ctx);

    void stop();

private:
    
    
    uint8_t currentState;
    void *programPtr;
    size_t programSize;
    uint8_t *programMemory;
    
    // Memory management
    bool allocateMemory(size_t size);
    void freeMemory();
    RamContext* context;
};

#endif