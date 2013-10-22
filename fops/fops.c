#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/types.h> /*for dev_t*/
#include<linux/fs.h>/*for fops*/
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/uaccess.h>/*for user copy*/

MODULE_LICENSE("GPL");

int filep_init(void);
void filep_exit(void);

/*fops function set*/
ssize_t filep_read0 (struct file *filep, char __user *buffer, size_t size, loff_t *offset);
ssize_t filep_read1 (struct file *filep, char __user *buffer, size_t size, loff_t *offset);
ssize_t filep_write (struct file *filep, const char __user *buffer, size_t size, loff_t *offset);
int filep_open (struct inode *inode, struct file *filep);
int filep_release (struct inode *inode, struct file *filep);


dev_t filepDev;
struct cdev filepCdev;
struct file_operations filepFops0=
{
  .open=filep_open,
  .read=filep_read0,
  .write=filep_write,
  .release=filep_release
};

struct file_operations filepFops1=
{
  .open=filep_open,
  .read=filep_read1,
  .write=filep_write,
  .release=filep_release
};

/*module init*/
int filep_init(void)
{
  int status;
  /*allocate one major and 2 minor numbers*/
  status=alloc_chrdev_region(&filepDev,0,2,"fops");
  if(0!=status)
  {
    printk(KERN_ALERT"alloc_chrdev_region failed\n");
    return status;
  }
  else
  {
    /*init one module for 2 devices*/
    cdev_init(&filepCdev,&filepFops0);    /*using minor0's file_op for now*/
    status=cdev_add(&filepCdev,filepDev,2);
    if(0!=status)
    {
      printk(KERN_ALERT"cdev_add failed \n");
      filep_exit();
      return status;
    }
  }
  printk(KERN_ALERT"inserted filep (%d,%d)\n",MAJOR(filepDev),MINOR(filepDev));
  return 0;
}

void filep_exit(void)
{
  cdev_del(&filepCdev);
  unregister_chrdev_region(filepDev,2);
}

/*reasd syscall backend*/
ssize_t filep_read0 (struct file *filep, char __user *buffer, size_t size, loff_t *offset)
{

  /*decision fork for different minor numbers*/
  char *message="this is minor number 0\n";

  /*print only once for read from begining of file*/
  if(0==*offset)
  {
    copy_to_user(buffer,message,sizeof(char)*(strlen(message)+1));
    *offset=1;
    return (sizeof(char)*(strlen(message)+1));
  }
  else
  {
    return 0;
  }
}

/*reasd syscall backend*/
ssize_t filep_read1 (struct file *filep, char __user *buffer, size_t size, loff_t *offset)
{

  /*decision fork for different minor numbers*/
  char *message="this is minor number 1\n";

  /*print only once for read from begining of file*/
  if(0==*offset)
  {
    copy_to_user(buffer,message,sizeof(char)*(strlen(message)+1));
    *offset=1;
    return (sizeof(char)*(strlen(message)+1));
  }
  else
  {
    return 0;
  }
}

/*pass through write*/
ssize_t filep_write (struct file *filep, const char __user *buffer, size_t size, loff_t *offset)
{
  return size;
}
int filep_open (struct inode *inode, struct file *filep)
{
  /*fetch device minor number from inode in the file structure pointer*/
  int filepMinor=iminor(filep->f_dentry->d_inode);
  if(0==filepMinor)
  {
   filep->f_op=&filepFops0; 
  }
  else if (1==filepMinor)
  {
   filep->f_op=&filepFops1;
  }
  else
  {
    return -ENODEV;  
  }
  return 0;
}
int filep_release (struct inode *inode, struct file *filep)
{
  return 0;
}
module_init(filep_init);
module_exit(filep_exit);
