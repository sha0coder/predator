#include <iostream>
#include <sstream>
#include <fstream>
#include <random>
#include <limits>
#include <r_socket.h>

#include "include/population.hpp"
#include "include/sandbox.hpp"
#include "include/r2.hpp"


using namespace std;

Population::Population(unsigned int popu_sz, unsigned long geno_sz) {
    this->r2 = new R2();
    this->id = rand() % 1024;
    this->sandbox = new Sandbox();
    //this->sandbox->debug();
    this->pop_sz = popu_sz;
    this->geno_sz = geno_sz;
    this->best = NULL;
    for (unsigned int i=0; i<popu_sz; i++) {
        auto *geno = new Genotype(geno_sz);
        geno->random();
        this->popu.push_back(geno);
    }

    for (register int i=0; i<popu_sz; i++) {
        this->sorted.push_back(i);
    }
    cout << "Population engine on " << this->id << endl;
}

Population::~Population() {
    delete this->sandbox;
    this->clear();
    delete r2;
    cout << "Population engine off" << endl;
}

void Population::clear() {
    for (int i=0; i<this->popu.size(); i++) {
        delete this->popu[i];
    }
    this->popu.clear();
}

unsigned int Population::size(void) {
    return this->popu.size();
}

void Population::sort(void) {
    int sz = this->popu.size();

    // bubble sort
    register int i,j,k, tmp;

    for (i=0; i<sz-1; i++) {
        k = i;
        for (j=i+1; j<sz; j++) {
            if (this->popu[this->sorted[j]]->get_fitness() > this->popu[this->sorted[k]]->get_fitness()) {
                k = j;
            }
        }

        tmp = this->sorted[k];
        this->sorted[k] = this->sorted[i];
        this->sorted[i] = tmp;
    }

    if (this->best == NULL) {
        this->best = this->popu[this->sorted[0]]->clone();
    } else {
        if (this->popu[this->sorted[0]]->get_fitness() > this->best->get_fitness()) {
            //cout << "cambio de best better:" << this->popu[this->sorted[0]]->get_fitness() << " gana al best:" << this->best->get_fitness() << endl;
            delete this->best;
            this->best = this->popu[this->sorted[0]]->clone();
        }
    }



    // Save Best to file

    //stringstream filename;
    //filename << "best_" << this->best->get_fitness() << ".bin";
    //this->best->save(filename.str().c_str());



    /*
    if (!this->best || this->popu[this->sorted[0]]->get_fitness() > this->best->get_fitness()) {
        this->best = this->popu[this->sorted[0]]->clone();

    }*/
}

void Population::eval(void) {
    for (int i=0; i<this->popu.size(); i++) {
        //this->popu[i]->random();
        this->sandbox->run(this->popu[i]);
    }
}


void Population::asm_crossover(void) {
    // Logical instruction level crossover
    Genotype *g1, *g2;
    vector<unsigned int> szs1, szs2;

    int top = this->popu.size()*0.05;

    for (int i=0; i<top-2; i++) {
        for (int j=2; j<top; j++) {
            g1 = this->popu[this->sorted[i]];
            g2 = this->popu[this->sorted[j]];

            r2->load(g1);
            szs1 = r2->get_instruction_sizes();
            r2->load(g2);
            szs2 = r2->get_instruction_sizes();

            // Halfs
            int k;
            int sz1 = szs1.size();
            int sz2 = szs2.size();
            int half_asm1 = sz1/2;
            int half_asm2 = sz2/2;
            int half_bytes1 = 0;
            int half_bytes2 = 0;

            for (k=0; k<half_asm1; k++)
                half_bytes1 += szs1[k];
            for (k=0; k<half_asm2; k++)
                half_bytes2 += szs2[k];
            

            //int q1 = half/2;
            //int q2 = half+q1;
            Genotype *child1 = new Genotype(this->geno_sz);
            Genotype *child2 = new Genotype(this->geno_sz);
          

            for (k=0; k < this->geno_sz; k++) {

                if (k < half_asm1) {
                    child1->put(k, g1->read()[k]);
                } else {
                    child2->put(k, g1->read()[k]);
                }

                if (k < half_asm2) {
                    child2->put(k, g2->read()[k]);
                } else {
                    child1->put(k, g2->read()[k]);
                }

            }
            
        }
    }


}

/*
static void r2cmd(R2Pipe *r2, const char *cmd) {
    char *msg = r2p_cmd(r2, cmd);
    if (msg) {
        printf ("%s\n", msg);
        free (msg);
    }
}*/


