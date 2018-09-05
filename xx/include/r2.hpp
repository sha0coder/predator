#pragma once
#include <vector>
#include <r_socket.h>
#include "genotype.hpp"

class R2 {
private:
    R2Pipe *r2; 

public:
    R2();
    ~R2();

    void load(char *buff);
    std::vector<unsigned int> get_instruction_sizes();
    void print_asm();
};