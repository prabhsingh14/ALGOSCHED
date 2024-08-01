// The parsing functions and global variables defined here can be included and reused in multiple source files if needed, avoiding code duplication.

// Defining header guards - to prevent header file get included more than once in same compilation.
#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include<bits/stdc++.h>
using namespace std;

// Global variables that we will be parsing
string operation;
int lastInstant, processCount;

vector<pair<char,int>> algorithms; //identifier and associated parameter (char, int) (FCFS,-1)
vector<tuple<string,int,int>> processes; // identifier, arrival time, burst time
vector<vector<char>> timeline;

unordered_map<string,int> processToIndex;

vector<int>finishTime;
vector<int>turnAroundTime;
vector<float>normTurn; // normalised turn around time - TAT/BT, measure of efficiency of process scheduling

void parseAlgorithms(string algorithm){
    stringstream stream(algorithm);
    // while there is data left to read
    while(stream.good()){
        string strTemp;
        getline(stream, strTemp, ','); // separating individual algorithm specifications

        stringstream str(strTemp);
        getline(str, strTemp, '-'); // extracting algorithm id

        char algorithmId = strTemp[0];

        getline(str, strTemp, '-');
        int quantum = strTemp.size() >=1 ? stoi(strTemp) : -1; // -1 if quantum not provided/needed or is optional.

        // algorithm id and quantum has been extracted, add them to algorithm string
        algorithms.push_back(make_pair(algorithmId, quantum));
    }
}

void parseProcesses(){
    string process, processName;
    int processArrivalTime, processServiceTime;

    for(int i = 0; i < processCount; i++){
        cin>>process;

        stringstream stream(process);
        string strTemp;
        
        getline(stream, strTemp, ','); // extracting process name
        processName = strTemp;

        getline(stream, strTemp, ',');
        processArrivalTime = stoi(strTemp);

        getline(stream, strTemp, ',');
        processServiceTime = stoi(strTemp);

        processes.push_back(make_tuple(processName, processArrivalTime, processServiceTime));
        processToIndex[processName] = i;
    }
}

// ensuring all data structures are populated and are ready for processing
void parse(){
    string algorithm;
    cin>>operation>>algorithm>>lastInstant>>processCount;

    parseAlgorithms(algorithm);
    parseProcesses();
    
    finishTime.resize(processCount);
    turnAroundTime.resize(processCount);
    normTurn.resize(processCount);

    timeline.resize(lastInstant);

    for(int i = 0; i < lastInstant; i++){
        for(int j = 0; j < processCount; j++){
            timeline[i].push_back(' ');
        }
    }
}

#endif