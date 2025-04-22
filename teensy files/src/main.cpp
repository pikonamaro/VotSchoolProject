#include <Arduino.h>
#include "Bios.h"
#include "Executor.h"
#include "RamApiWrapper.h"
#include "RamApiHostWrapper.h"
#include "Structures.h"
#include "TeensyThreads.h"
Interfaces interfaces;
BiosConfig biosConfig;

RamContext ctx = {
    .api = &GlobalRamApi,
    .config = &biosConfig,
    .iface = &interfaces
};
    
ProgramExecutor programExecutor;
Bios bios(&programExecutor, &biosConfig, &interfaces);


void setup() {
    Serial.begin(115200);
    
    
    bios.startup();
    Serial.println("bios done");
    initRamHostApi(&interfaces);
    Serial.println("ram done");
    programExecutor.setRamContext(&ctx);
    programExecutor.execute();
    

    Serial.println("Booted up");
}

void loop() {

    delay(1000);
}