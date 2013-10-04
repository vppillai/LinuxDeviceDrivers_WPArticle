#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/errno.h>
#include<linux/fs.h>  /*for fops*/
#include<linux/slab.h> /*for kmalloc*/
#include<linux/uaccess.h> /*for intermode copy*/
#include<linux/cdev.h>  /*for device registration*/
#include<linux/device.h> /*for class and node creation*/
MODULE_LICENSE("GPL");

int device_open (struct inode *inodep, struct file *filep);
ssize_t device_write (struct file *filep, const char __user *userp, size_t size, loff_t *offsetp);
ssize_t device_read (struct file *filep, char __user *userp, size_t size, loff_t *offsetp);
int device_release (struct inode *inodep, struct file *filep);
int device_init(void);
void device_exit(void);

char *deviceMemory =NULL; /*persistant memory of the device*/
dev_t deviceNumber;
struct cdev *device_cdev;
struct class *deviceClass;
struct device *deviceNode;

/*structure holding pointers to actual file operation functions*/
struct file_operations device_fops={
  .open=device_open,
  .read=device_read,
  .write=device_write,
  .release=device_release
};

/*device initialization function*/
int device_init(void)
{
  int status;
  /*dynamically fetching device node numbers*/
  status=alloc_chrdev_region(&deviceNumber,0,1,"device");
  if(0==status)
  {
    printk(KERN_ALERT"got device numbers : %d-%d\n",MAJOR(deviceNumber),MINOR(deviceNumber));
  }
  else
  {
    printk(KERN_ALERT"failed alloc_chrdev_region (%d)\n",status);
    return status;
  }
  /*creating a cdev structure to hold the device info - mainly fops. Allocation is not mandatory, kept here for function coverage*/
  device_cdev=cdev_alloc();
  if(NULL==device_cdev)
  {
    printk(KERN_ALERT"failed allocating cdev\n");
    return -1;
  }
  else
  {
    /*intiialize the cdev struct with fops. this can even be done directly (device_cdev->ops=&cdev_fops)*/
    cdev_init(device_cdev,&device_fops);
    status=cdev_add(device_cdev,deviceNumber,1);
    if(0!=status)
    {
      printk(KERN_ALERT"cdev_add failed(%d)\n",status);
      return(status);
    }
    /*allocate memory for device. Again, this is an overkill for a byte. But kept here for API ref*/
    deviceMemory=kmalloc(sizeof(char),GFP_KERNEL);
    if(NULL==deviceMemory)
    {
      printk(KERN_ALERT"failed to allocate memory for device\n");
      device_exit();
      return -ENOMEM;
    }
    else
    {
      *deviceMemory=0;
      printk(KERN_ALERT"module - device -inserted. now creating device node\n");
	    deviceClass=class_create(THIS_MODULE,"deviceClass");
      if(IS_ERR(deviceClass))
      {
        printk(KERN_ALERT"error in class_create\n");
        device_exit();
        return -1;
      }
      else
      {
        deviceNode=device_create(deviceClass,NULL,deviceNumber,NULL,"device");
        if(IS_ERR(deviceNode))
        {
          device_exit();
          return -1;
        }
      }
    }
  }
  return 0;
}

/*This function will be called whenever an open syscall is made on the device file. If this fop is null, it will return success by default*/
int device_open (struct inode *inodep, struct file *filep)
{
  printk(KERN_ALERT"device opened\n");
  return 0;
}

/*write syscall backend. we will write a single byte of data into the device using this call and fail the operation if 
request is not for a simgle byte. so use echo -n <data> > /dev/device */
ssize_t device_write (struct file *filep, const char __user *buffer, size_t size, loff_t *offsetp)
{
  int status;
  if (1!=size)
  {
    printk(KERN_ALERT"write size != 1\n");
    return(-ENOSPC);  /*complain that there isno space in the device*/
  }
  else
  {
    /*copy buffer containing data to kernel space into our device buffer*/
    status=copy_from_user(deviceMemory,buffer,1);
    if(0==status)
    {
      return 1;
    }
    else
    {
      printk(KERN_ALERT"copy_from_user failed \n");
      return -EAGAIN;
    }
  }
}

/*read syscall backend. */
ssize_t device_read (struct file *filep, char __user *userp, size_t size, loff_t *offsetp)
{
  /*if teh intentio is to read the 0th position of the file*/
  if(*offsetp==0)
  {
    copy_to_user(userp,deviceMemory,1);
    *offsetp=1;/*move new current position*/
    return 1; /*number of bytes read*/
  }
  else /*any position other than 0*/
  {
    *offsetp=0;
    return 0;/*no bytes read*/
  }
}

/*for each fclose*/
int device_release (struct inode *inodep, struct file *filep)
{
  printk(KERN_ALERT"device released\n");
  return 0;
}

/*cleanup function for modeule remove*/
void device_exit(void)
{
  if(NULL!=deviceMemory)
  {
    kfree(deviceMemory);
  }
    unregister_chrdev_region(deviceNumber,1);
  printk(KERN_ALERT"module - device -removed\n");
  cdev_del(device_cdev);
  device_destroy(deviceClass,deviceNumber);
  class_destroy(deviceClass);
}

module_init(device_init)
module_exit(device_exit)
