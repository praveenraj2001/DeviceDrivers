#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>// for all character devices
//#include<file_operations.h>


dev_t pdevID;
int ndevices = 1;
struct cdev cdev;
int ret,i;

int pseudo_open(struct inode* inode,struct file* file){
    printk("Pseudo--Open method\n");
    return 0;
}

int pseudo_close(struct inode* inode,struct file* file){
    printk("Pseudo--Close method\n");
    return 0;
}

ssize_t pseudo_read(struct file* file,char __user * buf, size_t size, loff_t *off){
    printk("Pseudo--Read method\n");
    return 0;
}

ssize_t pseudo_write(struct file* file,const char __user * buf, size_t size, loff_t *off){
    printk("Pseudo--Write method\n");
    return -ENOSPC;
}

struct file_operations fops = {
	.open 	 = pseudo_open,
	.release = pseudo_close,
	.write   = pseudo_write, 
	.read	 = pseudo_read
};
	
static int __init char_init(void)
{
    // for char drive allocation
    ret = alloc_chrdev_region(&pdevID,0,ndevices,"char_Device");
    if(ret){
   	    printk("Char_Device : Failed to register driver");
   	    return -EINVAL;
   	    }
    
    // for operations initilization
    cdev_init(&cdev,&fops);
    kobject_set_name(&cdev.kobj,"pdevice%d",i);
    ret = cdev_add(&cdev,pdevID+i,1);
    
    printk("Successfully registered, major=%d,minor=%d\n",MAJOR(pdevID),MINOR(pdevID));
    printk("Welcome to char_Driver\n");
    return 0;
}




static void __exit char_exit(void)
{
    cdev_del(&cdev);
    unregister_chrdev_region(pdevID,ndevices);
    printk("Goodbye, Char_Driver\n");
}
module_init(char_init);
module_exit(char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Praveenraj");
MODULE_DESCRIPTION("A Simple Module");
