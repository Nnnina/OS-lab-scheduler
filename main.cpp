#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <iomanip>
#include <set>
#include <queue>
#include "process.h"
#include "tokenizer.h"

using namespace std;
//------------global variable--------
const int quantum = 2;
string file = "LargeRandomNumbers.txt";
tokenizer* randomNumber = new tokenizer(file);
vector<process*> processes;
vector<process*> unsortedProcesses;
vector<process*> sjfProcesses;
queue<process*> readyQueue;
process* runningProcess = nullptr;
bool verbose = true;

int cycle = 0;
int processCount = 0;
double ioRunningTime = 0;
//---------------Methods-------------
int randomOS() {
    if(randomNumber->nextToken()) {
        return randomNumber->getToken();
    }
}

void readProcess(string file) {
    tokenizer* parser = new tokenizer(file);
    processCount = parser->getToken();
    for(int i = 0; i < processCount; i++) {
        int arrivalTime = parser->getToken();
        int cpuBurst = parser->getToken();
        int cpuTime = parser->getToken();
        int ioBurst = parser->getToken();
        process* p = new process(arrivalTime, cpuBurst, cpuTime, ioBurst);
        processes.push_back(p);
        unsortedProcesses.push_back(p);
        sjfProcesses.push_back(p);
    }
}

bool comp(process* const &a, process* const &b) {
    return a->arrivalTime < b->arrivalTime;
}

bool psjfComp(process* const &a,  process* const &b){
    if (a->remainingCpuTime != b->remainingCpuTime) {
        return a->remainingCpuTime < b->remainingCpuTime;
    } else {
        return a->arrivalTime < b->arrivalTime;
    }
}


void printInput() {
    cout<<"The original input was: "<<processes.size()<<setw(4);
    for (vector<process*>::iterator iter = unsortedProcesses.begin(); iter != unsortedProcesses.end(); ++iter) {
        cout<<(*iter)->arrivalTime<<" "<<(*iter)->cpuBurst<<" "<<(*iter)->cpuTime<<" "<<(*iter)->ioBurst<<setw(4);
    }
    sort(processes.begin(), processes.end(), comp);
    cout<<endl<<"The (sorted) input is: "<<processCount<<setw(4);
    for (vector<process*>::iterator iter = processes.begin(); iter != processes.end(); ++iter) {
        cout<<(*iter)->arrivalTime<<" "<<(*iter)->cpuBurst<<" "<<(*iter)->cpuTime<<" "<<(*iter)->ioBurst<<setw(4);
    }
    cout<<endl<<endl<<"This detailed printout gives the state and remaining burst for each process"<<endl;
}

void printProcesses() {
    if(verbose) cout<<"Before cycle     "<<cycle + 1<<":    ";
    bool isIO = false;
    for (vector<process*>::iterator iter = processes.begin(); iter != processes.end(); ++iter) {
        if((*iter)->state == "ready") { (*iter)->waitingTime++; }
        if((*iter)->state == "blocked") { isIO = true; }
        if(verbose) cout<<(*iter)->state<<"  "<<(*iter)->remainingTime<<"     ";
    }
    if (isIO) ioRunningTime++;
    if(verbose) cout<<endl;
}

bool allTerminated() {
    for (vector<process*>::iterator iter = processes.begin(); iter != processes.end(); ++iter) {
        if ((*iter)->state != "terminated") {
            return false;
        }
    }
    return true;
}

int stateToNumber(string state) {
    if (state == "unstarted") {
        return 0;
    } else if (state == "ready") {
        return 1;
    } else if (state == "running") {
        return 2;
    } else if (state == "blocked") {
        return 3;
    } else {
        return 4;
    }
}

void pringSummary() {
    float averageTurnaround = 0, averageWaiting = 0, cpuRunningTime = 0;
    double throughput = (100 / (double) cycle) * (double) processCount;
    for(int i = 0; i < processes.size(); i++) {
        averageTurnaround += processes[i]->turnaroundTime;
        averageWaiting += processes[i]->waitingTime;
        cpuRunningTime += processes[i]->cpuTime;
    }
    cout<<endl;
    cout<<"Summary Data: "<<endl;
    cout<<"Finishing time: "<<cycle<<endl;
    cout<<"CPU Utilization: "<<fixed<<setprecision(6)<<cpuRunningTime / cycle<<endl;
    cout<<"I/O Utilization: "<<fixed<<setprecision(6)<<ioRunningTime / cycle<<endl;
    cout<<"Throughput: "<<fixed<<setprecision(6)<<throughput<<" processes per hundred cycles "<<endl;
    cout<<"Average turnaround time: "<<fixed<<setprecision(6)<<averageTurnaround / processes.size()<<endl;
    cout<<"Average waiting time: "<<fixed<<setprecision(6)<<averageWaiting / processes.size()<<endl<<endl;

}