void Population::byte_crossover(void) {
    char *gen1, *gen2;

    int top = this->popu.size()*0.05;

    for (int i=0; i<top-2; i++) {
        for (int j=2; j<top; j++) {
            gen1 = this->popu[this->sorted[i]]->read();
            gen2 = this->popu[this->sorted[j]]->read();

            // halfs
            int half = this->geno_sz/2;
            Genotype *child1 = new Genotype(this->geno_sz);
            Genotype *child2 = new Genotype(this->geno_sz);

            // quarters
            Genotype *child[8];
            for (int k=0; k<8; k++)
                child[k] = new Genotype(this->geno_sz);

            int q1 = half/2;
            int q2 = half+q1;

            for (int k=0; k<this->geno_sz; k++) {

                if (k < q1) {
                    // Q1
                    child[0]->put(k, gen1[k]); // intercalado 1
                    child[1]->put(k, gen2[k]); // intercalado 2

                    child[2]->put(k, gen1[k]); // 3fin 1
                    child[3]->put(k, gen2[k]); // 3fin 2

                    child[4]->put(k, gen1[k]); // 3ini 1
                    child[5]->put(k, gen2[k]); // 3ini 2

                    child[6]->put(k, gen1[k]); // middle 1
                    child[7]->put(k, gen2[k]); // middle 2

                } else if (k >= q1 && k < half) {
                    // Q2
                    child[0]->put(k, gen2[k]); // intercalado 1
                    child[1]->put(k, gen1[k]); // intercalado 2

                    child[2]->put(k, gen1[k]); // 3fin 1
                    child[3]->put(k, gen2[k]); // 3fin 2

                    child[4]->put(k, gen2[k]); // 1fin 1
                    child[5]->put(k, gen1[k]); // 3ini 2

                    child[6]->put(k, gen2[k]); // middle 1
                    child[7]->put(k, gen1[k]); // middle 2

                } else if (k >= half && k < q2) {
                    // Q3
                    child[0]->put(k, gen1[k]); // intercalado 1
                    child[1]->put(k, gen2[k]); // intercalado 2

                    child[2]->put(k, gen1[k]); // 3fin 1
                    child[3]->put(k, gen2[k]); // 3fin 2

                    child[4]->put(k, gen2[k]); // 3ini 1
                    child[5]->put(k, gen1[k]); // 3ini 2

                    child[6]->put(k, gen2[k]); // middle 1
                    child[7]->put(k, gen1[k]); // middle 2

                } else if (k >= q2) {
                    // Q4
                    child[0]->put(k, gen2[k]); // intercalado 1
                    child[1]->put(k, gen1[k]); // intercalado 2

                    child[2]->put(k, gen2[k]); // 3fin 1
                    child[3]->put(k, gen1[k]); // 3fin 2

                    child[4]->put(k, gen2[k]); // 3ini 1
                    child[5]->put(k, gen1[k]); // 3ini 2

                    child[6]->put(k, gen1[k]); // middle 1
                    child[7]->put(k, gen2[k]); // middle 2
                }

                /*

                 ABCD
                 1234

                 AB34 no
                 12CD no

                 A2C4 si
                 1B3D si

                 ABC4 si
                 123D si

                 A234 si
                 1BCD si

                 1BC4 si
                 A23D si

                */


                // half1
                if (k < half) {
                    child1->put(k, gen1[k]);
                    child2->put(k, gen2[k]);

                // half2
                } else {
                    child1->put(k, gen2[k]);
                    child2->put(k, gen1[k]);
                }
            }

            this->ng.push_back(child1);
            this->ng.push_back(child2);

            for (int k=0; k<8; k++)
                this->ng.push_back(child[k]);

        }
    }
}

void Population::mutate2(int prob) {
    //TODO: reimplement mutation engine
    for (int i=0; i<this->popu.size(); i++) {


    }
}

