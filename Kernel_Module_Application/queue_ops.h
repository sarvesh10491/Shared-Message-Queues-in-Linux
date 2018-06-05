//######################################################
//
// Project Title : Shared Message Queues in User Space
// Created by : Sarvesh Patil & Nagarjun Chinnari
// Date : 2 October 2017
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

// Queue devices parameters
//-------------------------------
int fd1, fd2;
const char *DEVICE_FILE_1 = "/dev/dataqueue1";
const char *DEVICE_FILE_2 = "/dev/dataqueue2";


char rec_msg_buf[256];

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
  fd1 = open(DEVICE_FILE_1, O_RDWR);
  fd2 = open(DEVICE_FILE_2, O_RDWR);

  if (fd1 < 0)
  {
    perror("Failed to open the device dataqueue1.");
    return errno;
  }
  if (fd2 < 0)
  {
    perror("Failed to open the device dataqueue2.");
    return errno;
  }

  return 0;
}


//##########################
// Queue write function
//##########################

int wret1, wret2;
int sq_write(struct q_msg *new_msg, int new_enqueue_num)
{
  //sprintf(new_msg_buf,"%d %d %d %d %1.20lf %f %f ",new_enqueue_num,msg_ID,new_msg->src_ID->t_type,new_msg->src_ID->t_num,new_msg->PI_val,new_msg->q_start,new_msg->q_time);
  //sprintf(new_msg_buf,"%s",(const char *)new_msg);
  //printf("%s\n", new_msg_buf);
  if(new_enqueue_num==0)
  {
   wret1 = write(fd1, (const char *)new_msg, sizeof(struct q_msg)); // Send the pointer to the LKM
   if (wret1 < 0)
   {
      perror("Failed to write the message to the device dataqueue1.");
      return errno;
   }
  }
  else if(new_enqueue_num==1)
  {
   wret2 = write(fd2, (const char *)new_msg, sizeof(struct q_msg)); // Send the pointer to the LKM
   if (wret2 < 0)
   {
      perror("Failed to write the message to the device dataqueue2.");
      return errno;
   }
  }

  return 0;
}

//##########################
// Queue read function
//##########################
int sq_read(int k)
{
int rret1=1;
int rret2=1;
if(k==1)
{
 while(rret1> 0)
{
  rec_msg_buf[0]='a';
  rret1 = read(fd1, rec_msg_buf, sizeof(rec_msg_buf));        // Read the response from the LKM
  //printf("rret1 is %d\n", rret1);

  struct op_msg_node *new_msg_node = (struct op_msg_node*)malloc(sizeof(struct op_msg_node));  // New message read from dataqueue
  new_msg_node->msg=(struct q_msg *)rec_msg_buf;

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
  if (rret1 < 0)
  {
    perror("Failed to read the message from the device dataqueue1.");
    return errno;
  }
}
}
if(k==2)
{
 
while(rret2 > 0)
{
  rec_msg_buf[0]='b';
  rret2 = read(fd2, rec_msg_buf, sizeof(rec_msg_buf));        // Read the response from the LKM
  //printf("rret2 is %d\n", rret2);

  struct op_msg_node *new_msg_node = (struct op_msg_node*)malloc(sizeof(struct op_msg_node));  // New message read from dataqueue
  
  new_msg_node->msg=(struct q_msg *)rec_msg_buf;

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
  if (rret2 < 0)
  {
    perror("Failed to read the message from the device dataqueue2.");
    return errno;
  }
}

 }
  return 0;
}


//##########################
// Queue delete function
//##########################
int sq_delete()
{
  printf("Delete started.\n");
  sq_read(1);
  sq_read(2);
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
  //double total_pi_sum=0;
  double standard_deviation_time=0;
  double mean_qtime;
  //double standard_deviation_pi=0;
  //double mean_PI;
  
  tracker = head;
    while(tracker != NULL)
    {
        msg_cnt++;
        total_queuing_time += tracker->msg->q_time;
        //total_pi_sum += tracker->msg->PI_val;
        tracker = tracker->next;
    }
    mean_qtime=(total_queuing_time/msg_cnt);
    //mean_PI=(total_pi_sum/msg_cnt);
    tracker = head;
    msg_cnt = 0;
    while(tracker != NULL)
    {
        msg_cnt++;
        standard_deviation_time+=(tracker->msg->q_time-mean_qtime)*(tracker->msg->q_time-mean_qtime);
        //standard_deviation_pi += (tracker->msg->PI_val-mean_PI)*(tracker->msg->PI_val-mean_PI);
        tracker = tracker->next;
    }

    printf("=====================================================\n");
    //printf("Total messages sent : %d\n", msg_ID);
    printf("Total messages received : %d\n", (int)msg_cnt);
    printf("=====================================================\n");
    printf("Average queuing time : %1.3f ms\n", (total_queuing_time/msg_cnt));
    //printf("Average PI value : %1.20f\n", (total_pi_sum/msg_cnt));
    printf("=====================================================\n");
    printf("Standard Deviation of queuing time : %1.3f ms\n", sqrt(standard_deviation_time/msg_cnt));
    //printf("Standard Deviation of PI sum : %1.20f\n", sqrt(standard_deviation_pi/msg_cnt) );
    printf("=====================================================\n");

 free(q_msg_ID);
  free(head);
  return 0;
}