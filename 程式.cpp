#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
using namespace std;


class Process {
  public : int pid, CPU_burst, arrivalTime, priority; // input
  public : char code; // 0 -> 0, 10 -> A, 35 -> Z
  public : int waitingTime, turnaroundTime;
  public : string status;
  public : double responseRatio;

  public : Process(int pID, int cb, int at, int p) {
    pid = pID, CPU_burst = cb, arrivalTime = at, priority = p, waitingTime = turnaroundTime = 0, responseRatio = 0.0, status = "Not Arrival";
    PidTransToCode();    
  } // constructor


  public : void PidTransToCode() {
    if (0 <= pid && pid <= 9) code = '0' + pid;
    else                      code = 'A' + (pid - 10);
  } // PidTransToCode()


  public : void RenewResponseRatio() {
    responseRatio = double(waitingTime + CPU_burst) / double(CPU_burst);
  } // RenewResponseRatio()

}; // class process


class Schedule {
  public : string name, GanttChart;
  public : int timeSlice, time;
  public : vector<Process*> readyQueue;
  public : vector<Process*> allProcess;


  public : Schedule(string n, int ts) {
    name = n, timeSlice = ts, GanttChart = "", time = 0;
    readyQueue.clear();
    allProcess.clear();
  } // constructor


  public : void Scheduling() {
    SortProcessByPID();

    if      (name == "FCFS") FCFS();
    else if (name ==   "RR")   RR();
    else if (name ==  "SJF")  SJF();
    else if (name == "SRTF") SRTF();
    else if (name == "HRRN") HRRN();
    else if (name == "PPRR") PPRR();
  } // Scheduling()


  public : void SortProcessByPID() {
    for (int a = 1; a < allProcess.size(); a++)
      for (int b = 0; b < allProcess.size() - a; b++)
        if (allProcess[b]->pid > allProcess[b + 1]->pid)
          swap(allProcess[b], allProcess[b + 1]);
  } // SortProcessByPID()


  public : void SortReadyQueueByPriority() {
    for (int a = 1; a < readyQueue.size(); a++)
      for (int b = 0; b < readyQueue.size() - a; b++)
        if (readyQueue[b]->priority > readyQueue[b + 1]->priority)
          swap(readyQueue[b], readyQueue[b + 1]);
  } // SortReadyQueueByPriority()


  public : void FCFS() {

    while(!AllProcessDone()) {
      PushArrivalProcessInReadyQueue(time);

      if(readyQueue.size() == 0) GanttChart += "-", time++;
      else {
        Process* execution = readyQueue[0];
        execution->status = "Execution";
        readyQueue.erase(readyQueue.begin());

        for(int a = 1; a <= execution->CPU_burst; a++) {
          PushArrivalProcessInReadyQueue(time);
          time++, GanttChart += execution->code;

          for(int b = 0; b < readyQueue.size(); b++) readyQueue[b]->waitingTime++;
        } // for(a)

        execution->status = "Done";
        execution->turnaroundTime = time - execution->arrivalTime;
      } // else
    } // while()
  } // FCFS()


  public : void RR() {
    while(!AllProcessDone()) {
      PushArrivalProcessInReadyQueue(time);

      if (readyQueue.size() == 0) GanttChart += "-", time++;
      else {
        Process* execution = readyQueue[0];
        execution->status = "Execution";
        readyQueue.erase(readyQueue.begin());

        for(int a = 0; a < timeSlice && execution->CPU_burst > 0; a++, execution->CPU_burst--) {
          PushArrivalProcessInReadyQueue(time);
          time++, GanttChart += execution->code;

          for(int b = 0; b < readyQueue.size(); b++) readyQueue[b]->waitingTime++;
        } // for(a)

        if (execution->CPU_burst == 0) {
          execution->status = "Done";
          execution->turnaroundTime = time - execution->arrivalTime;
        } // if()
        else {
          PushArrivalProcessInReadyQueue(time);
          execution->status = "Not Done Yet";
          readyQueue.push_back(execution);
        } // else
      } // else
    } // while()
  } // RR()


