#pragma once

#include <string>

class Eval {
    int pid;
private:

    bool loadLine(std::string filename, int line, char *buff, unsigned long sz);
public:
    Eval(int pid);
    float getOOM();
    int getNumMaps();
    float getIO();
    float getLatency();
    float getLoad();
    float getCPU();
    float getVMEM();
    float get_fitness();
};


