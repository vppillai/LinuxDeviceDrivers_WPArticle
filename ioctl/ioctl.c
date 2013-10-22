#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

int ioctl_init(void);
void ioctl_exit(void);
long ioctl_ioctl (struct file *pFile, unsigned int ioctlNum, unsigned long arg);

struct file_operations ioctlFops=
{
  .unlocked_ioctl=ioctl_ioctl
};

long ioctl_ioctl (struct file *pFile, unsigned int ioctlNum, unsigned long arg)
{
  return 0;  
}
int ioctl_init(void)
{

  return 0;
}
void ioctl_exit(void)
{

}

module_init(ioctl_init);
module_exit(ioctl_exit);
