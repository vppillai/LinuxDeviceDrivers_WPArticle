#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/proc_fs.h>
#include<linux/uaccess.h>
MODULE_LICENSE("GPL"); 

ssize_t procfs_read (struct file *filep, char __user *pBuffer, size_t size, loff_t *pOoffset);

struct file_operations procfs_fops=
{
 .read=procfs_read
};

int procfs_init(void)
{
  static struct proc_dir_entry* procfs_entry;
  int status;
  procfs_entry = proc_create("procfs",0,NULL,&procfs_fops);
  status=IS_ERR(procfs_entry);
  if(0!=status)
  {
    printk(KERN_ALERT"error creating entry in procfs\n");
    return -status;
  }
  else
  {
    printk(KERN_ALERT"procfs entry created\n");
  }
  return 0;
}

void procfs_exit(void)
{
  remove_proc_entry("procfs",NULL);
  printk(KERN_ALERT"procfs entry removed\n");
}

ssize_t procfs_read (struct file *filep, char __user *pBuffer, size_t size, loff_t *pOffset)
{
  if(0==*pOffset)
  {
    char *message="hello procfs world\n";
    unsigned int messageLength=strlen(message)+1;
    *pOffset=messageLength;
    copy_to_user(pBuffer,message,messageLength);
    return messageLength;
  }
  else
  {
    return 0;
  }
}

module_init(procfs_init);
module_exit(procfs_exit);
