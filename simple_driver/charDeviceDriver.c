/*
 *  chardev.c: Creates a read-only char device that says how many times
 *  you've read from the dev file
 */

#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>  

/* for put_user */
#include <charDeviceDriver.h>
#include "ioctl.h"

MODULE_LICENSE("GPL");

/* 
 * This function is called whenever a process tries to do an ioctl on our
 * device file. We get two extra parameters (additional to the inode and file
 * structures, which all device functions get): the number of the ioctl called
 * and the parameter given to the ioctl function.
 *
 * If the ioctl is write or read/write (meaning output is returned to the
 * calling process), the ioctl call returns the output of this function.
 *
 */


DEFINE_MUTEX  (devLock);

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param){
	int param = (int) ioctl_param;
	if (ioctl_num == RESET_COUNTER) {
	    if(MAX_MSG_SIZE*MAX_LIST_LEN>param){
	    	return -EINVAL;
	    }
	    return 0;
	}else {
	    return -EINVAL;
	}
}
 int  init_module(void){
       Major = register_chrdev(0, DEVICE_NAME, &fops);
	if (Major < 0) {
	  printk(KERN_ALERT "Registering char device failed with %d\n", Major);
	  return Major;
	}
	printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
	printk(KERN_INFO  "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
	return SUCCESS;
}

void  cleanup_module(void){
	 unregister_chrdev(Major, DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *file)
{
    
    mutex_lock (&devLock);
    if (Device_Open) {
	mutex_unlock (&devLock);
	return -EBUSY;
    }
    Device_Open++;
    mutex_unlock (&devLock);
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

/* Called when a process closes the device file. */
static int device_release(struct inode *inode, struct file *file)
{	
	//int i = 0;
      mutex_lock (&devLock);
	Device_Open--;		
	mutex_unlock (&devLock);
	module_put(THIS_MODULE);
	

	return 0;
}

/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */


static ssize_t device_read(struct file *filp, char *buffer,size_t length, loff_t * offset){     
	//int bytes_read = 0;
	//int result;
	char *msg;
	size_t msg_len=0;  
	int ifSend=0;
		printk(KERN_INFO "%d:No data to read!",wy_current);

	if(wy_current<0){
			printk(KERN_INFO "%d:No data to read!",wy_current);
		return -EAGAIN;
	} 
	msg = msg_list[wy_current];
	 msg_len= strlen(msg);
	 printk(KERN_INFO "Current in read :%zu\n",msg_len);
	ifSend= copy_to_user(buffer, msg, msg_len);
	printk(KERN_INFO "buffer in read :%s\n",buffer);
	if(ifSend==0){

		mutex_lock (&devLock);
		//strcpy(msg ,msg_list[wy_current]);
		memset(msg_list[wy_current], 0, MAX_MSG_SIZE);
	//	printk(KERN_INFO "cccc :%s\n",msg_list[wy_current]);
		wy_current --;
		mutex_unlock (&devLock);
		return msg_len;
	}else{
		return -EFAULT;
	}
	

   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
     

	
}

/* Called when a process writes to dev file: echo "hi" > /dev/hello  */
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{      
	
//	char *temp=(char*)kmalloc(sizeof(char)*len,GFP_KERNEL);
      printk( KERN_INFO " User message:%s\n", buff);
      printk(KERN_INFO "List length :%d\n",MAX_LIST_LEN);	
//	strncpy(temp,buff,len);
    //  printk(KERN_INFO " %s. Totalkto\n", temp);	

	if(strlen(buff)>MAX_MSG_SIZE){
		return -EINVAL;
	}
	if(wy_current>=MAX_LIST_LEN-1){
		return -EAGAIN;
	}
	mutex_lock (&devLock); 
	wy_current++;
	sprintf(msg_list[wy_current],"%s",buff);
	printk(KERN_INFO "Current :%s\n",msg_list[wy_current]);
	mutex_unlock (&devLock);
	printk(KERN_INFO "list length :%d\n",MAX_LIST_LEN+1);
	return len ;
	//sprintf(msg, "%s(%zu letters)", buffer, len);   // appending received string with its length
   	
}


