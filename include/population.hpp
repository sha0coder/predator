#pragma once

#include <vector>
#include "genotype.hpp"
#include "sandbox.hpp"

class Population {
private:
    Sandbox *sandbox;
    unsigned int pop_sz;
    unsigned long geno_sz;
    std::vector<Genotype *> popu;
    std::vector<Genotype *> ng;
    std::vector<int> sorted;
    Genotype *best;

    void eval();
    void sort();
    void crossover();
    void mutate();
    void change_generation();
    void clear();
    void diversity();
    void show_fitness();
public:
    Population(unsigned int popu_sz, unsigned long geno_sz);
    ~Population();
    unsigned int size();
    void evolve(unsigned int generations);
};