  public : void SJF() {
    while(!AllProcessDone()) {
      PushArrivalProcessInReadyQueue(time);

      if (readyQueue.size() == 0) GanttChart += "-", time++;
      else {
        int      index     = 0;
        Process* execution = readyQueue[0];
        
        for(int a = 1; a < readyQueue.size(); a++)
          if (execution->CPU_burst > readyQueue[a]->CPU_burst ||
              execution->CPU_burst == readyQueue[a]->CPU_burst && execution->arrivalTime > readyQueue[a]->arrivalTime) 
              execution = readyQueue[a], index = a;

        execution->status = "Execution";
        readyQueue.erase(readyQueue.begin() + index);

        for(int a = 0; a < execution->CPU_burst; a++) {
          PushArrivalProcessInReadyQueue(time);
          time++, GanttChart += execution->code;

          for(int b = 0; b < readyQueue.size(); b++) readyQueue[b]->waitingTime++;
        } // for(a)

        execution->status = "Done";
        execution->turnaroundTime = time - execution->arrivalTime;
      } // else
    } // while()
  } // SJF()


  public : void SRTF() {
    while(!AllProcessDone()) {
      PushArrivalProcessInReadyQueue(time);

      if (readyQueue.size() == 0) GanttChart += "-", time++;
      else {
        int      index     = 0;
        Process* execution = readyQueue[0];
        
        for(int a = 1; a < readyQueue.size(); a++)
          if (execution->CPU_burst > readyQueue[a]->CPU_burst ||
              execution->CPU_burst == readyQueue[a]->CPU_burst && execution->arrivalTime > readyQueue[a]->arrivalTime ||
              execution->CPU_burst == readyQueue[a]->CPU_burst && execution->arrivalTime == readyQueue[a]->arrivalTime && execution->pid > readyQueue[a]->pid) 
              execution = readyQueue[a], index = a;

        execution->status = "Execution";
        readyQueue.erase(readyQueue.begin() + index);

        time++, execution->CPU_burst--;
        GanttChart += execution->code;

        for(int a = 0; a < readyQueue.size(); a++) readyQueue[a]->waitingTime++;

        if (execution->CPU_burst == 0) {
          execution->status = "Done";
          execution->turnaroundTime = time - execution->arrivalTime;
        } // if()
        else {
          execution->status = "Not Done Yet";
          readyQueue.push_back(execution);
        } // else
      } // else
    } // while()
  } // SRTF()


  public : void HRRN() {
    while(!AllProcessDone()) {
      PushArrivalProcessInReadyQueue(time);

      if (readyQueue.size() == 0) GanttChart += "-", time++;
      else {
        int      index     = 0;
        Process* execution = readyQueue[0];

        for(int a = 0; a < readyQueue.size(); a++) readyQueue[a]->RenewResponseRatio();

        for(int a = 1; a < readyQueue.size(); a++)
          if (execution->responseRatio  < readyQueue[a]->responseRatio ||
              execution->responseRatio == readyQueue[a]->responseRatio && execution->arrivalTime  > readyQueue[a]->arrivalTime ||
              execution->responseRatio == readyQueue[a]->responseRatio && execution->arrivalTime == readyQueue[a]->arrivalTime && execution->pid > readyQueue[a]->pid) 
              execution = readyQueue[a], index = a;

        execution->status = "Execution";
        readyQueue.erase(readyQueue.begin() + index);

        for(int a = 0; a < execution->CPU_burst; a++) {
          PushArrivalProcessInReadyQueue(time);
          time++, GanttChart += execution->code;

          for(int b = 0; b < readyQueue.size(); b++) readyQueue[b]->waitingTime++;
        } // for(a)

        execution->status = "Done";
        execution->turnaroundTime = time - execution->arrivalTime;
      } // else
    } // while()
  } // HRRN()


