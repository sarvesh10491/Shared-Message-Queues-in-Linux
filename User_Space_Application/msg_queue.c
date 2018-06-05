//######################################################
//
// Project Title : Shared Message Queues in User Space
// Created by : Sarvesh Patil & Nagarjun Chinnari
// Date : 25 September 2017
// File : msg_queue.c
// 
//######################################################

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <sched.h>
#include "queue_ops.h"


//######################################################
//
// Global variable & functions declarations area
//
//######################################################
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
//===============================
// Thread description parameters
//===============================

#define P_SENDER_THREAD 4         // Number of periodic sender pthreads
#define A_SENDER_THREAD 2         // Number of aperiodic sender pthreads
#define RECEIVER_THREAD 1         // Number of receiver pthreads
const int thread_type[]={0,1,2};  // Thread type identifiers

//===============================
// Thread scheduling parameters
//===============================
#define BASE_PERIOD 1000
long double p_start;
// Periodic intervals (In milliseconds)
const int P_PERIOD_MULTIPLIER[]={12,32,18,28};
const int R_PERIOD_MULTIPLIER[]={40};

struct periodic_info 
{
    int timer_fd;
    unsigned long long wakeups_missed;
};
struct periodic_info s_info[P_SENDER_THREAD];
struct periodic_info r_info[RECEIVER_THREAD];


static int make_periodic(unsigned int period, struct periodic_info *info)
{
    int ret;
    unsigned int ns;
    unsigned int sec;
    int fd;
    struct itimerspec itval;

    // Create the timer 
    fd = timerfd_create(CLOCK_MONOTONIC, 0);
    info->wakeups_missed = 0;
    info->timer_fd = fd;
    if (fd == -1)
        return fd;

    // Make the timer periodic
    sec = period / 1000000;
    ns = (period - (sec * 1000000)) * 1000;
    itval.it_interval.tv_sec = sec;
    itval.it_interval.tv_nsec = ns;
    itval.it_value.tv_sec = sec;
    itval.it_value.tv_nsec = ns;
    ret = timerfd_settime(fd, TFD_TIMER_ABSTIME, &itval, NULL);
    return ret;
}

static void wait_period(struct periodic_info *info)
{
    unsigned long long missed;
    int ret;

    // Wait for the next timer event. 
    //If we have missed any the number is written to "missed" 
    ret = read(info->timer_fd, &missed, sizeof(missed));
    if (ret == -1) 
    {
        perror("Timer read error");
        return;
    }

    info->wakeups_missed += missed;
}

//==============================
// Thread priority attributes
//==============================
#define P_SENDER_PRIO 40
#define A_SENDER_PRIO 60
#define RECIVER_PRIO  20

//========================
// Semaphore declarations
//========================
sem_t wt_sem;

//====================
// Mouse declarations
//====================
int mouse();
int mouse_read;
const char *mouse_device = "/dev/input/mice"; // Decive file path to read mice events

int fd_m, mouse_bytes;
unsigned char mouse_data[3];

long double l_start=0, L_flag=0;
int left=0, right=0;

//=========================
// PI calculation function
//=========================
double pi_cal();

//######################################################
//
// Thread functions
//
//######################################################

