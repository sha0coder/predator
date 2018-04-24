#pragma once
#include "genotype.hpp"

class Sandbox {
protected:
    char *pool;
    unsigned long pool_sz;
    bool ready;

    void launch(void);
    void clear(void);
public:
    Sandbox();
    ~Sandbox();
    void load(char *code, unsigned long len);
    void run(Genotype *geno);
};