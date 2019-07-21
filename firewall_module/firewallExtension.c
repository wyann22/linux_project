#include <linux/module.h>  /* Needed by all modules */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#include <linux/netfilter.h> 
#include <linux/netfilter_ipv4.h>
#include <linux/device.h>   
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/proc_fs.h>   // Header to support the kernel Driver Model
#include <linux/skbuff.h>
#include <linux/string.h>
#include <linux/compiler.h>
#include <net/tcp.h>
#include <linux/namei.h>
#include <linux/sched.h>
#include <linux/version.h>
#include <linux/dcache.h>

MODULE_AUTHOR ("Yan Wang 1882184");
MODULE_DESCRIPTION ("Extensions to the firewall") ;
MODULE_LICENSE("GPL");
#define DEVICE_NAME "firewallExtension"

/* make IP4-addresses readable */
#define NAME_SIZE 80
#define NIPQUAD(addr) \
((unsigned char *)&addr)[0], \
((unsigned char *)&addr)[1], \
((unsigned char *)&addr)[2], \
((unsigned char *)&addr)[3]


//struct nf_hook_ops *reg;
typedef struct rule{
  long port;
  char program[100];
  //struct rule *next;  
}Rule;
static Rule rules[200];
static int rule_count;
static spinlock_t wy_lock;
static int if_busy;
//static Rule * wy_current; 
// the firewall hook - called for each outgoing packet 
#if LINUX_VERSION_CODE <= KERNEL_VERSION(4, 3, 0)
#error "Kernel version < 4.4 not supported!"
//kernels < 4.4 need another firewallhook!
#endif

int current_process_name(char name[])
{
    struct path path;
    pid_t mod_pid;
    struct dentry *procDentry;
    struct dentry *parent;

    char cmdlineFile[NAME_SIZE];
    int res;
    
    printk (KERN_INFO "findExecutable module loading\n");
    /* current is pre-defined pointer to task structure of currently running task */
    mod_pid = current->pid;
    snprintf (cmdlineFile,NAME_SIZE, "/proc/%d/exe", mod_pid); 
    res = kern_path (cmdlineFile, LOOKUP_FOLLOW, &path);
    if (res) {
      printk (KERN_INFO "Could not get dentry for %s!\n", cmdlineFile);
      return -EFAULT;
    }
    
    procDentry = path.dentry;
    parent = procDentry->d_parent;
    printk (KERN_INFO "The name is %s\n", procDentry->d_name.name);
    printk (KERN_INFO "The name of the parent is %s\n", parent->d_name.name);
    path_put(&path);
    snprintf(name,NAME_SIZE,"/usr/%s/%s",parent->d_name.name,procDentry->d_name.name);
    return 0;
    
}
unsigned int FirewallExtensionHook (void *priv,
  struct sk_buff *skb,
  const struct nf_hook_state *state) {

  struct tcphdr *tcp;
  struct tcphdr _tcph;
  struct sock *sk;
  struct mm_struct *mm;
  int i  ; 
  int if_port=0;
  char exe_name[NAME_SIZE]; 
  sk = skb->sk;
  if (!sk) {
    printk (KERN_INFO "firewall: netfilter called with empty socket!\n");;
    return NF_ACCEPT;
  }

  if (sk->sk_protocol != IPPROTO_TCP) {
    printk (KERN_INFO "firewall: netfilter called with non-TCP-packet.\n");
    return NF_ACCEPT;
  }



    /* get the tcp-header for the packet */
  tcp = skb_header_pointer(skb, ip_hdrlen(skb), sizeof(struct tcphdr), &_tcph);
  if (!tcp) {
   printk (KERN_INFO "Could not get tcp-header!\n");
   return NF_ACCEPT;
 }
 if (tcp->syn) {
   struct iphdr *ip;

   printk (KERN_INFO "firewall: Starting connection \n");
   ip = ip_hdr (skb);
   if (!ip) {
     printk (KERN_INFO "firewall: Cannot get IP header!\n!");
   }
   else {
     printk (KERN_INFO "firewall: Destination address = %u.%u.%u.%u\n", NIPQUAD(ip->daddr));
   }
   printk (KERN_INFO "firewall: destination port = %d\n", ntohs(tcp->dest)); 



   if (in_irq() || in_softirq() || !(mm = get_task_mm(current))) {
    printk (KERN_INFO "Not in user context - retry packet\n");
    return NF_ACCEPT;
  }
  mmput(mm);
  current_process_name(exe_name);
  printk(KERN_INFO "current process name:%s",exe_name);
  for(i=0 ; i<rule_count;i++){  
    printk(KERN_INFO "rule port:%ld",rules[i].port);
    if (ntohs (tcp->dest) == rules[i].port) {
      printk(KERN_INFO "rule programe name:%s",rules[i].program);
      if(strcmp(exe_name,rules[i].program)==0){
        return NF_ACCEPT;
      }else{
         if_port=1;
      }
      
   }
 }
 if(if_port==1){
      tcp_done (sk); /* terminate connection immediately */
      printk (KERN_INFO "Connection shut down\n");
      return NF_DROP;
 }
 

}
return NF_ACCEPT;	
}
/*static void free_all(void){
 Rule* temp=rules.next;
 while(temp!=NULL){
  kfree(temp);
  temp=temp->next;
}
}*/
static struct nf_hook_ops firewallExtension_ops = {
	.hook    = FirewallExtensionHook,
	.pf      = PF_INET,
	.priority = NF_IP_PRI_FIRST,
	.hooknum = NF_INET_LOCAL_OUT
};
static int device_open(struct inode *inode, struct file *file)
{
  spin_lock(&wy_lock);
  if(if_busy==1){
     return EAGAIN;
  }else{
    if_busy=1;
  }
  spin_unlock(&wy_lock);
  return 0;
}

