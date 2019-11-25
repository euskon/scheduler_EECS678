/** @file libscheduler.h
 */

#ifndef LIBSCHEDULER_H_
#define LIBSCHEDULER_H_
//#include "../libpriqueue/libpriqueue.h"
typedef struct _priqueue_t priqueue_t;
/*  ZACH DEFINED GLOBALS: */


//GLOBAL QUEUE FOR USE IN SCHEDULING
priqueue_t* readyQueue;
//Array of CPU cores to run current job;
int num_Cores;

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

job_t** arr_Cores;
/**
  Constants which represent the different scheduling algorithms
*/
typedef enum {FCFS = 0, SJF, PSJF, PRI, PPRI, RR} scheme_t;
scheme_t schem_Curr;

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