// Thread function for sender threads
//------------------------------------
void *s_thread_func(void *vptr)
{
  struct q_msg_src *tdata = vptr; //Pointer to capture thread details that invoked sender thread function

  while(mouse_read!=3)
  {
    int enqueue_num=-1;
    // Below checks matches thread type of calling thread everytime any thread invokes sender function.
    // Determines which thread has called & runs sq_write() routine for same.

    //Periodic thread part
    //======================
    if(tdata->t_type == thread_type[0])
    {
     //For Periodic sender thread 1
     if(tdata->t_num==0)
     {
      printf("Periodic sender thread : %d\n",tdata->t_num );
      sem_wait(&wt_sem);
      q_msg_ID=malloc(sizeof(struct q_msg));
      enqueue_num=0;
      q_msg_ID->src_ID=tdata;
      msg_ID++;
      q_msg_ID->PI_val=pi_cal();
      q_msg_ID->q_start=rdtsc();   
      sq_write(q_msg_ID,enqueue_num);
      sem_post(&wt_sem);      
      wait_period(&s_info[tdata->t_num]);
     }
     
     //For Periodic sender thread 2
     if(tdata->t_num==1)
     {
      printf("Periodic sender thread : %d\n",tdata->t_num );
      sem_wait(&wt_sem);
      q_msg_ID=malloc(sizeof(struct q_msg));
      enqueue_num=0;
      q_msg_ID->src_ID=tdata;
      msg_ID++;
      q_msg_ID->PI_val=pi_cal();
      q_msg_ID->q_start=rdtsc();
      sq_write(q_msg_ID,enqueue_num);
      sem_post(&wt_sem);
      wait_period(&s_info[tdata->t_num]);
     }

     //For Periodic sender thread 3
     if(tdata->t_num==2)
     {
      printf("Periodic sender thread : %d\n",tdata->t_num );
      sem_wait(&wt_sem);
      q_msg_ID=malloc(sizeof(struct q_msg));
      enqueue_num=1;
      q_msg_ID->src_ID=tdata;
      msg_ID++;
      q_msg_ID->PI_val=pi_cal();
      q_msg_ID->q_start=rdtsc();
      sq_write(q_msg_ID,enqueue_num);
      sem_post(&wt_sem);
      wait_period(&s_info[tdata->t_num]);
     }

     //For Periodic sender thread 4
     if(tdata->t_num==3)
     {
      printf("Periodic sender thread : %d\n",tdata->t_num );
      sem_wait(&wt_sem);
      q_msg_ID=malloc(sizeof(struct q_msg));
      enqueue_num=1;
      q_msg_ID->src_ID=tdata;
      msg_ID++;
      q_msg_ID->PI_val=pi_cal();
      q_msg_ID->q_start=rdtsc();
      sq_write(q_msg_ID,enqueue_num);
      sem_post(&wt_sem);
      wait_period(&s_info[tdata->t_num]);
     }
    }

    //Aperiodic thread part   
    //----------------------                                                                                                                                                                                                                                                                                          
     if(mouse_read==1) //Left click
     {
      //Aperiodic sender 0
      mouse_read=0;
      sem_wait(&wt_sem);
      q_msg_ID=malloc(sizeof(struct q_msg));
      tdata->t_type = thread_type[1];
      tdata->t_num=0;
      enqueue_num=0;
      q_msg_ID->src_ID=tdata;
      msg_ID++;
      q_msg_ID->PI_val=pi_cal();
      q_msg_ID->q_start=rdtsc();
      sq_write(q_msg_ID,enqueue_num);
      printf("Aperiodic sender thread : %d\n",tdata->t_num );
      sem_post(&wt_sem);
     }

     if(mouse_read==2) //Right click
     {
      //Aperiodic sender 1
      mouse_read=0;
      sem_wait(&wt_sem);
      q_msg_ID=malloc(sizeof(struct q_msg));
      tdata->t_type = thread_type[1];
      tdata->t_num=1;
      enqueue_num=1;
      q_msg_ID->src_ID=tdata;
      msg_ID++;
      q_msg_ID->PI_val=pi_cal();
      q_msg_ID->q_start=rdtsc();
      sq_write(q_msg_ID,enqueue_num);
      printf("Aperiodic sender thread : %d\n",tdata->t_num );
      sem_post(&wt_sem);
     }
    }
    printf("Sender thread out\n");
    pthread_exit(NULL);

  return NULL;
}


// Thread function for receiver threads
//---------------------------------------
void *r_thread_func(void *vptr)
{
  struct q_msg_src *tdata = vptr;

  while(mouse_read!=3)
  {
    if(tdata->t_type == thread_type[2])
    {
     if(tdata->t_num==0)
     {
        printf("Receiver thread : %d\n",tdata->t_num );
        sq_read();
        wait_period(&r_info[tdata->t_num]);
     }
    }
  }
  printf("Receiver thread out\n");
  pthread_exit(NULL);

  return NULL;
}