  public : void PPRR() {
    while(!AllProcessDone()) {
      PushArrivalProcessInReadyQueue(time);

      if (readyQueue.size() > 1) SortReadyQueueByPriority();

      if (readyQueue.size() == 0) GanttChart += "-", time++;
      else {
        Process* execution = readyQueue[0];
        execution->status = "Execution";
        readyQueue.erase(readyQueue.begin());

        for(int a = 0; a < timeSlice && execution->CPU_burst > 0; a++) {
          time++, execution->CPU_burst--, GanttChart += execution->code;

          for(int b = 0; b < readyQueue.size(); b++) readyQueue[b]->waitingTime++;

          PushArrivalProcessInReadyQueue(time);
          SortReadyQueueByPriority();

          if (readyQueue[0]->priority < execution->priority) break;
        } // for(a)

        if (execution->CPU_burst == 0) {
          execution->status = "Done";
          execution->turnaroundTime = time - execution->arrivalTime;
        } // if()
        else {
          execution->status = "Not Done Yet";
          PushArrivalProcessInReadyQueue(time);
          readyQueue.push_back(execution);
        } // else
      } // else
    } // while()
  } // PPRR()


  public : bool AllProcessDone() {
    bool re = true;

    for (int a = 0; a < allProcess.size(); a++)
      if (allProcess[a]->status != "Done") return false;

    return true;
  } // AllProcessDone()


  public : void PushArrivalProcessInReadyQueue(int time) {
    for(int a = 0; a < allProcess.size(); a++) {
      if (allProcess[a]->arrivalTime == time && allProcess[a]->status == "Not Arrival") {
        allProcess[a]->status = "Ready Queue";
        readyQueue.push_back(allProcess[a]);
      } // if()
    } // for(a)
  } // PushArrivalProcessInReadyQueue()

}; // class Schedule


int ReadFile(vector<Schedule*>& allSchedule, ifstream& file);
char* StringTransToCharPointer(string s);
void PrintSchedule(vector<Schedule*>& allSchedule, int method, string fileName);


int main() {

  while(true) {
    string fileName;
    ifstream file;

    do {
      cout << "Please enter the file name (eg. input1, input1_method2) : ";

      getline(cin, fileName);
      file.open(StringTransToCharPointer(fileName + ".txt")); // try to open file

     if (!file) cout << "Input error, please try again.\n\n";
    } while(!file);

    vector<Schedule*> allSchedule;
    int method = ReadFile(allSchedule, file);

    if (method != 7) 
      allSchedule[method - 1]->Scheduling();
    else
      for (int a = 0; a < allSchedule.size(); a++)
        allSchedule[a]->Scheduling();

    PrintSchedule(allSchedule, method, fileName);
  } // while(true)
} // main()


char* StringTransToCharPointer(string s) {
  char* fileName = new char[s.length() + 1]; // string transfer to char[]
  for (int a = 0; a < s.length(); a++) fileName[a] = s[a];
  fileName[s.length()] = '\0';
  return fileName;
} // StringTransToCharStar()


int ReadFile(vector<Schedule*>& allSchedule, ifstream& file) {
  int method, timeSlice;
  string methodName;
  file >> method >> timeSlice;

  Schedule* s1 = new Schedule("FCFS", timeSlice);
  Schedule* s2 = new Schedule("RR",   timeSlice);
  Schedule* s3 = new Schedule("SJF",  timeSlice);
  Schedule* s4 = new Schedule("SRTF", timeSlice);
  Schedule* s5 = new Schedule("HRRN", timeSlice);
  Schedule* s6 = new Schedule("PPRR", timeSlice);

  string tittle;
  for (int a = 1; a <= 6; a++) file >> tittle; // skip tittle

  int pid, CPU_burst, arrivalTime, priority;

  while(file >> pid >> CPU_burst >> arrivalTime >> priority) {
    Process* p1 = new Process(pid, CPU_burst, arrivalTime, priority);
    Process* p2 = new Process(pid, CPU_burst, arrivalTime, priority);
    Process* p3 = new Process(pid, CPU_burst, arrivalTime, priority);
    Process* p4 = new Process(pid, CPU_burst, arrivalTime, priority);
    Process* p5 = new Process(pid, CPU_burst, arrivalTime, priority);
    Process* p6 = new Process(pid, CPU_burst, arrivalTime, priority);

    s1->allProcess.push_back(p1);
    s2->allProcess.push_back(p2);
    s3->allProcess.push_back(p3);
    s4->allProcess.push_back(p4);
    s5->allProcess.push_back(p5);
    s6->allProcess.push_back(p6);
  } // while()

  allSchedule.push_back(s1);
  allSchedule.push_back(s2);
  allSchedule.push_back(s3);
  allSchedule.push_back(s4);
  allSchedule.push_back(s5);
  allSchedule.push_back(s6);

  file.close(); // close file
  
  return method;
} // readFile()


