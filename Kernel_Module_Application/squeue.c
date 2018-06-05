#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/string.h>



#define  MODULE_NAME   "squeue"
#define  DEVICE_1_NAME "dataqueue1"
#define  DEVICE_2_NAME "dataqueue2"
#define  CLASS_NAME    "dataqueueclass"
#define  size 10

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sarvesh Patil & Nagarjun Chinnari");
MODULE_DESCRIPTION("Linux Message Queue Driver");
MODULE_VERSION("0.1");


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
}*new_msg_buf;


struct op_msg_node 
{
  //Output queue Linked list's current node Data
  struct q_msg *msg;

  //Output queue Linked list's next node pointer
  struct op_msg_node *next;
};
struct op_msg_node *head=NULL;


static int    majorNumber;
static struct class*  dataqueueClass  = NULL;
static struct device* dataqueueDevice = NULL;


// The prototype functions for driver
static int     sq_create(struct inode *, struct file *);
static int     sq_delete(struct inode *, struct file *);
static ssize_t sq_read(struct file *, char *, size_t, loff_t *);
static ssize_t sq_write(struct file *, const char *, size_t, loff_t *);
 

static struct file_operations fops =
{
   .owner = THIS_MODULE,
   .open = sq_create,
   .read = sq_read,
   .write = sq_write,
   .release = sq_delete,
};


int msg_ID=0;

int enqueue_num=-1,minor0=0,minor1=1;
int front1 = -1, rear1 =-1;
int front2 = -1, rear2 =-1;

struct q_msg *dataqueue1[size];
struct q_msg *dataqueue2[size];


//############################################################################################
// Kernel module init

static int __init squeue_init(void)
{

	printk(KERN_INFO "Initializing the squeue LKM\n");
 
  // 0 given as 1st argument dynamically allocate a major number for the device
  majorNumber = register_chrdev(0, MODULE_NAME, &fops);
  if (majorNumber<0)
  {
  	printk(KERN_ALERT "Failed to register a major number\n");
  	return majorNumber;
  }
  printk(KERN_INFO "squeue registered correctly with major number %d\n", majorNumber);

  dataqueueClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(dataqueueClass))
  {                
    unregister_chrdev(majorNumber, MODULE_NAME);
    printk(KERN_ALERT "Failed to register device class\n");
    return PTR_ERR(dataqueueClass);
  }
  printk(KERN_INFO "dataqueue device class registered correctly\n");

  
  // Create devices
  dataqueueDevice = device_create(dataqueueClass, NULL, MKDEV(majorNumber, minor0), NULL, DEVICE_1_NAME);
  if (IS_ERR(dataqueueDevice))
  {               
    class_destroy(dataqueueClass);
    unregister_chrdev(majorNumber, DEVICE_1_NAME);
    printk(KERN_ALERT "Failed to create the device\n");
    return PTR_ERR(dataqueueDevice);
  }
  dataqueueDevice = device_create(dataqueueClass, NULL, MKDEV(majorNumber, minor1), NULL, DEVICE_2_NAME);
  if (IS_ERR(dataqueueDevice))
  {               
    class_destroy(dataqueueClass);
    unregister_chrdev(majorNumber, DEVICE_2_NAME);
    printk(KERN_ALERT "Failed to create the device\n");
    return PTR_ERR(dataqueueDevice);
  }

  printk(KERN_INFO "dataqueue device class created correctly\n"); 

  return 0;
}

static void __exit squeue_exit(void)
{
   device_destroy(dataqueueClass, MKDEV(majorNumber, 0));
   device_destroy(dataqueueClass, MKDEV(majorNumber, 1));
   class_unregister(dataqueueClass);
   class_destroy(dataqueueClass);
   unregister_chrdev(majorNumber, MODULE_NAME);
   printk(KERN_INFO "Goodbye from the squeue LKM!\n");
}

//############################################################################################

static int sq_create(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "Device dataqueue1 & dataqueue2 have been opened.\n");
   return 0;
}
 int x,y,z;
