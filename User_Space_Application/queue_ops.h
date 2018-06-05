//######################################################
//
// Project Title : Shared Message Queues in User Space
// Created by : Sarvesh Patil & Nagarjun Chinnari
// Date : 25 September 2017
// File : queue_ops.h
// 
//######################################################

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <semaphore.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <string.h>
#include <time.h>
#include <math.h>

//#define 1896359000 1900000000

//######################################################
//
// Queue functions Header
//
//######################################################

// Queue message description parameters
//--------------------------------------
struct q_msg_src  // Message Identifiers
{
    int t_type;
    int t_num;
};

struct q_msg  // Queue message body
{
    struct q_msg_src *src_ID;
    double PI_val;
    double q_start;
    double q_time;
}*q_msg_ID;


struct op_msg_node 
{
  //Output queue Linked list's current node Data
  struct q_msg *msg;

  //Output queue Linked list's next node pointer
  struct op_msg_node *next;
};
struct op_msg_node *head=NULL;

int msg_ID=0;
//=========================================
// Output Initialization
//=========================================

// Queue description parameters
//-------------------------------
#define MQ_NUM 2      //Number of message queues
#define QUEUE_LEN 10  //Size of each message queue
struct q_msg *dataqueue[MQ_NUM][QUEUE_LEN];

//pointers for traversing dataqueue
//----------------------------------
struct q_msg *(*wt_ptr0) = &dataqueue[0][0];
struct q_msg *(*wt_ptr1) = &dataqueue[1][0];
struct q_msg *(*rt_ptr0) = &dataqueue[0][0];
struct q_msg *(*rt_ptr1) = &dataqueue[1][0];

int wt_dq0=0, wt_dq1=0;
int rt_dq0=0, rt_dq1=0;

//========
// RDTSC
//========
#define base 1000
long double clk_f = 1896359000;
static __inline__ unsigned long long rdtsc(void)
{
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
}

//######################################################
//
// Queue functions area
//
//######################################################

//##########################
// Queue creation function
//##########################
int sq_create()
{

  for(int i=0; i<MQ_NUM; i++)
    for(int j=0;j<QUEUE_LEN; j++)
      dataqueue[MQ_NUM][QUEUE_LEN]=NULL;
  return 0;
}


//##########################
// Queue write function
//##########################
int q0_full_flag=0;
int q1_full_flag=0;
int sq_write(struct q_msg *new_msg, int new_enqueue_num)
{
  if(new_enqueue_num==0 && q0_full_flag==0)
  {
    dataqueue[0][wt_dq0]=new_msg; //add to queue
    wt_dq0++;
    wt_ptr0++;
    if(wt_dq0==10)
    {
      wt_dq0=0;
      wt_ptr0 = &dataqueue[0][0];
    }
    if(wt_dq0==rt_dq0)
    {
      printf("Dataqueue 0 is full\n");
      q0_full_flag=1;
    }
  }
  if(new_enqueue_num==1 && q1_full_flag==0)
  {
    dataqueue[1][wt_dq1]=new_msg; //add to queue
    wt_dq1++;
    wt_ptr1++;
    if(wt_dq1==10)
    {
      wt_dq1=0;
      wt_ptr1 = &dataqueue[1][0];
    }
    if(wt_dq1==rt_dq1)
    {
      printf("Dataqueue 1 is full\n");
      q1_full_flag=1;
    }
  }

  return 0;
}

