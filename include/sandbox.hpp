#pragma once
#include "genotype.hpp"

class Sandbox {
protected:
    char *pool;
    bool isDebug;
    unsigned long pool_sz;
    unsigned long TIMEOUT;
    unsigned long RES_CRASH = 1;
    unsigned long RES_TIMEOUT = 2;
    unsigned long RES_UNKNOWN = 3;
    unsigned long RES_OK = 4;
    bool ready;

    void launch(void);
    void clear(void);
public:
    Sandbox();
    ~Sandbox();
    void debug(void);
    void load(char *code, unsigned long len);
    void run(Genotype *geno);
};