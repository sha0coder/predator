#include "include/sandbox.hpp"
#include "include/eval.hpp"

#include <iostream>
#include <cstdlib>
#include <cstring>

#include <sys/mman.h>
#include <unistd.h>
#include <zconf.h>
#include <wait.h>


Sandbox::Sandbox() {
    this->ready = false;
    std::cout << "Loading sandbox ..." << std::endl;
    int page_size = 0xfff;
    int geno_sz = 100;
    int page_sz = getpagesize();
    int pages = geno_sz/page_sz + 1;
    this->pool_sz = pages * page_sz;
    this->pool = (char *)malloc(this->pool_sz);
    if (this->pool <= 0) {
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
    if (this->pool>0)
        free(this->pool);
    this->pool = 0;
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
    if (len > this->pool_sz-5) {
        std::cout << "Sandbox: Aborting load, code too long, len: " << len << std::endl;
        return;
    }
    this->clear();
    memcpy(this->pool, code, len);
}

void Sandbox::launch(void) {
    void (*fptr)() = (void(*)())this->pool;
    fptr();
}

void Sandbox::run(Genotype *geno) {
    int pid, stat, sz, pipefd[2];
    char cfitness[6];


    this->load(geno->read(), geno->size());
    geno->set_fitness(0);

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return;
    }

    pid = fork();
    if (pid<0) {
        std::cout << "Sandbox: fork failed!!" << std::endl;
        return;
    }

    if (pid==0) { // GDB debug: set follow-fork-mode child
        // setsid();
        pid = getpid();
        sigaction(SIGSEGV, NULL, NULL);

        alarm(4);
        this->launch();

        //std::cout << "from child" << std::endl;
        Eval eval(pid);
        sprintf(cfitness, "%f", eval.get_fitness());
        write(pipefd[1], cfitness, strlen(cfitness));
        exit(1);

    } else {

        //std::cout << "waiting child" << std::endl;

        wait(&stat); //pid(pid);

        if (WCOREDUMP(stat)) {
            std::cout << "COREDUMPED!! " << std::endl;
            geno->show();
            geno->save("coredump.gen");
        }


        if (WIFSIGNALED(stat)) {
            if (WTERMSIG(stat)) {
                //std::cout << "ALARM!! " << std::endl;
            }
        }

        if (WIFEXITED(stat)) {
            sz = read(pipefd[0], cfitness, 4);
            geno->set_fitness(std::stof(cfitness)+2);

        } else {
            //std::cout << "signaled" << std::endl;
            Eval eval(pid);
            geno->set_fitness(eval.get_fitness());

            kill(pid, SIGKILL);
            kill(pid, SIGHUP);
        }


    }
}