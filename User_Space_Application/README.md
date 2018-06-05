Shared Message Queues in User Space
====================================

   Following project emulates the working of Message bus which is used as middleware in linux kernel for communication among user applications.

   For more information on message bus go to https://en.wikipedia.org/wiki/D-Bus

Getting Started
================

    These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 
    See deployment for notes on how to deploy the project on a live system.

Prerequisites
=============

  Linux kernel (preferably linux 2.6.19 and above)
  GNU (preferably gcc 4.5.0 and above)
  64 bit x86 machine

Installing
===========

Download below files in user directory on your machine running linux distribution.

   1) msg_queue.c
   2) queue_ops.h
   3) makefile
   4) message_queue.sh

Ensure that 666(rw- rw- rw-) file permissions exist for /dev/input/mice.

you can check by the following command 
ls -lrt /dev/input/mice. 

Otherwise change using the following command 
sudo chmod 666 /dev/input/mice


Deployment
===========
   Run following set of commands in your terminal window.
   1] cd "<Directory where all files in Installing part have been downloaded.>"
   2] chmod 755 message_queue.sh
   3] sh message_queue.sh

This initiates the execution of program code
Termination sequence happens on mouse double click.
make related output is captured in /tmp/makelog.lst
    

(OR ALTERNATIVELY RUN USING MAKE FILE) 

   Open the terminal & go to directory where files in installing part have been downloaded. [cd <Directory name>] 

   Use below commands to compile :
   
   1] Update queue_ops.h with clock frequency of your terminal in Hertz. Same can be found by running following in terminal window.
      val=`cat /proc/cpuinfo |grep "cpu MHz"|uniq|cut -d ":" -f2`
      multi=1000000
      clk_freq=$(echo "${val}*${multi}" |bc)
      echo $clk_freq

   2] Run “make” command to compile msg_queue.c 

   3] If there is no error then a object file under the name msg_queue will be created. 

   4] Now run the object file using the command “./msg_queue”
   

This initiates the execution of program code

Expected results
=================

During runtime

   it displays the type of sender or receiver which is running currently

example:- 

  periodic sender 0 , aperiodic sender 1 etc.

After termination

   It displays the values of average queuing time , standard deviation of the queuing time , average value of pi , standard deviation of the pi.


Built With
===========
  Linux 4.10.0-28-generic
  x86_64 GNU/Linux
  64 bit x86 machine

Authors
========
Sarvesh Patil 
Nagarjun chinnari 

License
=========
This project is licensed under the ASU License

Acknowledgments
================
1)"http://www.2net.co.uk/tutorial/periodic_threads”- creating periodic tasks in linux
2)“https://www.mcs.anl.gov/~kazutomo/rdtsc.html”-Time stamp counter examples
