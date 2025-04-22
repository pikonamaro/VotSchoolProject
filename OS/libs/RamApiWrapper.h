// RamApiWrapper.h
#ifndef RAM_API_WRAPPER_H
#define RAM_API_WRAPPER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------
// Base functions: timing, IO, memory
// ------------------------------
struct BaseFunc {
    void (*print)(const char*);
    void (*printf)(const char*, ...);
    void (*delay)(unsigned long);
    unsigned long (*millis)(void);
    unsigned long (*micros)(void);

    void (*digitalWrite)(uint8_t, uint8_t);
    uint8_t (*digitalRead)(uint8_t);
    void (*pinMode)(uint8_t, uint8_t);

    void* (*malloc)(size_t);
    void (*free)(void*);
    void (*memcpy)(void*, const void*, size_t);
    void* (*memalign)(size_t, size_t);
};

// ------------------------------
// USB Serial class wrapper
// ------------------------------
struct USBSerial {
    void (*print)(const char*);
    void (*printcln)(const char*);
    void (*printu32ln)(const uint32_t);
    void (*printu8ln)(const uint8_t);
    void (*printintln)(const int);
    void (*printptrln)(const uintptr_t);
    int (*available)();
    int (*read)();
    void (*flush)();
};

// ------------------------------
// SPI Communication wrapper
// ------------------------------
struct SPICom {
    void (*begin)();
    void (*beginTransaction)(void* settings);
    void (*endTransaction)();
    uint8_t (*transfer)(uint8_t);
};

// ------------------------------
// I2C Communication wrapper
// ------------------------------
struct I2CCom {
    void (*begin)();
    void (*beginTransmission)(uint8_t address);
    void (*write)(uint8_t data);
    void (*endTransmission)(void);
    uint8_t (*requestFrom)(uint8_t address, uint8_t quantity);
    int (*read)();
};

// ------------------------------
// Software Serial wrapper
// ------------------------------
struct SerialMux {
    void (*begin)(int index, unsigned long baud);
    void (*end)(int index);
    int  (*available)(int index);
    int  (*read)(int index);
    size_t (*write)(int index, uint8_t);
    void (*flush)(int index);
};

// ------------------------------
// SD Card wrapper
// ------------------------------
struct SDInterface {
    bool (*begin)(int pin);
    bool (*exists)(const char* path); 
    uintptr_t (*open)(const char* path, const char* mode);
    size_t (*read)(uintptr_t file, void* buffer, size_t len);
    size_t (*write)(uintptr_t file, const void* buffer, size_t len);
    void (*close)(uintptr_t file);
    size_t (*size)(uintptr_t file);
    bool (*remove)(const char* path);
    void (*flush)(uintptr_t file);
    bool (*mkdir)(const char* path);                 
    bool (*rmdir)(const char* path);                 
};
// ------------------------------
// Threading interface 
// ------------------------------
struct ThreadingInterface {
    int (*start)(int new_state);
    int (*stop)();
    int (*yield)();
    int (*addthreadS)(void (*func)());
    int (*addthreadA)(void (*func)(void*), void* arg, unsigned int stack_size, void* stack);
    void (*delay)(int ms);
    int (*id)();
    int (*kill)(int id);
    int (*suspend)(int id);
    int (*restart)(int id);
    int (*getState)(int id);
};
// ------------------------------
// Master API struct to hold everything
// ------------------------------
struct RamApiWrapper {
    BaseFunc* base;
    USBSerial* usb;
    SPICom* spi;
    I2CCom* i2c;
    SerialMux* serialMux;
    SDInterface* sd; 
    ThreadingInterface* threading;
};

#ifdef __cplusplus
}
#endif

#endif // RAM_API_WRAPPER_H