static ssize_t sq_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{ 
  char *a;
  a=kmalloc(sizeof(a), GFP_KERNEL);
  z=copy_from_user(a,buffer,sizeof(a));
  if(*a=='a')
  {
  
   if (front1 == -1)
    {
        printk("\nQueue1 is Empty");
        x=0;
        return x;
    }

    if(front1==rear1 && front1!=-1)
    {
      z=copy_to_user(buffer,(dataqueue1[front1]),sizeof(struct q_msg));
      front1=-1;
      rear1=-1;
      x=0;
    }

    else if(front1==size-1)
      {  
        z=copy_to_user(buffer,dataqueue1[front1],sizeof(struct q_msg));
        front1=0;
        x=rear1-front1+1;
      }
      
    else 
    {
      z=copy_to_user(buffer,dataqueue1[front1],sizeof(struct q_msg));
      front1++;
       if (rear1 >=front1)
    {       
            x=rear1-front1+1;
               
    }
  else
    {
              x=size-(front1-rear1)+1;
    }
    }

return x;
}

else if(*a=='b')
{
   if (front2 == -1)
    {
        printk("\nQueue2 is Empty");
       y=0; 
       return y;
    }

    if(front2==rear2 && front2!=-1)
    {
      z=copy_to_user(buffer,(dataqueue2[front2]),256);
      front2=-1;
      rear2=-1;
      y=0;
    }

    else if(front2==size-1)
      {  
        z=copy_to_user(buffer,dataqueue2[front2],256);
        front2=0;
        y=rear2-front2+1;
      }
      
    else 
    {
      z=copy_to_user(buffer,dataqueue2[front2],256);
      front2++;
       if (rear2 >=front2)
    {       
            y=rear2-front2+1;
               
    }
  else
    {
              y=size-(front2-rear2)+1;
    }
    }
    
return y;
}
return 0;
  // printk(KERN_INFO "squeue Received characters from the user\n");
   
}

// Write to device
static ssize_t sq_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
new_msg_buf=kmalloc(sizeof(struct q_msg),GFP_KERNEL);
  z=copy_from_user(new_msg_buf,(struct q_msg *)buffer,sizeof(struct q_msg));

if((new_msg_buf->src_ID->t_type == 0 && new_msg_buf->src_ID->t_num==0) || (new_msg_buf->src_ID->t_type == 0 && new_msg_buf->src_ID->t_num==1) || (new_msg_buf->src_ID->t_type == 1 && new_msg_buf->src_ID->t_num==0))
   enqueue_num=0;
  else if((new_msg_buf->src_ID->t_type == 0 && new_msg_buf->src_ID->t_num==2) || (new_msg_buf->src_ID->t_type == 0 && new_msg_buf->src_ID->t_num==3) || (new_msg_buf->src_ID->t_type == 1 && new_msg_buf->src_ID->t_num==1))
   enqueue_num=1;


    if(enqueue_num == 0)
    {
    if ((front1 == 0 && rear1 == size-1) ||
            (rear1 == front1-1))
    {
        printk("\nQueue1 is Full\n");
        return -1;
    }
 
    else if (front1 == -1) 
    {
        front1 = rear1 = 0;
        (dataqueue1[rear1])=new_msg_buf;
        

    }
 
    else if (rear1 == size-1 && front1 != 0)
    {
        rear1 = 0;
        dataqueue1[rear1]=new_msg_buf;
      
    }
 
    else
    {
        rear1++;
        dataqueue1[rear1]=new_msg_buf;
    }
  }
 else if(enqueue_num == 1)
  {
    

    if ((front2 == 0 && rear2 == size-1) ||
            (rear2 == front2-1))
    {
        printk("\nQueue2 is Full\n");
        return -1;
    }
 
    else if (front2 == -1) 
    {
        front2 = rear2 = 0;
        dataqueue2[rear2]=new_msg_buf;
   }
 
    else if (rear2 == size-1 && front2 != 0)
    {
        rear2 = 0;
        dataqueue2[rear2]=new_msg_buf;

    }
 
    else
    {
        rear2++;

        dataqueue2[rear2]=new_msg_buf;
    }
  }
  
  //printk(KERN_INFO "squeue Received message from the user\n");
  return 0;
}

// Device delete
static int sq_delete(struct inode *inodep, struct file *filep)
{
   printk(KERN_INFO "Device squeue successfully closed\n");
   return 0;
}
 
module_init(squeue_init);
module_exit(squeue_exit);