#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

int ndevice = 1;
module_param(ndevice,int,S_IRUGO);
static int __init hello_init(void)
{
printk("Hello, world\n devices = %d\n",ndevice);
return 0;
}
static void __exit hello_exit(void)
{
printk("Goodbye, cruel world\n");
}
module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Praveenraj");
MODULE_DESCRIPTION("A Simple Module");
