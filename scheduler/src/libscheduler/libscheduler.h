/** @file libscheduler.h
 */

#ifndef LIBSCHEDULER_H_
#define LIBSCHEDULER_H_
/*  ZACH DEFINED GLOBALS: */
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

typedef struct _job_t
{
  int jobNumber;
  int arrivalTime;
  int startTime;
  int burstTime;
  int remainBurstTime;
  int priority;
  int virgin;
} job_t;
/**
  Constants which represent the different scheduling algorithms
*/
typedef enum {FCFS = 0, SJF, PSJF, PRI, PPRI, RR} scheme_t;

void  scheduler_start_up               (int cores, scheme_t scheme);
int   scheduler_new_job                (int job_number, int time, int running_time, int priority);
int   scheduler_job_finished           (int core_id, int job_number, int time);
int   scheduler_quantum_expired        (int core_id, int time);
float scheduler_average_turnaround_time();
float scheduler_average_waiting_time   ();
float scheduler_average_response_time  ();
void  scheduler_clean_up               ();

void  scheduler_show_queue             ();

#endif /* LIBSCHEDULER_H_ */
