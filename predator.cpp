#include "include/population.hpp"
#include <iostream>



int main(void) {
   
    auto *popu = new Population(100, 0x20);

    popu->evolve(500000);

    delete popu;
}

