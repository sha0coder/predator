#pragma once
#include <vector>
#include "/home/sha0/soft/radare2/libr/include/r_socket.h"
#include "genotype.hpp"

class R2 {
private:
    R2Pipe *r2; 

public:
    R2();
    ~R2();

    void load(Genotype *g);
    std::vector<unsigned int> get_instruction_sizes();
    bool has_invalid_instructions();
    void print_asm();
};

