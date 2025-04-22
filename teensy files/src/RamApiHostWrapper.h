// RamHostWrapper.h
#ifndef RAM_HOST_WRAPPER_H
#define RAM_HOST_WRAPPER_H

#include "RamApiWrapper.h"
#include "Structures.h"
#include "RamContext.h"
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <stdlib.h>
#include <SD.h>
#include <malloc.h>
#include <TeensyThreads.h>



extern RamApiWrapper GlobalRamApi;

void initRamHostApi(Interfaces* iface);

#endif // RAM_HOST_WRAPPER_H

