#include <cstdlib>
#include <iostream>
#include <cstring>
#include <random>
#include <fstream>
#include <assert.h>
#include "include/genotype.hpp"

using namespace std;

Genotype::Genotype(unsigned long sz) {
    this->fitness = 0;
    this->alloc(sz);
}

Genotype::~Genotype(void) {
    this->dealloc();
}

bool Genotype::alloc(unsigned long sz) {
    this->buff = (char *)malloc(sz+1);
    if (this->buff == NULL) {
        cerr << "No memory for a new Genotype sz:" << (sz+1) << endl;
        this->sz = 0;
        return false;
    }

    this->sz = sz;
    return true;
}

void Genotype::dealloc(void) {
    if (this->buff != NULL)
        free(this->buff);
    this->sz = 0;
}

unsigned long Genotype::size() {
    return this->sz;
}

char *Genotype::read(void) {
    return this->buff;
}

void Genotype::write(char *buff) {
    memcpy(this->buff, buff, this->sz);
}

Genotype *Genotype::clone(void) {
    Genotype *geno = new Genotype(this->sz);
    geno->write(this->buff);
    geno->set_fitness(this->fitness);
    return geno;
}

void Genotype::random() {
    for (unsigned int i=0; i<this->sz; i++) {
        this->buff[i] = rand()%255;
    }
}

void Genotype::show(void) {
    cout << "< ";
    for (unsigned int i=0; i<this->sz; i++)
        printf("%hhx ", this->buff[i]);

    cout << ">" << endl;
}

void Genotype::save(const char *filename) {
    ofstream fd;
    fd.open(filename,  ios::out | ios::binary);
    fd.write(this->buff, this->sz);
    fd.close();
    cout << "saved " << this->sz << " bytes" << endl;
}

void Genotype::load(const char *filename) {
    this->dealloc();

    ifstream fd;
    fd.open(filename, ios::in | ios::binary | ios::ate);
    if (this->alloc(fd.tellg())) {
        fd.seekg(0);
        fd.read(this->buff, this->sz);
        cout << "loaded " << this->sz << " bytes" << endl;
    }
    fd.close();
}

void Genotype::set_fitness(float fitness) {
    this->fitness = fitness;
}

float Genotype::get_fitness(void) {
    return this->fitness;
}

void Genotype::put(int pos, char c) {
    assert(pos<this->sz);
    this->buff[pos] = c;
}

char Genotype::get(int pos) {
    assert(pos<this->sz);
    return this->buff[pos];
}

