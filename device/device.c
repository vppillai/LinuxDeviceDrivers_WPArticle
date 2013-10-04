#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/errno.h>
#include<linux/fs.h>	/*for fops*/
#include<linux/slab.h> /*for kmalloc*/
#include<linux/uaccess.h> /*for intermode copy*/
#include<linux/cdev.h>	/*for device registration*/
MODULE_LICENSE("GPL");

int device_open (struct inode *inodep, struct file *filep);
ssize_t device_write (struct file *filep, const char __user *userp, size_t size, loff_t *offsetp);
ssize_t device_read (struct file *filep, char __user *userp, size_t size, loff_t *offsetp);
int device_release (struct inode *inodep, struct file *filep);
int device_init(void);
void device_exit(void);

char *deviceMemory =NULL;	/*persistant memory of the device*/
dev_t deviceNumber;
struct cdev *device_cdev;

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
		return -errno;
	}
	else
	{
		cdev_init(device_cdev,&device_fops);
		status=cdev_add(device_cdev,deviceNumber,1);
		if(!status)
		{
			printk(KERN_ALERT"cdev_add failed");
			return(status);
		}
		deviceMemory=kmalloc(sizeof(char),GFP_KERNEL);
		if(NULL==deviceMemory)
		{
			printk(KERN_ALERT"failed to allocate memory for device\n");
			device_exit();
			return -ENOMEM;
		}
		else
		{
			printk(KERN_ALERT"module - device -inserted\n");
		}
	}
	return 0;
}

int device_open (struct inode *inodep, struct file *filep)
{
	printk(KERN_ALERT"device opened\n");
	return 0;
}
ssize_t device_write (struct file *filep, const char __user *buffer, size_t size, loff_t *offsetp)
{
	int status;
	if (1!=size)
	{
		printk(KERN_ALERT"write size != 1\n");
		return(-ENOSPC);	/*complain that there isno space in the device*/
	}
	else
	{
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
ssize_t device_read (struct file *filep, char __user *userp, size_t size, loff_t *offsetp)
{
	if(*offsetp==0)
	{
		copy_to_user(userp,deviceMemory,1);
		*offsetp=1;
		return 1;
	}
	else
	{
		*offsetp=0;
		return 0;
	}
}

int device_release (struct inode *inodep, struct file *filep)
{
	printk(KERN_ALERT"device released\n");
	return 0;
}
void device_exit(void)
{
	if(NULL!=deviceMemory)
	{
		kfree(deviceMemory);
	}
  	unregister_chrdev_region(deviceNumber,1);
	printk(KERN_ALERT"module - device -removed\n");
	cdev_del(device_cdev);
}

module_init(device_init)
module_exit(device_exit)
