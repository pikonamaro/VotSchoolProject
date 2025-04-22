#include "Bios.h"

Bios::Bios(ProgramExecutor* executor , BiosConfig* config, Interfaces* interface ) {
    
    _executor = executor;
    _config = config;
    _interfaces = interface;

    _config->SPIEnabled = true;
    _config->numSPIPins = 0;
    memset(_config->SPIPins, -1, sizeof(_config->SPIPins));

    _config->I2CEnabled = false;

    _config->numSoftwareSerial = 0;
    memset(_config->SoftwareSerialPins, -1, sizeof(_config->SoftwareSerialPins));
    memset(_config->SoftwareSerialSpeed, 0, sizeof(_config->SoftwareSerialSpeed));

    _config->SDCardCS = 15;
    _config->bootFromSD = true;

    _config->SerialJumper = 6;
    _config->BiosEditJumper = 7;
    _config->Led1Pin = 5;
    _config->Led2Pin = 4;

    _interfaces->serial = &Serial;
    _interfaces->spi = &SPI;
    _interfaces->wire = &Wire;
    _interfaces->sd = &SD;

    for (int i = 0; i < 10; i++) {
        _interfaces->softwareSerial[i] = nullptr;
    }
    
}

void Bios::startup() {    
    
    pinMode(_config->SerialJumper, INPUT);
    pinMode(_config->BiosEditJumper, INPUT);
    pinMode(_config->Led1Pin, OUTPUT);
    pinMode(_config->Led2Pin, OUTPUT);
    
    unsigned long start_time = millis();
    
    while (true) {
        if (digitalRead(_config->SerialJumper) == HIGH) {
            SerialEnabled = true;
            break;
        }
        
        if (millis() - start_time > 10000) {
            break;
        }
    }

    if (SerialEnabled) {
        _interfaces->serial->begin(115200);
        _interfaces->serial->println("Serial enabled");
        digitalWrite(_config->Led1Pin, HIGH);
    }
    
    loadSettings();

    start_time = millis();
    
    if(SerialEnabled == true){
    _interfaces->serial->println("Press Btn 2 to edit settings.");
        
    
    while (SerialEnabled == true) {
        if (digitalRead(_config->BiosEditJumper) == HIGH) {
            SettingsEditor();
            break;
        }
        
        if (millis() - start_time > 10000) {
            break;
        }
    }
    }
    
    initAllInterfaces();

    //load bootloader
    loadBootloader();
    
}

void Bios::loadSettings() {
    if (EEPROM.read(0) == 0) {
        saveSettings();
        if(SerialEnabled == true){
            _interfaces->serial->println("bios settings not found, saved default settings");
        }
        return;
    }else if (EEPROM.read(0) == 1) {
        EEPROM.get(10, _config);
        _interfaces->serial->println("bios settings loaded");
    }
}
void Bios::saveSettings() {
    
    EEPROM.put(10, _config);
    EEPROM.put(0, 1);
}

void Bios::initSD() {
    pinMode(_config->SDCardCS,OUTPUT);
    while(true){
    if (!_interfaces->sd->begin(_config->SDCardCS)) {
        if(SerialEnabled == true){
            _interfaces->serial->println("SD Card initialization failed!");
        }   
        digitalWrite(_config->Led2Pin, HIGH);
        delay(2500);
    } else {
        if(SerialEnabled == true){
            _interfaces->serial->println("SD Card initialized.");
        }
        digitalWrite(_config->Led2Pin, LOW);
        break;
    }
    }
    while(true){
    if (!_interfaces->sd->exists("bootup/boot.bin")) {
        if(SerialEnabled == true){
            _interfaces->serial->println("Boot file not found, trying again...");
        }   
        digitalWrite(_config->Led2Pin, HIGH);
        delay(2500);
    } else {
        if(SerialEnabled == true){
            _interfaces->serial->println("Boot file found.");
        }
        digitalWrite(_config->Led2Pin, LOW);
        break;
    }
    }
}


bool Bios::isPinUsed(uint8_t pin) {

    if (pin == _config->Led1Pin || pin == _config->Led2Pin || 
        pin == _config->SerialJumper || pin == _config->BiosEditJumper) {
        return true;
    }

    if (_config->SPIEnabled) {

        if (pin == 13 || pin == 12 || pin == 11) {
            return true;
        }
        // Check all configured CS pins
        for (int i = 0; i < _config->numSPIPins; i++) {
            if (_config->SPIPins[i] == pin) {
                return true;
            }
        }
    }


    if (_config->I2CEnabled) {

        if (pin == 18 || pin == 19) {
            return true;
        }
    }


    for (int i = 0; i < _config->numSoftwareSerial; i++) {
        if (_config->SoftwareSerialPins[i][0] == pin || 
            _config->SoftwareSerialPins[i][1] == pin) {
            return true;
        }
    }

    return false;
}

