#include <iostream>
#include <cstdlib>
#include <cstring>
#include "include/sandbox.hpp"
#include "include/genotype.hpp"

using namespace std;

int main(int argv, char **argc) {

  Genotype *gen = new Genotype(19);
  gen->load(argc[1]);

  Sandbox *sbox = new Sandbox();
  sbox->debug();
  sbox->run(gen);


  delete sbox;
  delete gen;

}


