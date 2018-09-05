#include "include/population.hpp"

#include <iostream>

/*
#include <vector>
#include "include/r2.hpp"
#include "include/genotype.hpp"

int r2test() {
    Genotype *g = new Genotype(0x20);
    g->random();
    g->random();g->random();
    g->show();

    std::vector<unsigned int> szs;

    R2 *r2 = new R2();
    r2->load(g);
    r2->print_asm();
    szs = r2->get_instruction_sizes();
    for (int i=0; i<szs.size(); i++)
        std::cout << i << ": " << szs[i] << std::endl;

    if (r2->has_invalid_instructions())
        std::cout <<  "invalid instr." << std::endl;
    else
        std::cout <<  "not invalid instr." << std::endl;

    delete r2;
    delete g;
}*/

int main(void) {
   
    auto *popu = new Population(100, 0x20);

    popu->evolve(500000);

    delete popu;
}

