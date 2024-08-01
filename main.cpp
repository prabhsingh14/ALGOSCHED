#include<bits/stdc++.h>
#include "parser.h" //header file, to parse input data

#define all(v) v.begin(), v.end() // By using all(v), we can concisely write v.begin(), v.end() making code more readable

using namespace std;

/*
string reprsenting various process scheduling algorithms
FCFS - First come first serve
RR - Round Robin
SPN - Shortest Process Next
SRT - Shortest remaining time
HRNN - Highest response ratio next
FB - Feedback
*/
const string Algorithms[9] = {"", "FCFS", "RR", "SPN", "SRT", "HRNN", "FB-1", "FB-2i", "AGING"};

// To display important information like waiting time, turn around time etc.
const string showData = "data";

// to see each process data like their process state
const string trace = "trace";

// Sorting functions
// string - process id/name - P1, P2, so on.
// rest two parameters signify arrival time and service or burst time respectively.
bool sortByServiceTime(const tuple<string, int, int>& process1, const tuple<string, int, int>& process2){
    return (get<2>(process1) < get<2>(process2)); //third input from the tuple - service time
    // in SJF, processes with shorter burst time are given priority.
}

bool sortByArrivalTime(const tuple<string, int, int> &process1, const tuple<string, int, int> &process2) {
    return (get<1>(process1) < get<1>(process2));
    // in FCFS, process who arrives first is given priority
}

bool sortResponseRatio(tuple<string, double, int> process1, tuple<string, double, int> process2){
    return (get<1>(process1) > get<1>(process2));
    // inn HRNN, process with highest response ratio is given priority.
}

// three inputs - priority, arrival time, service time
bool sortPriority(const tuple<int, int, int>& process1, const tuple<int, int, int>& process2){
    if(get<0>(process1) == get<0>(process2)){
        // if priority of both the processes are same, sort them on the basis of burst time
        return (get<2>(process1) > get<2>(process2));
    }

    return (get<0>(process1) > get<0>(process2));
}

// Functions to manage process details and their scheduling on a timeline
void clearTimeline(){
    for(int i = 0; i < lastInstant; i++){
        for(int j = 0; j < processCount; j++){
            timeline[i][j] = ' ';
        }
    }
}

string getProcessName(tuple<string,int,int>&process1){
    return (get<0>(process1));
}

int getArrivalTime(tuple<string, int, int> &process1) {
    return get<1>(process1);
}

int getServiceTime(tuple<string, int, int> &process1) {
    return get<2>(process1);
}

int getPriorityLevel(tuple<string, int, int> &process1){
    return get<2>(process1);
}

double responseRatio(int waitingTime, int serviceTime){
    return (waitingTime + serviceTime) * 1.0 / serviceTime;
}

void fillWaitingTime(){
    for(int i = 0; i < processCount; i++){
        int arrivalTime = getArrivalTime(processes[i]);

        for (int iter = arrivalTime; iter < finishTime[i]; iter++){
            // if the process is not running
            if (timeline[iter][i] != '*')
                timeline[iter][i] = '.'; //waiting
        }
    }
}

// FCFS - First-Come, First-Served (FCFS) schedules processes in the order they arrive, with each process running to completion before the next begins.
void firstComeFirstServe(){
    int time = getArrivalTime(processes[0]); // starting

    for(int i = 0; i < processCount; i++){
        int processIndex = i;
        int arrivalTime = getArrivalTime(processes[i]);
        int serviceTime = getServiceTime(processes[i]);

        finishTime[processIndex] = (time + serviceTime);
        turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
        normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0/serviceTime);
        
        // update timeline
        for(int j = time; j < finishTime[processIndex]; j++){
            timeline[j][processIndex] = '*';
        }

        for(int j = arrivalTime; j < time; j++){
            timeline[j][processIndex] = '.';
        }

        time += serviceTime; // scheduling next processes
    }
}

