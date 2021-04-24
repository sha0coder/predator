#include "include/sandbox.hpp"
#include "include/eval.hpp"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <random>
#include <sys/resource.h>
#include <sys/mman.h>
#include <unistd.h>
#include <zconf.h>
#include <wait.h>


Sandbox::Sandbox() {
    this->isDebug = false;
    this->ready = false;
    std::cout << "Loading sandbox ..." << std::endl;
    int page_size = 0xfff;
    int geno_sz = 100;
    int page_sz = getpagesize();
    int pages = geno_sz/page_sz + 1;
    this->TIMEOUT = 2;
    this->pool_sz = pages * page_sz;
    this->pool = (char *)malloc(this->pool_sz);
    
    if (this->pool == NULL) {
        std::cout << "The Sandbox cannot allocate the pool size of " << this->pool_sz << std::endl;
        return;
    }

    if (mprotect((void *)((unsigned long)this->pool & ~(page_sz-1)), this->pool_sz+page_sz, PROT_EXEC|PROT_WRITE|PROT_READ) < 0) {
        std::cerr << "The sandbox couldn't unprotect the pool" << std::endl;
        perror("mprotect");
        return;
    }

    this->clear();
    this->ready = true;
    std::cout << "sandbox ready." << std::endl;
}

Sandbox::~Sandbox() {
    if (this->pool != NULL)
        free(this->pool);
    this->pool = 0;
    std::cout << "sandbox unloaded. :(" << std::endl;
}

void Sandbox::debug() {
    this->isDebug = true;
}

void Sandbox::clear(void) {
    int TRAP = 0xcc;
    int NEAR_RET = 0xc3;
    int FAR_RET = 0xcb;


    /*
    char *exit = "\x31\xc0"              // xor  %eax,%eax
                 "\x40"                  // inc  %eax
                 "\x89\xc3"              // mov  %eax,%ebx
                 "\xcd\x80";             // int  $0x80
    */

    memset(this->pool, NEAR_RET, this->pool_sz);
}

void Sandbox::load(char *code, unsigned long len) {
    char NEAR_RET = 0xC3;
    char FAR_RET = 0xCB;

    if (len > this->pool_sz-5) {
        std::cout << "Sandbox: Aborting load, code too long, len: " << len << std::endl;
        return;
    }
    this->clear();
    memcpy(this->pool, code, len);
    memset(this->pool+len, NEAR_RET, 5);
}

void Sandbox::launch(void) {
    void (*fptr)() = (void(*)())this->pool;
    fptr();
}

int pipefd[2];

void pipe_alarm(int sig) {
    char cfitness[6] = "err";
    std::cout << "alarm!!" <<  std::endl;
    write(pipefd[1], cfitness, 4);
    close(pipefd[1]);
}


void Sandbox::run(Genotype *geno) {
    int pid, stat, sz;
    char cfitness[6];


    this->load(geno->read(), geno->size());
    geno->set_fitness(0);

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    //geno->save("run.gen.bin");

    pid = fork();
    if (pid<0) {
        std::cout << "Sandbox: fork failed!!" << std::endl;
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }

    if (pid==0) { // GDB debug: set follow-fork-mode child
        // setsid();
        pid = getpid();

        // prepare signals
        sigaction(SIGSEGV, NULL, NULL);
        alarm(this->TIMEOUT);

        this->launch();

        // std::cout << "from child" << std::endl;


        /*
        struct rusage *r_usage;
        if (getrusage(RUSAGE_SELF, r_usage) ==0) {
            r_usage.
        }*/


        Eval eval(pid);
        sprintf(cfitness, "%f", eval.get_fitness());

        // send the fitness through a pipe
        write(pipefd[1], cfitness, strlen(cfitness));
        close(pipefd[1]);
        exit(1);

    } else {

        //std::cout << "waiting child" << std::endl;

        wait(&stat); //pid(pid);
        //unsigned long id = rand() % 1000;
        unsigned long result = 0;

        if (WCOREDUMP(stat)) {
            if (this->isDebug)
                std::cout << " EXECUTION CRASHED!! " << std::endl;
            result = this->RES_CRASH;
            geno->set_fitness(0);
            //kill(pid, SIGKILL);
            //kill(pid, SIGHUP);

            //geno->show();
            //geno->save("coredump.gen");
        } else {

            if (WIFSIGNALED(stat)) {
                if (WTERMSIG(stat)) {
                    // SIGALARM timeout
                    if (this->isDebug)
                        std::cout << " EXECUTION TIMEOUT!! " << std::endl;
                    result = this->RES_TIMEOUT;
                    geno->set_fitness(1);

                    //kill(pid, SIGKILL);
                    //kill(pid, SIGHUP);
                }
            }

            if (WIFEXITED(stat)) {

                signal(SIGALRM, pipe_alarm);
                memset(cfitness, 0, sizeof(cfitness));
                alarm(this->TIMEOUT);
                sz = read(pipefd[0], cfitness, 4);
                alarm(0);
                signal(SIGALRM, NULL);
                if (strcmp(cfitness, "err")==0) {
                    std::cout << "err received!!" << std::endl;
                    cfitness[0] = '0';
                    cfitness[1] = 0x00;
                }


                geno->set_fitness(2 + std::stof(cfitness));
                if (this->isDebug)
                    std::cout << " EXECUTION OK!!" << std::endl;
                result = this->RES_OK;

            } else {
                if (this->isDebug)
                    std::cout << "signaled" << std::endl;


                //Eval eval(pid);
                //geno->set_fitness(eval.get_fitness());

                //std::cout << " EXECUTION SIGNALED" << std::endl;
                result = this->RES_UNKNOWN;
                
                //kill(pid, SIGKILL);
                //kill(pid, SIGHUP);
            }

        }
    }

    close(pipefd[0]);
    close(pipefd[1]);
}


