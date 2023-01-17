#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

int xvar=100;
void sayHello(void){};
static int __init main_init(void)
{
printk("Hello, world -- Main\n %d-xvar \n",xvar);
return 0;
}
static void __exit main_exit(void)
{
printk("Goodbye, cruel world- Main\n");
}
module_init(main_init);
module_exit(main_exit);
EXPORT_SYMBOL_GPL(xvar);//exports the value to extern xvar
EXPORT_SYMBOL_GPL(sayHello);//exports the defination to extern function sayHello
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Praveenraj");
MODULE_DESCRIPTION("A Simple Module - Main");
