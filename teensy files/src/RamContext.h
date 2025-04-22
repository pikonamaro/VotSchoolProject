#ifndef RAMCONTEXT_H
#define RAMCONTEXT_H

#include "RamApiWrapper.h"
#include "Structures.h"

struct RamContext {
    RamApiWrapper* api;
    BiosConfig* config;
    Interfaces* iface;
};

#endif // RAMCONTEXT_H