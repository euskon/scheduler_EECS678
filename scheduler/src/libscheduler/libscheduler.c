/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"
#include <stdbool.h>

/** @GLOBALS: 
  int currTime = 0;
  int totalTurnaround = 0;
  int totalWait = 0;
  int totalResponse = 0;
  int totalJobs = 0;
  //GLOBAL QUEUE FOR USE IN SCHEDULING
  priqueue_t* readyQueue;
  //Array of CPU cores to run current job;
  job_t** arr_Cores;
  int num_Cores;
  scheme_t schem_Curr;
*/
int currTime = 0;
int totalTurnaround = 0;
int totalWait = 0;
int totalResponse = 0;
int totalJobs = 0;
/**
  Stores information making up a job to be scheduled including any statistics.
  You may need to define some global variables or a struct to store your job queue elements. 
*/
// typedef struct _job_t
// {
//   int jobNumber;
//   int arrivalTime;
//   int startTime;
//   int burstTime;
//   int remainBurstTime;
//   int priority;
//   int virgin;
// } job_t;

/**
  @return: return 1 if newJob is higher priority based on earlier arrivalTime
  -1 if newJob->arrivalTime < jobInQ->arrivalTime
   0 if newJob->arrivalTime == jobInQ->arrivalTime
   1 if newJob->arrivalTime > jobInQ->arrivalTime
  this will allow for comparison in while loop based on return from this function 
  while its necessary to continue traversing further into the PQ past the higher priority
  jobs. For this specific scheme this could be done by just putting the new job at the end.
*/
int fcfs(const void* newJob, const void* jobInQ){
  if(((job_t*)newJob)->arrivalTime < ((job_t*)jobInQ)->arrivalTime){
    return -1;
  }
  else if(((job_t*)newJob)->arrivalTime > ((job_t*)jobInQ)->arrivalTime){
    return 1;
  }
  else{
    return 0;
  }
}
/**
  @return: return 1 if newJob is higher priority based on shorter burstTime
  -1 newJob->burstTime < jobInQ->burstTime
   0 if newJob->burstTime == jobInQ->burstTime 
   1 if newJob->burstTime > jobInQ->burstTime 
  this will allow for comparison in while loop based on return from this function 
  while its necessary to continue traversing further into the PQ past the higher priority
  jobs. 
*/
int sjf(const void* newJob, const void* jobInQ){
  if(((job_t*)newJob)->burstTime < ((job_t*)jobInQ)->burstTime){
    return -1;
  }
  else if(((job_t*)newJob)->burstTime > ((job_t*)jobInQ)->burstTime){
    return 1;
  }
  else{
    return 0;
  }
}
/**
  @return: return 1 if newJob is higher priority based on shorter remainBurstTime
  -1 newJob->remainBurstTime < jobInQ->remainBurstTime
   0 if newJob->remainBurstTime == jobInQ->remainBurstTime 
   1 if newJob->remainBurstTime > jobInQ->remainBurstTime 
  this will allow for comparison in while loop based on return from this function 
  while its necessary to continue traversing further into the PQ past the higher priority
  jobs. This can use remainBurstTime for both the job that is already in the Queue and 
  the newJob because newJob->burstTime === newJob->remainBurstTime
*/
int psjf(const void* newJob, const void* jobInQ){
  if(((job_t*)newJob)->remainBurstTime < ((job_t*)jobInQ)->remainBurstTime){
    return -1;
  }
  else if(((job_t*)newJob)->remainBurstTime > ((job_t*)jobInQ)->remainBurstTime){
    return 1;
  }
  else{
    return 0;
  }
}
/**
  @return: return 1 if newJob is higher priority based on SMALLEST priority
  -1 newJob->priority > jobInQ->priority
   0 if newJob->priority == jobInQ->priority 
   1 if newJob->priority < jobInQ->priority 
*/
int pri(const void* newJob, const void* jobInQ){
  if(((job_t*)newJob)->priority > ((job_t*)jobInQ)->priority){
    return -1;
  }
  else if(((job_t*)newJob)->priority < ((job_t*)jobInQ)->priority){
    return 1;
  }
  else{
    return 0;
  }
}
/**
  @return: return 1 if newJob is higher priority based on SMALLEST priority value
   0 if newJob->priority >= jobInQ->priority //imposes fcfs
   1 if newJob->priority < jobInQ->priority 
*/
int ppri(const void* newJob, const void* jobInQ){
  if(((job_t*)newJob)->priority > ((job_t*)jobInQ)->priority){
    return -1;
  }
  else if(((job_t*)newJob)->priority < ((job_t*)jobInQ)->priority){
    return 1;
  }
  else{
    return 0;
  }
}
//update remaining time of each active job within all cores
void timeSync(int newTime){
  for(int i = 0; i < num_Cores; i++){
    if(arr_Cores[i] != NULL){
      arr_Cores[i]->remainBurstTime -= (newTime - currTime);
    }
  }
  currTime = newTime;
}

/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  num_Cores = cores;
  schem_Curr = scheme;
  arr_Cores = malloc(num_Cores * sizeof(job_t*));
  for(int i = 0; i < num_Cores; i++){
    arr_Cores[i] = NULL;
  }
  readyQueue = malloc(sizeof(priqueue_t));

  switch(schem_Curr){
    case FCFS:
          priqueue_init(readyQueue, &fcfs);
          break;
    case SJF:
          priqueue_init(readyQueue, &sjf);
          break;
    case PSJF:
          priqueue_init(readyQueue, &psjf);
          break;
    case PRI:
          priqueue_init(readyQueue, &pri);
          break;
    case PPRI:
          priqueue_init(readyQueue, &ppri);
          break;
    case RR:
          //purposely used fcfs
          priqueue_init(readyQueue, &fcfs);
          break;
  }
}


