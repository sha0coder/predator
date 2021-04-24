#include <vector>
#include <assert.h>
#include <string>
#include <r_socket.h>
#include "include/strtk.hpp"
#include "include/genotype.hpp"
#include "include/r2.hpp"

using namespace std;

R2::R2() {
    cout << "opening link" << endl;
    this->r2 = r2pipe_open("r2 -w -q0 malloc://32");
    cout << "link ok" << endl;
}

R2::~R2() {
    cout << "closing link" << endl;
    r2pipe_close(this->r2);
    cout << "link closed" << endl;
}

void R2::load(Genotype *g) {
    assert(g->size() == 0x20);

    char *buff = g->read();
    char hex[68];
    hex[0] = 'w'; hex[1] = 'x'; hex[2] = ' ';
    for (int i; i<32; i++) {
        char *p = &hex[i*2+3];
        snprintf(p, 3, "%.2x", *((unsigned char *)&buff[i]));
    }
    hex[67] = 0x00;

    r2pipe_cmd(this->r2, hex);
}

vector<unsigned int> R2::get_instruction_sizes() {
    vector<unsigned int> opcodes_sz;
    if (!this->r2) {
        cout << "Genotype::r2cmd() r2 was not opened" << endl;
        return opcodes_sz; 
    }
    
    string::size_type sz;
    char *szs = r2pipe_cmd(this->r2, "pdl 0x20 ~!0");
    string sszs = string(szs);
    vector<string> snums;

    if (strtk::parse(sszs, "\n", snums)) {
        for (int i=0; i<snums.size()-1; i++) {
            //string s = snums[i];
            opcodes_sz.push_back( stoi (snums[i],&sz) );
        }
    }

    free(szs);
    return opcodes_sz;
}

void R2::print_asm(void) {
    char *out;

    if (!this->r2) {
        cout << "Genotype::r2cmd() r2 was not opened" << endl;
        return;
    }

    cout << "showing r2pipe cmd pD" << endl;
    
    out = r2pipe_cmd(this->r2, "pD 0x20");  // TODO: don't hardcode the size    

    printf("%s\n", out); 

    free(out);
    cout << "done." << endl;
}

bool R2::has_invalid_instructions(void) {
    if (!this->r2) {
        cout << "Genotype::r2cmd() r2 was not opened" << endl;
        return false; 
    }

    vector<unsigned int> opcodes_sz;
    string::size_type sz;
    char *szs = r2pipe_cmd(this->r2, "pdl 0x20");
    string sszs = string(szs);
    vector<string> snums;
    int sum = 0;

    if (strtk::parse(sszs, "\n", snums)) {
        for (int i=0; i<snums.size()-1; i++) {
            sum += stoi (snums[i], &sz);
        }
    }

    return (sum < 0x20);
}