//######################################################
//
// Main
//
//######################################################


int main(int argc, char const *argv[])
{
  //=========================================
  // Thread Initializations
  //=========================================
  int tret;

  pthread_t P_tid[P_SENDER_THREAD];
  pthread_t A_tid[A_SENDER_THREAD];
  pthread_t R_tid[RECEIVER_THREAD];

  long P_tidlst[P_SENDER_THREAD];
  long A_tidlst[A_SENDER_THREAD];
  long R_tidlst[RECEIVER_THREAD];

  pthread_attr_t fifo_attr;
  pthread_attr_init(&fifo_attr);
  pthread_attr_setschedpolicy(&fifo_attr,SCHED_FIFO);
  struct sched_param param;

  //=========================================
  // Semaphore Initialization
  //=========================================
  sem_init(&wt_sem, 0, 1);

  //=========================================
  // Timer Initialization
  //=========================================
  for(int i=0;i<P_SENDER_THREAD;i++)
  {
    make_periodic(P_PERIOD_MULTIPLIER[i]*BASE_PERIOD, &s_info[i]);
  }
  for(int i=0;i<RECEIVER_THREAD;i++)
  {
    make_periodic(R_PERIOD_MULTIPLIER[i]*BASE_PERIOD, &r_info[i]);
  }
  
  //=========================================
  // Mouse Initialization
  //=========================================
  fd_m = open(mouse_device, O_RDONLY);
  if(fd_m == -1)
  {
      printf("ERROR Opening mouse device. %s\n", mouse_device);
      return -1;
  }
  
  //=========================================
  // Create Message queue;
  //=========================================
  sq_create();

  //=========================================
  // Set CPU affinity
  //=========================================
  // Bitset where each bit represents a CPU.
  cpu_set_t cpuset;
  // Initializes the CPU set to be the empty set.
  CPU_ZERO(&cpuset);
  // CPU_SET: This macro adds cpu to the CPU set.
  CPU_SET(0, &cpuset);


  //=========================================
  // Threads creation
  //=========================================
  struct q_msg_src *sptr;
  sptr=(struct q_msg_src *)malloc((P_SENDER_THREAD+A_SENDER_THREAD+RECEIVER_THREAD)*sizeof(struct q_msg_src));
  struct q_msg_src *tmpsptr;
  tmpsptr=sptr;


  //Creating Periodic Sender threads
  //----------------------------------
  param.sched_priority = P_SENDER_PRIO;
  pthread_attr_setschedparam(&fifo_attr, &param);

  for(int i=0; i<P_SENDER_THREAD; i++)
  {
    P_tidlst[i]=i;  //To make it thread safe
                    //ensuring each thread's argument remains intact throughout code
    sptr->t_type=thread_type[0];
    sptr->t_num=P_tidlst[i];

    tret=pthread_create(&P_tid[i],&fifo_attr,s_thread_func,(void *)sptr);
    pthread_setaffinity_np(P_tid[i], sizeof(cpu_set_t), &cpuset);
    if (tret)
    {
      printf("ERROR. Return code from pthread_create() is %d\n", tret);
      exit(-1);
    }
    sptr++;
  }


  //Creating Aperiodic Sender threads
  //-----------------------------------
  param.sched_priority = A_SENDER_PRIO;
  pthread_attr_setschedparam(&fifo_attr, &param);
  for(int i=0; i<A_SENDER_THREAD; i++)
  {
    A_tidlst[i]=i;  //To make it thread safe
                    //ensuring each thread's argument remains intact throughout code
    sptr->t_type=thread_type[1];
    sptr->t_num=A_tidlst[i];

    tret=pthread_create(&A_tid[i],&fifo_attr,s_thread_func,(void *)sptr);
    pthread_setaffinity_np(A_tid[i], sizeof(cpu_set_t), &cpuset);
    if (tret)
    {
      printf("ERROR. Return code from pthread_create() is %d\n", tret);
      exit(-1);
    }
    sptr++;
  }


  //Creating Receiver threads
  //--------------------------
  param.sched_priority = RECIVER_PRIO;
  pthread_attr_setschedparam(&fifo_attr, &param);
  for(int i=0; i<RECEIVER_THREAD; i++)
  {
    R_tidlst[i]=i;  //To make it thread safe
                    //ensuring each thread's argument remains intact throughout code
    sptr->t_type=thread_type[2];
    sptr->t_num=R_tidlst[i];

    tret=pthread_create(&R_tid[i],&fifo_attr,r_thread_func,(void *)sptr);
    pthread_setaffinity_np(R_tid[i], sizeof(cpu_set_t), &cpuset);
    if (tret)
    {
      printf("ERROR. Return code from pthread_create() is %d\n", tret);
      exit(-1);
    }
    sptr++;
  }
  
  //=========================================
  // Mouse polling
  //=========================================
  while(mouse_read!=3)
  {
    mouse();
  }
  printf("out of main\n");
  
  //=========================================
  // Threads termination
  //=========================================
  
  for(int i=0; i<P_SENDER_THREAD; i++)
    pthread_join(P_tid[i],NULL);
  for(int i=0; i<A_SENDER_THREAD; i++)
    pthread_join(A_tid[i],NULL);
  for(int i=0; i<RECEIVER_THREAD; i++)
    pthread_join(R_tid[i],NULL);
  sq_delete();
  
  free(tmpsptr);
  printf("Terminated successfully.\n");

  return 0;
}

