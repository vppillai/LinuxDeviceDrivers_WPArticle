#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
MODULE_LICENSE("GPL");    /*else kernel complaints about kernel taint*/
int simple_init(void)
{
   printk(KERN_ALERT"Hello kernel space world\n");   /*Kernel mode printf - messages appear in /var/log/messages . or use command dmesg | tail*/
    return 0;
}
int simple_exit(void)
{
   printk(KERN_ALERT"Bye bye kernel world\n");
    return 0;
}
module_init(simple_init);    /* register the module init function to be executed on insmod*/
module_exit(simple_exit);    /* register the module cleanup/exit function*/