void PrintSchedule(vector<Schedule*>& allSchedule, int method, string fileName) {
  ofstream file;
  file.open(StringTransToCharPointer("out_" + fileName + ".txt"));

  if      (method == 6) file << "Priority RR\n";
  else if (method != 7) file << allSchedule[method - 1]->name << "\n";
  else                  file << "All\n";

  if (method != 7) {
    file << "==        ";

    if      (allSchedule[method - 1]->name ==  "RR") file << "  ";
    else if (allSchedule[method - 1]->name == "SJF") file << " ";

    file << allSchedule[method - 1]->name       << "==\n";
    file << allSchedule[method - 1]->GanttChart << '\n';
  } // if()
  else { // method == 7, All
    for (int a = 0 ; a < allSchedule.size() ; a++) {
      file << "==        ";

      if      (allSchedule[a]->name ==  "RR") file << "  ";
      else if (allSchedule[a]->name == "SJF") file << " ";

      file << allSchedule[a]->name       << "==\n";
      file << allSchedule[a]->GanttChart << '\n';
    } // for(a)
  } // else

  file << "===========================================================\n\n";
  file << "Waiting Time\n";
  file << "ID";

  if (method != 7)
    file << '\t' << allSchedule[method - 1]->name;
  else // method == 7, All
    for(int a = 0; a < allSchedule.size(); a++)
      file << '\t' << allSchedule[a]->name;

  file << "\n===========================================================\n";

  if (method != 7) {
    for (int a = 0; a < allSchedule[0]->allProcess.size(); a++) {
      file << allSchedule[method - 1]->allProcess[a]->pid         << '\t'
           << allSchedule[method - 1]->allProcess[a]->waitingTime << '\n';
    } // for(a)
  } // if()
  else { // method == 7, All
    for (int a = 0; a < allSchedule[0]->allProcess.size(); a++) {
      file << allSchedule[0]->allProcess[a]->pid;

      for(int b = 0; b < allSchedule.size(); b++)
        file << '\t' << allSchedule[b]->allProcess[a]->waitingTime;

      file << '\n';
    } // for(a)
  } // else

  file << "===========================================================\n\n";
  file << "Turnaround Time\n";
  file << "ID";

  if (method != 7)
    file << '\t' << allSchedule[method - 1]->name;
  else // method == 7, All
    for(int a = 0; a < allSchedule.size(); a++)
      file << '\t' << allSchedule[a]->name;

  file << "\n===========================================================\n";

  if (method != 7) {
    for (int a = 0; a < allSchedule[0]->allProcess.size(); a++) {
      file << allSchedule[method - 1]->allProcess[a]->pid            << '\t'
           << allSchedule[method - 1]->allProcess[a]->turnaroundTime << '\n';
    } // for(a)
  } // if()
  else { // method == 7, All
    for (int a = 0; a < allSchedule[0]->allProcess.size(); a++) {
      file << allSchedule[0]->allProcess[a]->pid;

      for(int b = 0; b < allSchedule.size(); b++)
        file << '\t' << allSchedule[b]->allProcess[a]->turnaroundTime;

      file << '\n';
    } // for(a)
  } // else

  file << "===========================================================\n\n";
  file.close();
} // PrintSchedule()