void printProcessesState() {
    cout<<endl;
    for(int i = 0; i < processes.size(); i++) {
        cout<<"Process "<<i<<":"<<endl;
        printf("(A,B,C,IO) = (%d,%d,%d,%d)",processes[i]->arrivalTime,processes[i]->cpuBurst,processes[i]->cpuTime,processes[i]->ioBurst);
        cout<<endl<<"Finishing time: "<<processes[i]->finishingTime<<endl;
        cout<<"Turnaround time: "<<processes[i]->turnaroundTime<<endl;
        cout<<"I/O time: "<<processes[i]->ioTime<<endl;
        cout<<"Waiting time: "<<processes[i]->waitingTime<<endl<<endl;
    }
}

void reset() {
    cycle = -1;
    runningProcess = nullptr;
    randomNumber = new tokenizer(file);
    ioRunningTime = 0;
    while (!readyQueue.empty())
    {
        readyQueue.pop();
    }
    for(int i = 0; i < processes.size(); i++) {
        processes[i]->remainingCpuTime = processes[i]->cpuTime;
        processes[i]->state = "unstarted";
        processes[i]->remainingTime = 0;
        processes[i]->start = false;
        processes[i]->finishingTime = 0;
        processes[i]->turnaroundTime = 0;
        processes[i]->ioTime = 0;
        processes[i]->waitingTime = 0;
        processes[i]->preempted = 0;
    }
}

void changeState(process* p) {
    switch (stateToNumber((*p).state)) {
        case 0://unstarted => ready
            if ((*p).arrivalTime == cycle) {
                (*p).state = "ready";
                readyQueue.push(p);
            }
            break;
        case 2://running => terminated/blocked/ready
            --(*p).preempted;
            if (--(*p).remainingCpuTime == 0) {
                //running => terminated
                runningProcess = nullptr;
                (*p).state = "terminated";
                (*p).remainingTime = 0;
                (*p).finishingTime = cycle;
                (*p).turnaroundTime = cycle - (*p).arrivalTime;
            } else if (--(*p).remainingTime == 0) {
                runningProcess = nullptr;
                if ((*p).preempted > 0) {
                    //running => ready
                    (*p).state = "ready";
                    readyQueue.push(p);
                } else {
                    //running => blocked
                    (*p).state = "blocked";
                    (*p).remainingTime = randomOS();
                    if(verbose) cout<<"Find I/O burst when blocking a process "<<(*p).remainingTime;
                    runningProcess = nullptr;
                    (*p).remainingTime = 1 + (*p).remainingTime % (*p).ioBurst;
                    if(verbose) cout<<" "<<(*p).remainingTime<<endl;
                    (*p).ioTime += (*p).remainingTime;
                }
            }
            break;
        case 3://blocked => ready
            if (--(*p).remainingTime == 0) {
                (*p).state = "ready";
                readyQueue.push(p);
            }
            break;
    }
}


void FCFS() {
    printInput();
    cycle = -1;
    while( !allTerminated() ) {
        printProcesses();
        cycle++;
        for (int i = 0; i < processes.size(); i++) {
            changeState(processes[i]);
        }
        //ready => running
        if (runningProcess == nullptr && !readyQueue.empty()) {
            runningProcess = readyQueue.front();
            readyQueue.pop();
            (*runningProcess).state = "running";
            int cpuBurst = randomOS();
            if(verbose) cout<<"Find burst when choosing ready process to run "<<cpuBurst;
            cpuBurst = 1 + cpuBurst % (*runningProcess).cpuBurst;
            if(verbose) cout<<" "<<cpuBurst<<endl;
            (*runningProcess).remainingTime =  cpuBurst > (*runningProcess).remainingCpuTime ? (*runningProcess).remainingTime : cpuBurst;
        }
    }
    cout<<"The Scheduling algorithm used was First Come First Served"<<endl;
    printProcessesState();
    pringSummary();
}

