#include "RamApiHostWrapper.h"


static BaseFunc baseFuncs;
static USBSerial usbFuncs;
static SPICom spiFuncs;
static I2CCom i2cFuncs;
static SerialMux serialMuxFuncs;
static SDInterface sdImpl;
static ThreadingInterface threadFuncs;

static Interfaces* interfacesRef = nullptr;

RamApiWrapper GlobalRamApi;
// --- BaseFunc ---
void basePrint(const char* msg) { Serial.println(msg); }

void basePrintf(const char* fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    Serial.println(buf);
}

void baseDelay(unsigned long ms) { delay(ms); }
unsigned long baseMillis() { return millis(); }
unsigned long baseMicros() { return micros(); }
void baseDigitalWrite(uint8_t pin, uint8_t val) { digitalWrite(pin, val); }
uint8_t baseDigitalRead(uint8_t pin) { return digitalRead(pin); }
void basePinMode(uint8_t pin, uint8_t mode) { pinMode(pin, mode); }
void* baseMalloc(size_t size) { return malloc(size); }
void baseFree(void* ptr) { free(ptr); }
void baseMemcpy(void* dst, const void* src, size_t size) { memcpy(dst, src, size); }
void* baseMemalign(size_t alignment, size_t size) { return memalign(alignment, size); }

// --- USBSerial ---
void usbPrint(const char* msg) { Serial.print(msg); }
void usbPrintCln(const char* msg) { Serial.println(msg); }
void usbPrintu32ln(const uint32_t val) { Serial.println(val); }
void usbPrintu8ln(const uint8_t val) { Serial.println(val); }
void usbPrintintln(const int val) { Serial.println(val); }
void usbPrintptrln(const uintptr_t val) { 
    Serial.print("0x");
    Serial.println(val, HEX); 
    }
int usbAvailable() { return Serial.available(); }
int usbRead() { return Serial.read(); }
void usbFlush() { Serial.flush(); }




// --- SPI ---
void spiBegin() { SPI.begin(); }
void spiBeginTransaction(void* settings) {
    SPI.beginTransaction(*reinterpret_cast<SPISettings*>(settings));
}
void spiEndTransaction() { SPI.endTransaction(); }
uint8_t spiTransfer(uint8_t b) { return SPI.transfer(b); }

// --- I2C ---
void i2cBegin() { Wire.begin(); }
void i2cBeginTransmission(uint8_t addr) { Wire.beginTransmission(addr); }
void i2cWrite(uint8_t data) { Wire.write(data); }
void i2cEndTransmission() { Wire.endTransmission(); }
uint8_t i2cRequestFrom(uint8_t addr, uint8_t qty) { return Wire.requestFrom(addr, qty); }
int i2cRead() { return Wire.read(); }

// --- SoftwareSerial Mux ---
void muxBegin(int idx, unsigned long baud) {
    if (!interfacesRef || idx < 0 || idx >= 10 || !interfacesRef->softwareSerial[idx]) return;
    interfacesRef->softwareSerial[idx]->begin(baud);
}

void muxEnd(int idx) {
    if (!interfacesRef || idx < 0 || idx >= 10 || !interfacesRef->softwareSerial[idx]) return;
    interfacesRef->softwareSerial[idx]->end();
}

int muxAvailable(int idx) {
    if (!interfacesRef || idx < 0 || idx >= 10 || !interfacesRef->softwareSerial[idx]) return false;
    return interfacesRef->softwareSerial[idx]->available();
}


int muxRead(int idx) {
    if (!interfacesRef || idx < 0 || idx >= 10 || !interfacesRef->softwareSerial[idx]) return -1;
    return interfacesRef->softwareSerial[idx]->read();
}


size_t muxWrite(int idx, uint8_t val) {
    if (!interfacesRef || idx < 0 || idx >= 10 || !interfacesRef->softwareSerial[idx]) return 0;
    return interfacesRef->softwareSerial[idx]->write(val);
}

void muxFlush(int idx) {
    if (!interfacesRef || idx < 0 || idx >= 10 || !interfacesRef->softwareSerial[idx]) return;
    interfacesRef->softwareSerial[idx]->flush();
}


//  --- SD card ---
static bool sd_begin(int pin) {
    return SD.begin(pin);
}

static bool sd_exists(const char* path) {
    return SD.exists(path);
}

uintptr_t sd_open(const char* path, const char* mode) {
    File* f = nullptr;

    if (*mode == 'w') {
        f = new File(SD.open(path, FILE_WRITE));
    } else if (*mode == 'r') {
        f = new File(SD.open(path, FILE_READ));
    } else {
        return 0;
    }

    if (!f || !*f) {
        delete f;
        return 0;
    }
    return (uintptr_t)f;
}

static size_t sd_read(uintptr_t file, void* buffer, size_t len) {
    if (!file || !buffer) return 0;
    File* f = reinterpret_cast<File*>(file);
    return f->read(reinterpret_cast<uint8_t*>(buffer), len);
}

static size_t sd_write(uintptr_t file, const void* buffer, size_t len) {
    if (!file || !buffer) return 0;
    File* f = reinterpret_cast<File*>(file);
    return f->write(reinterpret_cast<const uint8_t*>(buffer), len);
}