/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumption:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */
bool isPreemptive();
int getCoreToPreempt();
int findEmptyCore();
int putJobInCore(int core_id, job_t* new_job);

int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  job_t* new_job = malloc(sizeof(job_t));
  new_job -> jobNumber = job_number;
  new_job -> arrivalTime = time;
  //must set this if the new job is going straight to a core to run 
  //i.e. core was empty beforehand or preempting process already running
  new_job -> startTime = -1;
  new_job -> burstTime = running_time;
  new_job -> remainBurstTime = running_time;
  new_job -> priority = priority;
  new_job -> virgin = 1;

  totalJobs++;
  if (isPreemptive())
  {
    new_job->startTime = time;
    return putJobInCore(getCoreToPreempt(new_job), new_job);
  }
  else
  { 
    int core = findEmptyCore();
    if(core != -1)
    {
      new_job->startTime = time;
      putJobInCore(core, new_job);
      return core;
    }
    priqueue_offer(readyQueue, new_job);
    return -1;
  }
}

/*
Get the empty core with the lowest id. If no cores are empty, return -1.
*/
int findEmptyCore(){
  for(int i = 0; i < num_Cores; i++){
    if(arr_Cores[i]==NULL){
      return i;
    }
  }
  return -1;
}

/*
  Simple utility function to get whether or not a certain algorithim may be preemptive.
*/
bool isPreemptive()
{
  if (schem_Curr == PSJF)
  {
    return true;
  }
  return false;
}

/*
  Return the core id of the core with the highest remaining burst time.
*/
int getCoreToPreempt(job_t* new_job)
{
  int greatest_burst = 0;
  int greatest_id = -1;
  for (int i = 0; i < num_Cores; i++)
  {
    if (arr_Cores[i] != NULL && (greatest_burst < arr_Cores[i]->burstTime))
    {
      greatest_id = i;
      greatest_burst = arr_Cores[i]->burstTime;
    }
  }
  if (new_job->burstTime < greatest_burst)
  {
    return greatest_id;
  }
  else
  {
    return -1;
  }
}

/*
Puts a job in a core. 
If the core is not empty, the job on the core will be offered to the ready queue.
If the core number is -1, the job will be offered to the ready queue.
*/
int putJobInCore(int core_id, job_t* new_job)
{
  if (core_id == -1)
  {
    priqueue_offer(readyQueue, new_job);
    return -1;
  }
  if (arr_Cores[core_id] != NULL)
  {
    priqueue_offer(readyQueue, arr_Cores[core_id]);
  }
  new_job->virgin = 0;
  arr_Cores[core_id] = new_job;
  return core_id;
}
/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
  totalWait += (time - arr_Cores[core_id]->burstTime - arr_Cores[core_id]->arrivalTime);
  totalTurnaround += (time - arr_Cores[core_id]->arrivalTime);
  totalResponse += (arr_Cores[core_id]->startTime - arr_Cores[core_id]->arrivalTime);
  job_t* frontJob = (job_t*)priqueue_poll(readyQueue);
  job_t* terminatedJob = arr_Cores[core_id];
  free(terminatedJob); 
  if(frontJob != NULL){
    //check if process that is going into core is virgin
    if(frontJob->virgin){
      frontJob->virgin = 0;//no longer virgin because it going to run now
      frontJob->startTime = time;//set start time of job entering core to run
    }
    arr_Cores[core_id] = frontJob;
    return frontJob->jobNumber;
  }
  else{
    arr_Cores[core_id] = NULL;
    return -1;
  }
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return (((float)totalWait) / ((float)totalJobs));
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return (((float)totalTurnaround) / ((float)totalJobs));
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return (((float)totalResponse) / ((float)totalJobs));
}


/**
  Free any memory associated with your scheduler.
 
  Assumption:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  for(int i = 0; i < num_Cores; i++){
    arr_Cores[i] = NULL;
  }
  free(arr_Cores);
  priqueue_destroy(readyQueue);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
  printf("CORES: \n");
  for (int i = 0; i < num_Cores; i++)
  {
    if (arr_Cores[i] != NULL)
    {
      printf("  - %d: %d\n", i, arr_Cores[i]->jobNumber);
    }
    else
    {
      printf("  - %d: EMPTY\n", i);
    }
  }
  printf("PRIORITY QUEUE: \n");
  for (int i = 0; i < priqueue_size(readyQueue); i++)
  {
    job_t* display = priqueue_at(readyQueue, i);
    printf("  - [%d] \n", display->jobNumber);
  }
}

void scheduler_cores_and_queue()
{
  printf("CORES: \n");
  for (int i = 0; i < num_Cores; i++)
  {
    if (arr_Cores[i] != NULL)
    {
      printf("  - %d: %d\n", i, arr_Cores[i]->jobNumber);
    }
    else
    {
      printf("  - %d: EMPTY\n", i);
    }
  }
  printf("PRIORITY QUEUE: \n");
  for (int i = 0; i < priqueue_size(readyQueue); i++)
  {
    job_t* display = priqueue_at(readyQueue, i);
    printf("  - [%d] \n", display->jobNumber);
  }
}