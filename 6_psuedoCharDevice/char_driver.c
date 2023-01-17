#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

dev_t pdevID;
int ndevices = 1;
static int __init char_init(void)
{
    int ret;
    ret = alloc_chrdev_region(&pdevID,0,ndevices,"char_Device");
    if(ret){
   	    printk("Char_Device : Failed to register driver");
   	    return -EINVAL;
   	    }
    printk("Successfully registered, major=%d,minor=%d\n",MAJOR(pdevID),MINOR(pdevID));
    printk("Welcome to char_Driver\n");
    return 0;
}
static void __exit char_exit(void)
{
    unregister_chrdev_region(pdevID,ndevices);
printk("Goodbye, Char_Driver\n");
}
module_init(char_init);
module_exit(char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Praveenraj");
MODULE_DESCRIPTION("A Simple Module");
