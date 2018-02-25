//
// Created by Nina Lei on 2/19/18.
//
#include <iostream>
#include <iomanip>
#include "process.h"

process::process(int A, int B, int C, int io) {
    this->arrivalTime = A;
    this->cpuBurst = B;
    this->cpuTime = C;
    this->ioBurst = io;
    this->state = "unstarted";
    this->remainingTime = 0;
    this->remainingCpuTime = C;
    this->start = false;
    this->preempted = 0;
}

