Shared Message Queues in Kernel Space

   Following project emulates the working of Message bus which is used as middleware in linux kernel for communication among user applications.

   For more information on message bus go to https://en.wikipedia.org/wiki/D-Bus

Getting Started

    These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 
    See deployment for notes on how to deploy the project on a live system.

Prerequisites

  Linux kernel (preferably linux 2.6.19 and above)
  GNU (preferably gcc 4.5.0 and above)

Installing

Download below files in user directory on your machine running linux distribution.

   1)msg_queue.c
   2)queue_ops.h
   3)Makefile
   4)squeue.c
   5)makefile

Ensure that 666(rw- rw- rw-) file permissions exist for /dev/input/mice.

you can check by the following command 
ls -lrt /dev/input/mice. 

Otherwise change using the following command 
sudo chmod 666 /dev/input/mice


Deployment

   Open the terminal & go to directory where files in installing part have been downloaded. [cd <Directory name>] 
   
   Create a folder with name "squeue" and inside it another folder named "user"
   
   In the "squeue" folder copy files "squeue.c" and "Makefile"

   In the "user" folder copy files "msg_queue.c" , "queue_ops.h" , "makefile"
   
   Use below command to to compile 

   Now go to squeue directory and run “make" command to compile "squeue.c"(for galileo use : make TEST_TARGET=Galileo2)

   run "sudo insmod squeue.ko" (or run on Galileo board after doing scp for running on Galileo)
   
   run "sudo chmod 666 /dev/datatqueue*"  (or run on Galileo board after doing scp for running on Galileo)
  
   now go to "user" folder and run "make" (for galileo use : make TEST_TARGET=Galileo2)

   Now run the object file using the command “./msg_queue”  (or run on Galileo board after doing scp for running on Galileo)
   

  this intiaties the execution of program code

Expected results

During runtime

   it displays the type of sender or receiver which is running currently

example:- 

  periodic sender 0 , aperiodic sender 1 etc.

After termination

   It displays the values of average queuing time , standard deviation of the queuing time.

Built With

  Linux 4.10.0-28-generic
  x86_64 GNU/Linux
  64 bit x86 machine

Authors

Sarvesh Patil 
Nagarjun chinnari 

License

This project is licensed under the ASU License

Acknowledgments

1)"http://www.2net.co.uk/tutorial/periodic_threads”- periodic tasks in linux
2)“https://www.mcs.anl.gov/~kazutomo/rdtsc.html”-time stamp counter