void uniRunProcess(process* &process) {
    if (process->state == "ready") {
        process->state = "running";
        int cpuBurst = randomOS();
        if(verbose) cout<<"Find burst when choosing ready process to run "<<cpuBurst;
        cpuBurst = 1 + cpuBurst % process->cpuBurst;
        if(verbose) cout<<" "<<cpuBurst<<endl;
        process->remainingTime = cpuBurst > process->remainingCpuTime ? process->remainingCpuTime : cpuBurst;
    }
}

void uniProgrammed() {
    printInput();
    reset();
    for(int i = 0; i < processes.size(); i++) {
        while(processes[i]->state != "terminated") {
            uniRunProcess(processes[i]);
            printProcesses();
            cycle++;
            for (int i = 0; i < processes.size(); i++) {
                changeState(processes[i]);
            }
            uniRunProcess(processes[i]);
        }
    }
    cout<<"The Scheduling algorithm used was Uniprocessor"<<endl;
    printProcessesState();
    pringSummary();
}


void RR(){
    reset();
    printInput();
    while( !allTerminated() ) {
        printProcesses();
        cycle++;
        for (int i = 0; i < processes.size(); i++) {
            changeState(processes[i]);
        }
        //ready => running
        if (runningProcess == nullptr && !readyQueue.empty()) {
            runningProcess = readyQueue.front();
            readyQueue.pop();
            (*runningProcess).state = "running";
            if((*runningProcess).preempted > 0){
                (*runningProcess).remainingTime = min((*runningProcess).preempted, quantum);
            } else {
                int cpuBurst = randomOS();
                if(verbose) cout<<"Find burst when choosing ready process to run "<<cpuBurst;
                cpuBurst = 1 + cpuBurst % (*runningProcess).cpuBurst;
                if(verbose) cout<<" "<<cpuBurst<<endl;
                (*runningProcess).preempted = cpuBurst;
                if (cpuBurst > quantum) {
                    (*runningProcess).remainingTime = quantum;
                } else (*runningProcess).remainingTime = cpuBurst;
                (*runningProcess).remainingTime = min((*runningProcess).remainingTime, (*runningProcess).remainingCpuTime);
            }
        }
        if (cycle > 1803) break;
    }
    cout<<"The Scheduling algorithm used was Round Robbin"<<endl;
    printProcessesState();
    pringSummary();
}

void SJF(){
    reset();
    printInput();
    //sort(processes.begin(), processes.end(), sjfComp);
    while( !allTerminated() ) {
        printProcesses();
        cycle++;
        for (int i = 0; i < processes.size(); i++) {
            changeState(processes[i]);
        }
        sort(sjfProcesses.begin(),sjfProcesses.end(),psjfComp);
        //ready => running
        for(int i = 0; i < sjfProcesses.size(); i++) {
            if (sjfProcesses[i]->state == "running") break;
            if (sjfProcesses[i]->state == "ready") {
                if (runningProcess == nullptr) {
                    runningProcess = sjfProcesses[i];
                } else {
                    (*runningProcess).state = "ready";
                    runningProcess = sjfProcesses[i];
                }
                (*runningProcess).state = "running";
                if (runningProcess->remainingTime == 0) {
                    int cpuBurst = randomOS();
                    if(verbose) cout<<"Find burst when choosing ready process to run "<<cpuBurst;
                    cpuBurst = 1 + cpuBurst % (*runningProcess).cpuBurst;
                    if(verbose) cout<<" "<<cpuBurst<<endl;
                    (*runningProcess).remainingTime =  cpuBurst;//> (*runningProcess).remainingCpuTime ? (*runningProcess).remainingTime : cpuBurst;
                }
                break;
            }
        }
    }
    cout<<"The Scheduling algorithm used was Preemptive Shortest Job First"<<endl;
    sort(processes.begin(), processes.end(), comp);
    printProcessesState();
    pringSummary();
}

int main(int argc, char* argv[]) {
    //string fileName(argv[1]);
    //file = argv[2];
    //readProcess(fileName);
    //verbose = argc == 3 ? true:false;
    readProcess("example7.txt");
    FCFS();
    RR();
    uniProgrammed();
    SJF();
    return 0;
}