void Population::mutate(int prob) {
    // more probabilities to have a ret or a int or a mov eax

    char NEAR_RET = 0xc3;
    char FAR_RET = 0xcb;
    char MOV_EAX_1[] = "\xb8\x01\x00\x00\x00";
    char MOV_EAX = 0xb8;
    char MOV_EBX = 0xbb;
    char MOV_ECX = 0xb9;
    char MOV_EDX = 0xba;
    char INC_EAX = 0x40;
    char XOR_EAX = 0x35;
    char INT80[] = "\xfd\x80"; // 0
    char SYSENTER[] = "\x0f\x05"; // 5
    char SYSENTER2[] = "\x0f\x34"; // 4  
    char INT = 0xfd;

    for (int i=0; i<this->ng.size(); i++) {
        if (rand()%100 == prob) {
            int pos = rand() % this->geno_sz;



            if (rand()%100 <= 30) {
                this->ng[i]->put(pos, INC_EAX);

            } else if(rand()%100 <= 20) {
                this->ng[i]->put(pos, XOR_EAX);

            } else if (pos<this->geno_sz-1 && rand()%100<=30) {
                this->ng[i]->put(pos, SYSENTER[0]);
                this->ng[i]->put(pos+1, SYSENTER[1]);

            } else if (pos<this->geno_sz-1 && rand()%100<=20) {
                this->ng[i]->put(pos, SYSENTER2[0]);
                this->ng[i]->put(pos+1, SYSENTER2[1]);

            } else if (pos<this->geno_sz-1 && rand()%100<=20) {
                this->ng[i]->put(pos, INT80[0]);
                this->ng[i]->put(pos+1, INT80[1]);

            } else if(rand()%100 <= 20) {
                switch(rand()%4) {
                    case 0: this->ng[i]->put(pos, MOV_EAX); break;
                    case 1: this->ng[i]->put(pos, MOV_EBX); break;
                    case 2: this->ng[i]->put(pos, MOV_ECX); break;
                    case 3: this->ng[i]->put(pos, MOV_EDX); break;
                }
            } else if(rand()%100 <= 20) {
                this->ng[i]->put(pos, '\x00');

            } else if(rand()%100 <= 20) {
                this->ng[i]->put(pos, INT);

            } else if(rand()%100 <= 20) {
                this->ng[i]->put(pos, NEAR_RET);

            } else if(rand()%100 <= 20) {
                this->ng[i]->put(pos, 0x0f);

            } else if(rand()%100 <= 20) {
                this->ng[i]->put(pos, FAR_RET);

            } else {
                this->ng[i]->put(pos, rand()%255);
            }
        }
    }
}

void Population::change_generation() {
    int i;
    for (i=0; i<this->popu.size(); i++) {
        delete this->popu[i];
    }
    this->popu.clear();

    for (int i=0; i<this->ng.size(); i++) {
        this->popu.push_back(this->ng[i]);
    }
    this->ng.clear();
}

void Population::show_fitness(void) {
    for (int i=0; i<this->popu.size(); i++)
        cout << "id: " << i << " fitness: " << this->popu[i]->get_fitness() << endl;
}

void Population::diversity(void) {
    register long sz = this->popu.size();
    while(this->ng.size() < this->pop_sz) {
        this->ng.push_back(this->popu[rand()%sz]->clone());
    }
}


// Core

void Population::evolve(unsigned int generations) {
    unsigned int g;
    cout << "Start evolving ... " << endl;
    for (g=1; g<generations; g++) {

        this->eval();
        this->sort();

        //this->show_fitness();

        float sum = 0;
        for (int i=0; i<this->popu.size(); i++) {
            //cout << "finess[" << i << "]: " << this->popu[i]->get_fitness() << endl;
            sum += this->popu[i]->get_fitness();
        }
        if (sum >= 1)
            sum /= this->popu.size();
        else
            sum = 0;

        if (best == NULL)
            cout << "WTFFFFF" << endl;

        this->r2->load(best);
        this->r2->print_asm();

        // save stats
        ofstream fda;
        fda.open("stats",  ios::out|ios::app);
        fda << g << "," << this->best->get_fitness() << "," << sum << endl;
        fda.close();




        cout << this->id << " ======================================================================" << endl;
        cout << "Generation " << g << " best fitness: " << this->best->get_fitness();
        cout << " population: " << this->popu.size() << " global fitness: " << sum << " mutation: " << (float)(30/g) << endl;
        this->best->show();
        cout << "======================================================================" << endl;
        if (g >= generations)
            break;


        //cin.ignore(numeric_limits<streamsize>::max(),'\n');


        this->ng.clear();
        this->byte_crossover();
        //cout << "ng: " << this->ng.size() << endl;
        this->mutate(30/g);

        // best survive
        for (int i=0; i<3; i++)
            this->ng.push_back(this->best->clone());


        // one random

        Genotype *geno = new Genotype(this->geno_sz);
        geno->random();
        this->ng.push_back(geno);

        this->diversity();
        this->change_generation();

    }
    cout << "Evolution completed" << endl;
    this->popu[this->sorted[0]]->save("final.best.geno");
}