void Bios::SettingsEditor() {
    if (!SerialEnabled) return;
    

    printCurrentSettings();
    

    while (true) {
        if (_interfaces->serial->available()) {
            String input = _interfaces->serial->readStringUntil('\n');
            input.trim();
            
            if (input == "?") {
                printHelp();
            }
            else if (input == "save") {
                saveSettings();
                _interfaces->serial->println("Settings saved to EEPROM.");
            }
            else if (input == "erase") {
                EEPROM.write(0, 0);
                _interfaces->serial->println("EEPROM erased. Restart to load defaults.");
            }
            else if (input.startsWith("spi=")) {
                handleSpiCommand(input);
            }
            else if (input.startsWith("i2c=")) {
                handleI2cCommand(input);
            }
            else if (input.startsWith("serial=")) {
                handleSerialCommand(input);
            }
            else if (input.startsWith("boot=")) {
                handleBootCommand(input);
            }
            else if (input.startsWith("cs=")) {
                handleCsCommand(input);
            }
            else if (input == "exit") {
                _interfaces->serial->println("Exiting BIOS editor.");
                return;
            }
            else {
                _interfaces->serial->println("Unknown command. Type '?' for help.");
            }
        }
    }
}

void Bios::printCurrentSettings() {
    _interfaces->serial->println("\n=== Current BIOS Settings ===");

    _interfaces->serial->print("SPI: ");
    _interfaces->serial->println(_config->SPIEnabled ? "ENABLED" : "DISABLED");
    _interfaces->serial->print("SPI CS Pins: ");
    if (_config->numSPIPins == 0) {
        _interfaces->serial->println("None");
    } else {
        for (int i = 0; i < _config->numSPIPins; i++) {
            _interfaces->serial->print(_config->SPIPins[i]);
            _interfaces->serial->print(" ");
        }
        _interfaces->serial->println();
    }
    

    _interfaces->serial->print("I2C: ");
    _interfaces->serial->println(_config->I2CEnabled ? "ENABLED" : "DISABLED");

    _interfaces->serial->println("SoftwareSerial Ports:");
    for (int i = 0; i < _config->numSoftwareSerial; i++) {
        _interfaces->serial->printf("  Port %d: RX=%d, TX=%d, Baud=%d\n", 
            i, 
            _config->SoftwareSerialPins[i][0], 
            _config->SoftwareSerialPins[i][1],
            _config->SoftwareSerialSpeed[i]);
    }
    

    _interfaces->serial->print("Boot from SD: ");
    _interfaces->serial->println(_config->bootFromSD ? "YES" : "NO");
    _interfaces->serial->print("SD Card CS Pin: ");
    _interfaces->serial->println(_config->SDCardCS);
    
    _interfaces->serial->println("\nType '?' for commands.");
}

void Bios::printHelp() {
    _interfaces->serial->println("\n=== BIOS Settings Commands ===");
    _interfaces->serial->println("?                - Show this help");
    _interfaces->serial->println("save             - Save settings to EEPROM");
    _interfaces->serial->println("erase            - Reset EEPROM to defaults");
    _interfaces->serial->println("exit             - Exit BIOS editor");
    _interfaces->serial->println("spi=enable       - Enable SPI");
    _interfaces->serial->println("spi=disable      - Disable SPI");
    _interfaces->serial->println("cs=pin           - Add SPI CS pin (e.g., cs=10)");
    _interfaces->serial->println("i2c=enable       - Enable I2C");
    _interfaces->serial->println("i2c=disable      - Disable I2C");
    _interfaces->serial->println("serial=RX,TX,BAUD - Add SoftwareSerial");
    _interfaces->serial->println("boot=sd          - Set boot source to SD");
    _interfaces->serial->println("boot=none        - Disable SD boot");
}

void Bios::handleSpiCommand(String input) {
    String value = input.substring(4);
    
    if (value == "enable") {
        _config->SPIEnabled = true;
        _interfaces->serial->println("SPI enabled.");
    } 
    else if (value == "disable") {
        _config->SPIEnabled = false;
        _interfaces->serial->println("SPI disabled.");
    } 
    else {
        _interfaces->serial->println("Usage: spi=enable or spi=disable");
    }
}

void Bios::handleCsCommand(String input) {
    int pin = input.substring(3).toInt();
    
    if (pin <= 0) {
        _interfaces->serial->println("Error: Invalid pin number.");
        return;
    }
    
    if (isPinUsed(pin)) {
        _interfaces->serial->printf("Error: Pin %d is already in use.\n", pin);
        return;
    }
    

    if (_config->numSPIPins < sizeof(_config->SPIPins)/sizeof(_config->SPIPins[0])) {
        _config->SPIPins[_config->numSPIPins++] = pin;
        _interfaces->serial->printf("Added SPI CS pin: %d\n", pin);
    } else {
        _interfaces->serial->println("Error: Maximum SPI CS pins reached.");
    }
}

void Bios::handleI2cCommand(String input) {
    String value = input.substring(4);
    
    if (value == "enable") {
        _config->I2CEnabled = true;
        _interfaces->serial->println("I2C enabled.");
    } 
    else if (value == "disable") {
        _config->I2CEnabled = false;
        _interfaces->serial->println("I2C disabled.");
    } 
    else {
        _interfaces->serial->println("Usage: i2c=enable or i2c=disable");
    }
}

