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
  proc_create("procfs",0,NULL,&procfs_fops);
  return 0;
}

void procfs_exit(void)
{
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