void roundRobin(int originalQuantum){
    queue<pair<int,int>> que; //pair of processIndex, remaining burst time
    int iter = 0;

    if(getArrivalTime(processes[iter]) == 0){
        que.push(make_pair(iter, getServiceTime(processes[iter])));
        iter++;
    }

    int currentQuantum = originalQuantum; // tracks remaining time slice of current process

    for(int i = 0; i < lastInstant; i++){
        if(!que.empty()){
            int processIndex = que.front().first;
            que.front().second -= 1;

            int remServiceTime = que.front().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);

            currentQuantum--;

            timeline[i][processIndex] = '*'; // current process is being executed

            while(iter < processCount && getArrivalTime(processes[iter]) == i+1){
                que.push(make_pair(iter, getServiceTime(processes[iter])));
                iter++;
            }

            // if current quantum is exhausted and process is completed
            if(currentQuantum == 0 && remServiceTime == 0){
                finishTime[processIndex] = i + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0/serviceTime);
                currentQuantum = originalQuantum;
                que.pop();
            }

            // If the quantum is exhausted but the process is not complete, push the process back into the queue with updated remaining service time.
            else if(currentQuantum == 0 && remServiceTime != 0){
                que.pop();
                que.push(make_pair(processIndex, remServiceTime));
                currentQuantum = originalQuantum;
            }

            // If the process is complete before quantum expires, record its finish time and move to the next process.
            else if(currentQuantum != 0 && remServiceTime == 0){
                finishTime[processIndex] = i + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0/serviceTime);
                que.pop();
                currentQuantum = originalQuantum;
            }   
        }

        while (iter < processCount && getArrivalTime(processes[iter]) == i + 1) {
            que.push(make_pair(iter, getServiceTime(processes[iter])));
            iter++;
        }
    }

    fillWaitingTime();
}

// process with the shortest service time is given priority, also known as shortest job next algorithm
void shortestProcessNext(){
    // sorted on the basis of service time
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq; // pair of service time and process index
    int iter = 0;

    for(int i = 0; i < lastInstant; i++){
        while(iter < processCount && getArrivalTime(processes[iter]) <= i){
            pq.push(make_pair(getServiceTime(processes[iter]), iter));
            iter++;
        }

        if(!pq.empty()){
            // index of process with shortest service time
            int processIndex = pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            pq.pop();

            // handling the timeline in case of process execution and idle
            int arrival = arrivalTime; // starting point of timeline

            for(; arrival < i; arrival++){
                timeline[arrival][processIndex] = '.'; // timeslots when no process is running
            }
            
            arrival = i;

            // marking process executing from start to end
            for(; arrival < i + serviceTime; arrival++){
                timeline[arrival][processIndex] = '*';
            }

            finishTime[processIndex] = (i + serviceTime);
            turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
            normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            i = arrival - 1;
        }
    }
}

// process with the shortest remaining time is given priority, making it a preemptive version of the Shortest Job First (SJF)
void shortestRemainingTime(){
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    
    // checks if any process has arrived, if yes then add it into the priority queue
    int iter = 0;
    for(int i = 0; i < lastInstant; i++){
        while(iter < processCount && getArrivalTime(processes[iter]) == i){
            pq.push(make_pair(getServiceTime(processes[iter]), iter));
            iter++;
        }

        if(!pq.empty()){
            int processIndex = pq.top().second;
            int remTime = pq.top().first;
            pq.pop();
            int serviceTime = getServiceTime(processes[processIndex]);
            int arrivalTime = getArrivalTime(processes[processIndex]);
            timeline[i][processIndex] = '*';

            if(remTime == 1){ //ensuring process will finish in the current time unit, prevents adding back to queue with remTime = 0
                finishTime[processIndex] = i + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            }

            else pq.push(make_pair(remTime - 1, processIndex));
        }
    }

    fillWaitingTime(); // waiting time of each process is calculated as scheduling is compl
}

// prioritizes processes based on their response ratio, which helps to avoid starvation and provides a better approximation of the shortest job next.
void highestResponseRatioNext(){
    vector<tuple<string, double, int>> presentProcesses;
    int curr = 0;

    for(int currInstant = 0; currInstant < lastInstant; currInstant++){
        // checks if any process has arrived, initialise response ratio = 1.0 and time in service = 0
        while(curr < processCount && getArrivalTime(processes[curr]) <= currInstant){
            presentProcesses.push_back(make_tuple(getProcessName(processes[curr]), 1.0, 0));
            curr++;
        }

        // calculate response ratio
        for(auto &process : presentProcesses){
            string processName = get<0>(process);
            int processIndex = processToIndex[processName];
            int waitingTime = currInstant - getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[serviceTime]);
            get<1>(process) = responseRatio(waitingTime, serviceTime);
        }

        sort(all(presentProcesses), sortResponseRatio);

        if(!presentProcesses.empty()){
            int processIndex = processToIndex[get<0>(presentProcesses[0])];

            while(currInstant < lastInstant && get<2>(presentProcesses[0]) != getServiceTime(processes[processIndex])){
                timeline[currInstant][processIndex] = '*';
                currInstant++;
                get<2>(presentProcesses[0])++; // process has been executed for one more unit of time
            }

            currInstant--;
            presentProcesses.erase(presentProcesses.begin());
            finishTime[processIndex] = currInstant + 1;
            turnAroundTime[processIndex] = finishTime[processIndex] - getArrivalTime(processes[processIndex]);
            normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / getServiceTime(processes[processIndex]));
        }
    }

    fillWaitingTime();
}

