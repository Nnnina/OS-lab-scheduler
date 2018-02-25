//
// Created by Nina Lei on 2/19/18.
//

#ifndef OSLAB2_PROCESS_H
#define OSLAB2_PROCESS_H

#endif //OSLAB2_PROCESS_H

#include <iostream>
#include <string.h>
using namespace std;

class process {
public:
    process(int A, int B, int C, int io);
    int getArrivalTime();
    int getCpuBurst();
    int getCpuTime();
    void decreCpuTime();
    int getioBurst();
    void setState(string state);
    void printState();


    string state;
    int remainingTime;
    bool start;
    int preempted;
    int finishingTime;
    int turnaroundTime;
    int ioTime;
    int waitingTime;

    int remainingCpuTime;
    int arrivalTime;
    int cpuBurst;
    int cpuTime;
    int ioBurst;
};