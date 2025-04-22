#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>


struct BiosConfig {
    bool SPIEnabled = 0;
    uint8_t numSPIPins = 0;
    uint8_t SPIPins[20] = {0};  

    bool I2CEnabled = 0;
    

    uint8_t SoftwareSerialPins[10][2] = {0};
    uint8_t numSoftwareSerial = 0;
    uint8_t SoftwareSerialSpeed[10] = {0};

    uint8_t SDCardCS = 0; 

    bool bootFromSD = 0;

    uint8_t SerialJumper = 0;
    uint8_t BiosEditJumper = 0;
    uint8_t Led1Pin = 0;
    uint8_t Led2Pin = 0;
};

struct Interfaces{
    usb_serial_class* serial;
    SoftwareSerial* softwareSerial[10];
    SPIClass* spi;
    TwoWire* wire;
    SDClass* sd;
};

#endif