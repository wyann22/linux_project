#include <linux/module.h>  /* Needed by all modules */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#include <linux/netfilter.h> 
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/compiler.h>
#include <net/tcp.h>
#include <linux/namei.h>
#include <linux/timer.h>
#include <linux/version.h>
#include <linux/miscdevice.h>

MODULE_AUTHOR ("Eike Ritter <E.Ritter@cs.bham.ac.uk>");
MODULE_DESCRIPTION ("Extensions to the firewall") ;
MODULE_LICENSE("GPL");


static int udp_counter = 0;
static int tcp_counter = 0;

struct counter_info {
	int id;
	unsigned long wait_address;
	int udp_counter;
	int tcp_counter;
};

struct nf_hook_ops *reg;


static spinlock_t udp_counter_lock; // exclusive access to message lists
static spinlock_t tcp_counter_lock; // exclusive access to message lists




void my_timer_callback( unsigned long data ) {
	struct counter_info *counters;
	
	counters = (struct counter_info *) data;
	printk(KERN_INFO "Timer called with address %lx for id %d\n", (long unsigned int) &counters->wait_address, counters->id);
	spin_lock(&udp_counter_lock);
	counters->udp_counter = udp_counter;
	spin_unlock(&udp_counter_lock);
	spin_lock(&tcp_counter_lock);
	counters->tcp_counter = tcp_counter;
	spin_unlock(&tcp_counter_lock);
	clear_bit(1, &counters->wait_address);
	wake_up_bit(&counters->wait_address, 1);
}



// the firewall hook - called for each outgoing packet 
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4, 3, 0)
#error "Kernel version < 4.4 not supported!"
//kernels < 4.4 need another firewallhook!
#endif
unsigned int FirewallExtensionHook (void *priv,
				    struct sk_buff *skb,
				    const struct nf_hook_state *state) {

    struct sock *sk;


  sk = skb->sk;
  if (!sk) {
    printk (KERN_INFO "firewall: netfilter called with empty socket!\n");;
    return NF_ACCEPT;
  }

  if (sk->sk_protocol == IPPROTO_TCP) {
   printk (KERN_INFO "firewall: netfilter called with TCP-packet.\n");
   spin_lock(&tcp_counter_lock);
    tcp_counter++;
   spin_unlock(&tcp_counter_lock);
    return NF_ACCEPT;
  }

  if (sk->sk_protocol == IPPROTO_UDP) {
    printk (KERN_INFO "firewall: netfilter called with UDP-packet.\n");
    spin_lock(&udp_counter_lock);
    udp_counter++;
    spin_unlock(&udp_counter_lock);
    return NF_ACCEPT;
  }
  return NF_ACCEPT;	
}


static struct nf_hook_ops firewallExtension_ops = {
	.hook    = FirewallExtensionHook,
	.pf      = PF_INET,
	.priority = NF_IP_PRI_FIRST,
	.hooknum = NF_INET_LOCAL_OUT
};

/* 
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file)
{
	return 0;
}

/* Called when a process closes the device file. */
static int device_release(struct inode *inode, struct file *file)
{
    printk (KERN_INFO "Closing device \n");

    return 0;
}


/* 
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp,	/* see include/linux/fs.h   */
			   char __user *buffer,	/* buffer to fill with data */
			   size_t length,	/* length of the buffer     */
			   loff_t * offset)
{

	int id;
	int res;
	struct timer_list my_timer;

	
	struct counter_info *counters;

	if (length < 2*sizeof(int)) 
		return -ENOMEM;

	counters = kmalloc(sizeof(*counters), GFP_KERNEL);
	if (!counters) 
		return -ENOMEM;

	
	counters->id = id;
	// prepare for waiting
	set_bit(1, &counters->wait_address);
	// my_timer.function, my_timer.data
	setup_timer(&my_timer, my_timer_callback, (unsigned long) counters);

	printk( "Starting timer with address %lx for id %d to fire in 2000ms (%ld)\n", (long unsigned int) &counters->wait_address, id, jiffies );
	res = mod_timer( &my_timer, jiffies + msecs_to_jiffies(5000));
	if (res) {
		printk("Error in mod_timer\n");
		del_timer_sync(&my_timer);
		kfree(counters);
		return -EFAULT;
	}

	res = wait_on_bit(&counters->wait_address, 1, TASK_INTERRUPTIBLE);
	if (res == -ERESTARTSYS) {
	    	del_timer_sync(&my_timer);
		kfree(counters);
		return -EINTR;
	}	
	if (copy_to_user(buffer, &counters->udp_counter, sizeof(int))) {
		kfree(counters);
		return -EFAULT;
	}
	if (copy_to_user(buffer+sizeof(int), &counters->tcp_counter, sizeof(int))) {
		kfree(counters);
		return -EFAULT;
	}

	kfree(counters);
	return 2*sizeof(int);
}


static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .open = device_open,
    .release = device_release
};



static struct miscdevice firewall_dev = {
    /* let the kernel pick a minor number for ourselves */
    .minor = MISC_DYNAMIC_MINOR,
    .name ="firewallCount",
    .fops = &fops
};

int init_module(void)
{

  int ret;

  ret = nf_register_hook (&firewallExtension_ops); /* register the hook */
  if (ret) {
	  printk (KERN_INFO "Firewall extension could not be registered!\n");
	  return -EFAULT;
  }

  ret = misc_register(&firewall_dev);
  if (ret) {
	  printk(KERN_ERR "Cannot register opsysmem-device!\n");
	  nf_unregister_hook(&firewallExtension_ops);
	  return -EFAULT;
  }	


  // A non 0 return means init_module failed; module can't be loaded.
  return ret;
}


void cleanup_module(void)
{

	nf_unregister_hook (&firewallExtension_ops); /* restore everything to normal */
	misc_deregister(&firewall_dev);

	printk(KERN_INFO "Firewall extensions module unloaded\n");
}  