/*feedback scheduling algorithm that dynamically adjusts the priority of processes based on their remaining service times, 
ensures that processes are scheduled efficiently and are given more CPU time if they have not been completed, 
using a feedback mechanism to adjust their priorities.*/

void feedback(){
    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    unordered_map<int,int> remServiceTime;

    int iter = 0;
    if(getArrivalTime(processes[0]) == 0){
        pq.push(make_pair(0, iter));
        remServiceTime[iter] = getServiceTime(processes[iter]);
        iter++;
    }

    for(int time = 0; time < lastInstant; time++){
        if(!pq.empty()){
            int priority = pq.top().first;
            int processIndex = pq.top().second;

            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);

            pq.pop();

            while(iter < processCount && getArrivalTime(processes[iter]) == time + 1){
                pq.push(make_pair(0, iter));
                remServiceTime[iter] = getServiceTime(processes[iter]);
                iter++;
            }

            remServiceTime[processIndex]--;
            timeline[time][processIndex] = '*';
        
            if (remServiceTime[processIndex] == 0) {
                finishTime[processIndex] = time + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            }

            else{
                if(pq.size() >= 1) pq.push(make_pair(priority + 1, processIndex));
                else pq.push(make_pair(priority, processIndex));
            }
        }

        while (iter < processCount && getArrivalTime(processes[iter]) == time + 1) {
            pq.push(make_pair(0, iter));
            remServiceTime[iter] = getServiceTime(processes[iter]);
            iter++;
        }
    }

    fillWaitingTime();
}

// feedback scheduling algorithm with dynamic quantum (time quantum doubles for each higher priority)
void feedbackQuantum(){
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    unordered_map<int, int> remServiceTime;

    int iter = 0;

    // initialise queue with the process arriving at time 0
    if(getArrivalTime(processes[0]) == 0){
        pq.push(make_pair(0, iter));
        remServiceTime[iter] = getServiceTime(processes[iter]);
        iter++;
    }

    for(int time = 0; time < lastInstant; time++){
        if(!pq.empty()){
            int priority = pq.top().first;
            int processIndex = pq.top().second;

            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);

            pq.pop();

            // add new processes to the queue
            while(iter < processCount && getArrivalTime(processes[iter]) <= time+1){
                pq.push(make_pair(0, iter));
                remServiceTime[iter] = getServiceTime(processes[iter]);
                iter++;
            }

            int currentQuantum = pow(2, priority);
            int temp = time;

            // executes the process for upto current quantum
            while(currentQuantum && remServiceTime[processIndex]){
                currentQuantum--;
                remServiceTime[processIndex]--;
                timeline[temp++][processIndex] = '*';
            }

            if(remServiceTime[processIndex] == 0){
                finishTime[processIndex] = temp;
                turnAroundTime[processIndex] = finishTime[processIndex] - arrivalTime;
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0)/serviceTime;
            }

            else{
                if(pq.size() >=1 ) pq.push(make_pair(priority + 1, processIndex));
                else pq.push(make_pair(priority, processIndex));
            }

            time = temp - 1;
        }

        // add new processes
        while(iter < processCount && getArrivalTime(processes[iter]) <= time + 1){
            pq.push(make_pair(0,iter));
            remServiceTime[iter] = getServiceTime(processes[iter]);
            iter++;
        }
    }

    fillWaitingTime();
}

// processes are managed based on their priority and aging. Aging adjusts priority of the process that waits too long (prevents starvation)
void aging(int originalQuantum){
    vector<tuple<int,int,int>> vec; // priority, process index, total waiting time
    int iter = 0;
    int currProcess = -1;

    for(int time = 0; time < lastInstant; time++){
        while(iter < processCount && getArrivalTime(processes[iter]) <= time){
            vec.push_back(make_tuple(getPriorityLevel(processes[iter]), iter, 0));
            iter++;
        }

        for(int i = 0; i < vec.size(); i++){
            // current process is running
            if(get<1>(vec[i]) == currProcess){
                get<2>(vec[i]) = 0; // waiting time 0 as process is running
                get<0>(vec[i]) = getPriorityLevel(processes[currProcess]);
            }

            else{
                get<2>(vec[i])++;
                get<0>(vec[i])++;
            }
        }

        // sort the process based on their priority
        sort(vec.begin(), vec.end(), sortPriority);
        currProcess = get<1>(vec[0]);
        int currQuantum = originalQuantum;

        while(currQuantum-- && time < lastInstant){
            timeline[time][currProcess] = '*';
            time++;
        }

        time--;
    }

    fillWaitingTime();
}

