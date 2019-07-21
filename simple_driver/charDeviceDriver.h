/* Global definition for the example character device driver */

 int init_module(void);
 void  cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long);
//static char* cur_msg(void);
//static  int  set_msg(char* new_msg);
#define SUCCESS 0
#define DEVICE_NAME "opsysmem"	/* Dev name as it appears in /proc/devices   */	/* Max length of the message from the device */
#define  MAX_MSG_SIZE 4*1024
//#define  MAX_ALL_SIZE 2*1024*1024    
#define  MAX_LIST_LEN 512
/* 
 * Global variables are declared as static, so are global within the file. 
 */
struct cdev *my_cdev;
dev_t dev_num;

static int Major;		/* Major number assigned to our device driver */
static int Device_Open = 0;	/* Is device open?  
				 * Used to prevent multiple access to device */
//static char msg[MAX_MSG_SIZE];	/* The msg the device will give when asked */
//static char  message[MAX_MSG_SIZE] = {0};           ///< Memory for the string that is passed from userspace
static char msg_list[MAX_LIST_LEN][MAX_MSG_SIZE];
static int wy_current  = -1;
static struct file_operations fops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.unlocked_ioctl = device_ioctl,
	.release = device_release
};
