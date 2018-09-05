#include <vector>
#include <r_socket.h>
#include "include/strtk.hpp"
#include "include/genotype.hpp"
#include "include/r2.hpp"

using namespace std;

R2::R2() {
    this->r2 = r2p_open("r2 malloc://32");
}

R2::~R2() {
    r2p_close(this->r2);
}

void load(char *buff) {
    char hex[65];
    for (int i; i<32; i++) {
        snprintf(&hex[i*2], 2, "%.2x", buff[i]);
    }
    hex[64] = 0x00;
    printf("---->%s\n", hex),
    //r2p_cmd(this->r2, "wx ");
}

vector<unsigned int> R2::get_instruction_sizes(Genotype *g) {
    vector<unsigned int> opcodes_sz;
    if (!this->r2) {
        cout << "Genotype::r2cmd() r2 was not opened" << endl;
        return opcodes_sz; 
    }
    
    char *opcodes = r2p_cmd(this->r2, "pD 0x10 ~0x[1]"); // TODO: don't hardcode the size
    string sopcodes = string(opcodes);
    vector<string> opcode_vec;
    
    if (strtk::parse(sopcodes, "\n", opcode_vec)) {
        for (int i=0; i<opcode_vec.size(); i++) {
            opcodes_sz.push_back(opcode_vec[i].size()/2);
        }
    }

    return opcodes_sz;
}

void print_asm(void) {
    if (!this->r2) {
        cout << "Genotype::r2cmd() r2 was not opened" << endl;
        return;
    }
    r2p_cmd(this->r2, "pD 0x10"); // TODO: don't hardcode the size
}