static void sd_close(uintptr_t file) {
    if (!file) return;
    File* f = reinterpret_cast<File*>(file);
    f->close();
    delete f;
}

static size_t sd_size(uintptr_t file) {
    if (!file) return 0;
    File* f = reinterpret_cast<File*>(file);
    return f->size();
}

static bool sd_remove(const char* path) {
    if (!path) return false;
    return SD.remove(path);
}

static void sd_flush(uintptr_t file) {
    if (!file) return;
    File* f = reinterpret_cast<File*>(file);
    f->flush();
}

static bool sd_mkdir(const char* path) {
    if (!path) return false;
    return SD.mkdir(path);
}

static bool sd_rmdir(const char* path) {
    if (!path) return false;
    return SD.rmdir(path);
}
//  --- Threads ---
int threadStart(int s) { return threads.start(s); }
int threadStop() { return threads.stop(); }
int threadYield() { threads.yield(); return 0; }
int addThreadS(void (*func)()) {
    return threads.addThread((void (*)(void*))func, nullptr);
}
int addThreadA(void (*func)(void*), void* arg , unsigned int stack_size , void* stack ) {
    return threads.addThread(func, arg, stack_size, stack);
} 
void threadDelay(int ms) { threads.delay(ms); }
int threadId() { return threads.id(); }
int threadKill(int id) { return threads.kill(id); }
int threadSuspend(int id) { return threads.suspend(id); }
int threadRestart(int id) { return threads.restart(id); }
int threadGetState(int id) { return threads.getState(id); }

//  --- Init ---
void initRamHostApi(Interfaces* iface) {
    interfacesRef = iface;

    baseFuncs.print         = basePrint;
    baseFuncs.printf        = basePrintf;
    baseFuncs.delay         = baseDelay;
    baseFuncs.millis        = baseMillis;
    baseFuncs.micros        = baseMicros;
    baseFuncs.digitalWrite  = baseDigitalWrite;
    baseFuncs.digitalRead   = baseDigitalRead;
    baseFuncs.pinMode       = basePinMode;
    baseFuncs.malloc        = baseMalloc;
    baseFuncs.free          = baseFree;
    baseFuncs.memcpy        = baseMemcpy;
    baseFuncs.memalign      = baseMemalign;

    usbFuncs.print          = usbPrint;
    usbFuncs.printcln       = usbPrintCln;
    usbFuncs.printu32ln     = usbPrintu32ln;
    usbFuncs.printu8ln      = usbPrintu8ln;
    usbFuncs.printintln     = usbPrintintln;
    usbFuncs.printptrln     = usbPrintptrln;
    usbFuncs.available      = usbAvailable;
    usbFuncs.read           = usbRead;
    usbFuncs.flush          = usbFlush;

    spiFuncs.begin              = spiBegin;
    spiFuncs.beginTransaction  = spiBeginTransaction;
    spiFuncs.endTransaction    = spiEndTransaction;
    spiFuncs.transfer          = spiTransfer;

    i2cFuncs.begin             = i2cBegin;
    i2cFuncs.beginTransmission= i2cBeginTransmission;
    i2cFuncs.write            = i2cWrite;
    i2cFuncs.endTransmission  = i2cEndTransmission;
    i2cFuncs.requestFrom      = i2cRequestFrom;
    i2cFuncs.read             = i2cRead;

    serialMuxFuncs.begin      = muxBegin;
    serialMuxFuncs.end        = muxEnd;
    serialMuxFuncs.available  = muxAvailable;
    serialMuxFuncs.read       = muxRead;
    serialMuxFuncs.write      = muxWrite;
    serialMuxFuncs.flush      = muxFlush;

    sdImpl.begin              = sd_begin;
    sdImpl.exists             = sd_exists;
    sdImpl.open               = sd_open;
    sdImpl.read               = sd_read;
    sdImpl.write              = sd_write;
    sdImpl.close              = sd_close;
    sdImpl.size               = sd_size;
    sdImpl.remove             = sd_remove;
    sdImpl.flush              = sd_flush;
    sdImpl.mkdir              = sd_mkdir;
    sdImpl.rmdir              = sd_rmdir;

    threadFuncs.start         = threadStart;
    threadFuncs.stop          = threadStop;
    threadFuncs.yield         = threadYield;
    threadFuncs.addthreadS    = addThreadS;
    threadFuncs.addthreadA    = addThreadA;
    threadFuncs.delay         = threadDelay;
    threadFuncs.id            = threadId;
    threadFuncs.kill          = threadKill;
    threadFuncs.suspend       = threadSuspend;
    threadFuncs.restart       = threadRestart;
    threadFuncs.getState      = threadGetState;

    GlobalRamApi.base = &baseFuncs;
    GlobalRamApi.usb  = &usbFuncs;
    GlobalRamApi.spi  = &spiFuncs;
    GlobalRamApi.i2c  = &i2cFuncs;
    GlobalRamApi.serialMux  = &serialMuxFuncs;
    GlobalRamApi.sd = &sdImpl;
    GlobalRamApi.threading = &threadFuncs;
}