/* Called when a process closes the device file. */
static int device_release(struct inode *inode, struct file *file)
{
  spin_lock(&wy_lock);
  if_busy=0;
  spin_unlock(&wy_lock);
  printk (KERN_INFO "Closing device \n");

  return 0;
}
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h   */
         char __user *buffer, /* buffer to fill with data */
         size_t length, /* length of the buffer     */
loff_t * offset)
{ 
  int i=0;
  for (i = 0; i < rule_count; i++){
    printk(KERN_INFO "%ld %s\n",rules[i].port, rules[i].program);
  }
  return 0;
}
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{      
 int buff_len = strlen(buff);
 int i;
 int j=0;
 int click=0;
 Rule* wy_current; 
 static char port_str[10];
 long port ; 
 if(strcmp(buff,"newRule")==0){
  printk(KERN_INFO "buff : %s\n",buff);
  memset(rules,0,sizeof(rules));
  rule_count=0;
  return len ; 
}
wy_current = &rules[rule_count];
printk(KERN_INFO "buff : %s\n",buff);
for(i=0;i<buff_len-1;i++){
 if(buff[i]==' '){
   click=1;
   j=i;
   continue;
 }
 if(click==0){
  port_str[i]=buff[i];
}
if(click==1){
 wy_current->program[i-j-1]=buff[i];
}
}
printk(KERN_INFO "port_str : %s\n",port_str);
printk(KERN_INFO "program: %s\n",wy_current->program);

kstrtol(port_str,10,&port); 
wy_current->port=port; 
rule_count++;
   //strcpy(wy_current->program,program_str);
printk(KERN_INFO "port_num:%ld", port);
  //printk(KERN_INFO "wy_current: %s\n",wy_current->program);
  //printk(KERN_INFO "wy_current:%d\n",wy_current->port);
memset(port_str,0,sizeof(port_str));
return len ;
  //sprintf(msg, "%s(%zu letters)", buffer, len);   // appending received string with its length

}
static struct file_operations fops = {
  .owner = THIS_MODULE,
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};
/*static struct miscdevice firewall_dev = {
     let the kernel pick a minor number for ourselves 
    .minor = MISC_DYNAMIC_MINOR,
    .name ="firewallExtension",
    .fops = &fops
};*/

int init_module(void)
{
  int errno;
  struct proc_dir_entry *proc_file_entry;
  rule_count=0;
  if_busy=0;
  errno = nf_register_hook(&firewallExtension_ops); /* register the hook */
  if (errno) {
    printk (KERN_INFO "Firewall extension could not be registered!\n");
    return -EFAULT;
  }else {
    printk(KERN_INFO "Firewall extensions module loaded\n");
  }
  proc_file_entry = proc_create(DEVICE_NAME, 666, NULL, &fops);
  if(proc_file_entry == NULL)
   return -ENOMEM;
  // A non 0 return means init_module failed; module can't be loaded.
 return errno;
}
void cleanup_module(void){
 remove_proc_entry(DEVICE_NAME, NULL);
 printk(KERN_INFO "proc/%s removed\n", DEVICE_NAME);
    nf_unregister_hook(&firewallExtension_ops); /* restore everything to normal */
 printk(KERN_INFO "Firewall extensions module unloaded\n");
}  