void printAlgorithm(int algorithmIndex){
    int algorithmId = algorithms[algorithmIndex].first - '0';
    
    if(algorithmId==2)
        cout << Algorithms[algorithmId] <<algorithms[algorithmIndex].second <<endl;
    
    else
        cout << Algorithms[algorithmId] << endl;
}

void printProcesses(){
    cout << "Process    ";
    for (int i = 0; i < processCount; i++)
        cout << "|  " << getProcessName(processes[i]) << "  ";
    cout << "|\n";
}

void printArrivalTime(){
    cout << "Arrival    ";

    for (int i = 0; i < processCount; i++)
        printf("|%3d  ",getArrivalTime(processes[i]));
    
    cout<<"|\n";
}

void printServiceTime(){
    cout << "Service    |";

    for (int i = 0; i < processCount; i++)
        printf("%3d  |",getServiceTime(processes[i]));
    
    cout << " Mean|\n";
}

void printFinishTime(){
    cout << "Finish     ";

    for (int i = 0; i < processCount; i++)
        printf("|%3d  ",finishTime[i]);

    cout << "|-----|\n";
}

void printTurnAroundTime(){
    cout << "Turnaround |";
    
    int sum = 0;
    
    for (int i = 0; i < processCount; i++){
        printf("%3d  |",turnAroundTime[i]);
        sum += turnAroundTime[i];
    }

    if((1.0 * sum / turnAroundTime.size())>=10)
		printf("%2.2f|\n",(1.0 * sum / turnAroundTime.size()));
    else
	 	printf(" %2.2f|\n",(1.0 * sum / turnAroundTime.size()));
}

void printNormTurn(){
    cout << "NormTurn   |";
    
    float sum = 0;
    
    for (int i = 0; i < processCount; i++){
        if( normTurn[i]>=10 )
            printf("%2.2f|",normTurn[i]);
        else
            printf(" %2.2f|",normTurn[i]);
        sum += normTurn[i];
    }

    if( (1.0 * sum / normTurn.size()) >=10 )
        printf("%2.2f|\n",(1.0 * sum / normTurn.size()));
	else
        printf(" %2.2f|\n",(1.0 * sum / normTurn.size()));
}

void printStats(int algorithmIndex){
    printAlgorithm(algorithmIndex);
    printProcesses();
    printArrivalTime();
    printServiceTime();
    printFinishTime();
    printTurnAroundTime();
    printNormTurn();
}

void printTimeline(int algorithm_index){
    for (int i = 0; i <= lastInstant; i++)
        cout << i % 10<<" ";

    cout <<"\n";
    cout << "------------------------------------------------\n";
    
    for (int i = 0; i < processCount; i++){
        cout << getProcessName(processes[i]) << "     |";
        
        for (int j = 0; j < lastInstant; j++){
            cout << timeline[j][i]<<"|";
        }

        cout << " \n";
    }

    cout << "------------------------------------------------\n";
}

void executeAlgorithm(char algorithm_id, int quantum,string operation){
    switch (algorithm_id){
        case '1':
            if(operation==trace) cout<<"FCFS  ";
            firstComeFirstServe();
            break;
        
        case '2':
            if(operation==trace) cout<<"RR-"<<quantum<<"  ";
            roundRobin(quantum);
            break;
        
        case '3':
            if(operation==trace) cout<<"SPN   ";
            shortestProcessNext();
            break;
        
        case '4':
            if(operation==trace) cout<<"SRT   ";
            shortestRemainingTime();
            break;
        
        case '5':
            if(operation==trace) cout<<"HRRN  ";
            highestResponseRatioNext();
            break;
        
        case '6':
            if(operation==trace) cout<<"FB-1  ";
            feedback();
            break;

        case '7':
            if(operation==trace) cout<<"FB-2i ";
            feedbackQuantum();
            break;
        
        case '8':
            if(operation==trace) cout<<"Aging ";
            aging(quantum);
            break;
        
        default:
            break;
    }
}

int main(){
    parse();

    for (int index = 0; index < (int)algorithms.size(); index++){
        clearTimeline();

        executeAlgorithm(algorithms[index].first, algorithms[index].second,operation);

        if (operation == trace)
            printTimeline(index);
        
        else if (operation == showData)
            printStats(index);
        
        cout << "\n";
    }

    return 0;
}