//######################################################
//
// End of Main
//
//######################################################



//######################################################
//
// Mouse Read function
//
//######################################################
int mouse()
{   
    //while(1)
    {
        // Read Mouse     
        mouse_bytes = read(fd_m, mouse_data, sizeof(mouse_data));

        if(mouse_bytes > 0)
        {
            left = mouse_data[0] & 0x1;
            right = mouse_data[0] & 0x2;

            if(right > 0)
            {
                //printf("Right\n");
                L_flag=0;
                mouse_read=2;
            }

            if(left > 0)
            {
                if(L_flag == 0)  //previous click was Right/Double click
                {
                    //printf("Left\n");
                    mouse_read=1;
                    L_flag=1;
                    l_start=rdtsc();
                }
                else if(L_flag == 1) //previous click was left
                {
                    if((((rdtsc()-l_start)/clk_f)*((long double) base)) < 500) //two left clicks happened inside 500us interval
                    {
                        mouse_read=3;
                        //printf("Double left\n");
                    }
                    else //two left clicks happened outside 500us interval
                    {
                        mouse_read=1;
                        //printf("Left\n");
                    } 
                    l_start=rdtsc();
                }
            }
        }
        else
            mouse_read=0;  // nothing detected from mouse
    } 
    return 0;
}

//######################################################
//
// PI calculations
//
//######################################################
double pi_cal()
{
   double pi_n;     // Number of iterations and control variables
   double pi_f;           // factor that repeats
   double pi = 1;

   time_t t_obj;
   srand((unsigned) time(&t_obj));

   pi_n=rand()%50;  //To pick random value for iterations between 10 - 50
   if(pi_n<10)
   {
      pi_n=pi_n+10;
   }
    
   for(int i = pi_n; i > 1; i--) 
   {
      pi_f = 2;
      for(int j = 1; j < i; j++)
      {
         pi_f=2+sqrt(pi_f);
      }
      pi_f = sqrt(pi_f);
      pi = pi * pi_f / 2;
   }
   pi *= (sqrt(2)/2);
   pi = 2/pi;

   //printf("\nAproximated value of PI = %1.16lf\n", pi);  

   return pi;
}

//######################################################
//
// End of msg_queue.c
//
//######################################################