//##########################
// Queue read function
//##########################
int overfl_chk=-1;
int sq_read()
{
  //Reading from 1st Dataqueue
 if(rt_dq0 < wt_dq0 || (rt_dq0==wt_dq0 && q0_full_flag==1))
 {
 while(rt_dq0 < wt_dq0 || (rt_dq0==wt_dq0 && q0_full_flag==1))
 {
  struct op_msg_node *new_msg_node = (struct op_msg_node*)malloc(sizeof(struct op_msg_node));  // New message read from dataqueue
  new_msg_node->msg=*rt_ptr0;
  new_msg_node->msg->q_time=(((rdtsc()-(new_msg_node->msg->q_start))/clk_f)*((long double) base)); //Evaluate queing time
  new_msg_node->next=NULL;
  //printf("val : %f \n", new_msg_node->msg->q_time );
  if(head == NULL)
  {
    head = new_msg_node;  // 1st node of output link list.
  }
  else
  {
    struct op_msg_node *end_seeker = head;  //To traverse output link list to find last node
    while(end_seeker->next != NULL)
    {
      end_seeker = end_seeker->next;  
    }
    
    end_seeker->next = new_msg_node; //last node of old list points to newly added node
  }
  rt_ptr0++;
  rt_dq0++;
 }
 }

 if(rt_dq0 > wt_dq0  || (rt_dq0==wt_dq0 && q0_full_flag==1))  //Run if write tracker has overflown & started again from initial position
 {
 overfl_chk=QUEUE_LEN+wt_dq0; 
 while(rt_dq0 <= overfl_chk && rt_dq0!=wt_dq0)
 {
  struct op_msg_node *new_msg_node = (struct op_msg_node*)malloc(sizeof(struct op_msg_node));  // New message read from dataqueue
  new_msg_node->msg=*rt_ptr0;
  new_msg_node->msg->q_time=(((rdtsc()-(new_msg_node->msg->q_start))/clk_f)*((long double) base)); //Evaluate queing time
  new_msg_node->next=NULL;
  //printf("val : %f \n", new_msg_node->msg->q_time );
  if(head == NULL)
  {
    head = new_msg_node;  // 1st node of output link list.
  }
  else
  {
    struct op_msg_node *end_seeker = head;  //To traverse output link list to find last node
    while(end_seeker->next != NULL)
    {
      end_seeker = end_seeker->next;  
    }
    
    end_seeker->next = new_msg_node; //last node of old list points to newly added node
  }
  rt_ptr0++;
  rt_dq0++;
  if(rt_dq0==10)
  {
    rt_dq0=0;
    rt_ptr0 = &dataqueue[0][0];
  }
 }
 }
 q0_full_flag=0;

  //Reading from 2nd Dataqueue
  if(rt_dq1 < wt_dq1 || (rt_dq1==wt_dq1 && q1_full_flag==1))
 {
 while(rt_dq1 < wt_dq1 || (rt_dq1==wt_dq1 && q1_full_flag==1))
 {
  struct op_msg_node *new_msg_node = (struct op_msg_node*)malloc(sizeof(struct op_msg_node));  // New message read from dataqueue
  new_msg_node->msg=*rt_ptr1;
  new_msg_node->msg->q_time=(((rdtsc()-(new_msg_node->msg->q_start))/clk_f)*((long double) base)); //Evaluate queing time
  new_msg_node->next=NULL;
  //printf("val : %f \n", new_msg_node->msg->q_time );
  if(head == NULL)
  {
    head = new_msg_node;  // 1st node of output link list.
  }
  else
  {
    struct op_msg_node *end_seeker = head;  //To traverse output link list to find last node
    while(end_seeker->next != NULL)
    {
      end_seeker = end_seeker->next;  
    }
    
    end_seeker->next = new_msg_node; //last node of old list points to newly added node
  }
  rt_ptr1++;
  rt_dq1++;
 }
 }

 if(rt_dq1 > wt_dq1  || (rt_dq1==wt_dq1 && q1_full_flag==1))  //Run if write tracker has overflown & started again from initial position
 {
 overfl_chk=QUEUE_LEN+wt_dq1; 
 while(rt_dq1 <= overfl_chk && rt_dq1!=wt_dq1)
 {
  struct op_msg_node *new_msg_node = (struct op_msg_node*)malloc(sizeof(struct op_msg_node));  // New message read from dataqueue
  new_msg_node->msg=*rt_ptr1;
  new_msg_node->msg->q_time=(((rdtsc()-(new_msg_node->msg->q_start))/clk_f)*((long double) base)); //Evaluate queing time
  new_msg_node->next=NULL;
  //printf("val : %f \n", new_msg_node->msg->q_time );
  if(head == NULL)
  {
    head = new_msg_node;  // 1st node of output link list.
  }
  else
  {
    struct op_msg_node *end_seeker = head;  //To traverse output link list to find last node
    while(end_seeker->next != NULL)
    {
      end_seeker = end_seeker->next;  
    }
    
    end_seeker->next = new_msg_node; //last node of old list points to newly added node
  }
  rt_ptr1++;
  rt_dq1++;
  if(rt_dq1==10)
  {
    rt_dq1=0;
    rt_ptr1 = &dataqueue[1][0];
  }
 }
 }
 q1_full_flag=0;

  return 0;
}


//##########################
// Queue delete function
//##########################
int sq_delete()
{
  printf("Delete started.\n");
  sq_read();
  struct op_msg_node *tracker = head;
  //To remove extra left click in end
  while(tracker->next->next != NULL)
  {
    tracker = tracker->next;
  }
  free(tracker->next);
  tracker->next=NULL;
  msg_ID--;
  

  //Final calculation
  double msg_cnt = 0;
  double total_queuing_time=0;
  double total_pi_sum=0;
  double standard_deviation_time=0;
  double mean_qtime;
  double standard_deviation_pi=0;
  double mean_PI;
  
  tracker = head;
    while(tracker != NULL)
    {
        msg_cnt++;
        total_queuing_time += tracker->msg->q_time;
        total_pi_sum += tracker->msg->PI_val;
        tracker = tracker->next;
    }
    mean_qtime=(total_queuing_time/msg_cnt);
    mean_PI=(total_pi_sum/msg_cnt);
    tracker = head;
    msg_cnt = 0;
    while(tracker != NULL)
    {
        msg_cnt++;
        standard_deviation_time+=(tracker->msg->q_time-mean_qtime)*(tracker->msg->q_time-mean_qtime);
        standard_deviation_pi += (tracker->msg->PI_val-mean_PI)*(tracker->msg->PI_val-mean_PI);
        tracker = tracker->next;
    }

    printf("=====================================================\n");
    printf("Total messages sent : %d\n", msg_ID);
    printf("Total messages received : %d\n", (int)msg_cnt);
    printf("=====================================================\n");
    printf("Average queuing time : %1.3f ms\n", (total_queuing_time/msg_cnt));
    printf("Average PI value : %1.20f\n", (total_pi_sum/msg_cnt));
    printf("=====================================================\n");
    printf("Standard Deviation of queuing time : %1.3f ms\n", sqrt(standard_deviation_time/msg_cnt));
    printf("Standard Deviation of PI sum : %1.20f\n", sqrt(standard_deviation_pi/msg_cnt) );
    printf("=====================================================\n");

  free(q_msg_ID);
  free(head);
  return 0;
}