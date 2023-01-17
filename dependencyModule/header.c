#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

extern int xvar;//telling this that we are using external xvar
extern void sayHello(void);
static int __init header_init(void)
{
printk("Hello, world -- Header\n %d-xvar \n",xvar);
return 0;
}
static void __exit header_exit(void)
{
printk("Goodbye, cruel world- Header\n");
}
module_init(header_init);
module_exit(header_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Praveenraj");
MODULE_DESCRIPTION("A Simple Module -Header");
