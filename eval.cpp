#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <zconf.h>


#include "sys/types.h"
#include "sys/sysinfo.h"
#include "include/eval.hpp"

using namespace std;

Eval::Eval(int pid) {
    this->pid = pid;
}


bool Eval::loadLine(string filename, int line, char *buff, unsigned long sz) {
    bool ret  = false;
    ifstream ifs(filename, ios::in);
    if (ifs) {
        for (int i=0;i<line;i++) {
            memset(buff, 0, sz);
            ifs.getline(buff, sz);
        }
        ret = true;
    }
    ifs.close();
    return ret;
}


float Eval::getLoad() {
    float load = 0;
    char buff[11];

    buff[10] = 0x00;
    if (this->loadLine("/proc/loadavg", 1, buff, 10))
        load = stof(buff);

    return load;
}

float Eval::getLatency() {

}

float Eval::getOOM() {
    float load = 0;
    char buff[11];
    stringstream filename;
    filename << "/proc/" << this->pid << "/oom_score";
    buff[10] = 0x00;
    if (this->loadLine(filename.str(), 1, buff, 10)) {
        load = stof(buff);
    }

    return load;
}


float Eval::getIO() {
    int line = 0;
    float io = 0;
    char buff[41];
    stringstream filename;
    filename << "/proc/" << this->pid << "/io";
    ifstream ifs(filename.str(), ios::in);
    if (ifs) {
        memset(buff, 0, 41);
        while (ifs.getline(buff, 40)) {
            line++;
            if (line >= 5) {
                strtok(buff, " ");
                char *buff2 = strtok(NULL, " ");
                io += stof(buff2);
                //cout << "buff: " << buff2 << " num: " << stof(buff2) << endl;
            }
            memset(buff, 0, 41);
        }
    }
    ifs.close();


    return io;
}

float Eval::getCPU() {


    return 0;
}

float Eval::getVMEM() {

    return 0;
}

int Eval::getNumMaps() {
    int maps=0;
    char buff[255];
    stringstream filename;
    filename << "/proc/" << this->pid << "/maps";
    ifstream ifs(filename.str(), ios::in);
    if (ifs) {
        while (ifs.getline(buff, 255))
            maps++;
    }
    ifs.close();
    return maps;
}



char *Eval::nextWord(char *phrase) {
    char *p = phrase;
    while (*p != 0x00 && *p != ' ')
        p++;

    if (*p==0x00 || *(p+1)==0x00)
        return NULL;

    return p+1;
}

float Eval::getMemScore() {
    stringstream filename;
    filename << "/proc/" << this->pid << "/statm";
    char buff[50];
    memset(buff, 0, 50);

    float statm[7];

    if (this->loadLine(filename.str(), 1, buff, 49)) {
        char *p = buff;
        int i = 0;
        float val0 = stof(p);
        statm[i++] = val0;

        //cout << buff << endl;

        while ((p = this->nextWord(p)) != NULL) {
            float val = stof(p);
            statm[i++] = val;
        }
    }

    // MEM SCORING
    float mscore = 0;

    mscore += statm[0]/1000;
    if (mscore<0)
        mscore = 0;

    //mscore += statm[1]/50; parent

    mscore += statm[2];

    mscore += statm[3]-10;

    mscore += statm[4];

    mscore += statm[5]/100;

    mscore += statm[6];

    //cout << "mscore: " << statm[0]/1000 << " " << statm[1]/50  << " " << statm[2] << " " << statm[3]-10 << " " << statm[4] << " " << statm[5]/100 << " " << statm[6] << endl;

    return mscore;
}


float Eval::get_fitness() {
    // dynaic fitness

    int maps;
    float score = 0;
    if (getuid() == 0 || geteuid() == 0 || getgid() == 0)
        return 1000;

    // avoid be detected by kernel's OOM Killer
    float oom = this->getOOM();
    if (oom > 0)
        cout << "oom score: " << oom << endl;

    //score -= oom*10;
    score += oom*10;

    maps = this->getNumMaps();
    if (maps>30)
        score += (maps-30);

    score += this->getIO();

    //score += this->getMemScore();

    //cout << "nummaps:" << this->getNumMaps() << " io:" << this->getIO() << " mem: " << this->getMemScore() << endl;
    return  score;
}




/*

int main(void) {
    Eval ev(getpid());
    cout << ev.getIO() << endl;

}*/
