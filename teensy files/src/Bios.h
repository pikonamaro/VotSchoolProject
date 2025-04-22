#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include "RamApiHostWrapper.h"
#include "Executor.h"

#define BIOS_VERSION "1.0.0"




class Bios {
    public:

    Bios(ProgramExecutor* executor = nullptr, BiosConfig* config = nullptr, Interfaces* interface = nullptr);
    void startup();

    private:
    ProgramExecutor* _executor;
    BiosConfig* _config;
    Interfaces* _interfaces;
    bool loadBootloader();
    void SettingsEditor();
    bool isPinUsed(uint8_t pin);
    void printCurrentSettings();
    void printHelp();
    void handleSpiCommand(String input);
    void handleCsCommand(String input);
    void handleI2cCommand(String input);
    void handleSerialCommand(String input);
    void handleBootCommand(String input);
    


    bool SerialEnabled;
    void loadSettings();
    void saveSettings();
    void initSD();
    void initWire();
    void initSPI();
    void initSerial();
    int initSoftwareSerial();
    void initAllInterfaces();
};