void Bios::handleSerialCommand(String input) {

    int rx = input.substring(7).toInt();
    int tx = input.substring(input.indexOf(',') + 1).toInt();
    int baud = input.substring(input.lastIndexOf(',') + 1).toInt();

    if (rx <= 0 || tx <= 0 || baud <= 0) {
        _interfaces->serial->println("Error: Invalid RX/TX/BAUD values.");
        return;
    }


    if (isPinUsed(rx) || isPinUsed(tx)) {
        _interfaces->serial->printf("Error: Pin %d or %d is already in use.\n", rx, tx);
        return;
    }

    if (_config->numSoftwareSerial < 10) {
        _config->SoftwareSerialPins[_config->numSoftwareSerial][0] = rx;
        _config->SoftwareSerialPins[_config->numSoftwareSerial][1] = tx;
        _config->SoftwareSerialSpeed[_config->numSoftwareSerial] = baud;
        _config->numSoftwareSerial++;
        _interfaces->serial->printf("Added SoftwareSerial: RX=%d, TX=%d, Baud=%d\n", rx, tx, baud);
    } else {
        _interfaces->serial->println("Error: Max SoftwareSerial ports (10) reached.");
    }
}
void Bios::handleBootCommand(String input) {
    String value = input.substring(5);
    
    if (value == "sd") {
        _config->bootFromSD = true;
        _interfaces->serial->println("Boot from SD enabled.");
    } 
    else if (value == "none") {
        _config->bootFromSD = false;
        _interfaces->serial->println("Boot from SD disabled.");
    } 
    else {
        _interfaces->serial->println("Usage: boot=sd or boot=none");
    }
}


void Bios::initAllInterfaces() {
    bool bootFound = false;

    if (SerialEnabled) {
        _interfaces->serial->begin(115200);
    }


    if (_config->SPIEnabled) {
        _interfaces->spi->begin();
        if (SerialEnabled) {
            _interfaces->serial->println("SPI initialized");
            _interfaces->serial->print("CS Pins: ");
            for (int i = 0; i < _config->numSPIPins; i++) {
                _interfaces->serial->print(_config->SPIPins[i]);
                _interfaces->serial->print(" ");
                pinMode(_config->SPIPins[i], OUTPUT);
                digitalWrite(_config->SPIPins[i], HIGH);
            }
            _interfaces->serial->println();
        }
    }


    if (_config->I2CEnabled) {
        _interfaces->wire->begin();
        if (SerialEnabled) {
            _interfaces->serial->println("I2C initialized (SDA=18, SCL=19)");
        }
    }


    for (int i = 0; i < _config->numSoftwareSerial; i++) {
        if (_config->SoftwareSerialPins[i][0] != -1 && 
            _config->SoftwareSerialPins[i][1] != -1) {
            
            _interfaces->softwareSerial[i] = new SoftwareSerial(
                _config->SoftwareSerialPins[i][0], 
                _config->SoftwareSerialPins[i][1]
            );
            _interfaces->softwareSerial[i]->begin(_config->SoftwareSerialSpeed[i]);
            
            if (SerialEnabled) {
                _interfaces->serial->printf(
                    "SoftwareSerial %d: RX=%d, TX=%d, Baud=%d\n",
                    i,
                    _config->SoftwareSerialPins[i][0],
                    _config->SoftwareSerialPins[i][1],
                    _config->SoftwareSerialSpeed[i]
                );
            }
        }
    }


    if(_config->bootFromSD == true){
        if(_config->SPIEnabled == true){
            initSD();
            bootFound = true;
        }else{
            _interfaces->serial->println("SPI not enabled, cannot boot from SD card");
        }
    }
    if(bootFound == false){
        _interfaces->serial->println("Boot method not found, please try again");
        while(true){
            delay(10000);
        }
    }
}


#define MAX_BOOTLOADER_SIZE 8192  // Adjust based on your memory limits



bool Bios::loadBootloader() {
    uint8_t bootloaderBuffer[MAX_BOOTLOADER_SIZE];
    size_t bootloaderSize = 0;
    File bootFile = SD.open("/bootup/boot.bin", FILE_READ);
    if (!bootFile) {
        if (SerialEnabled) _interfaces->serial->println("Failed to open bootloader file.");
        return false;
    }
    bootloaderSize = 0;
    while (bootFile.available() && bootloaderSize < MAX_BOOTLOADER_SIZE) {
        bootloaderBuffer[bootloaderSize++] = bootFile.read();
    }
    bootFile.close();

    //give the file to executor

    if (SerialEnabled) {
        Serial.print("Bootloader read successfully. Size: ");
        Serial.print(bootloaderSize);
        Serial.println(" bytes.");
    }
    
    _executor->loadProgram(bootloaderBuffer, bootloaderSize);
    return true;
}

