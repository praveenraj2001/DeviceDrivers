#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>// for all character devices
#include<linux/device.h>
#include<linux/kfifo.h>
#define MAX_SIZE 1024

dev_t pdevID;
int ndevices = 1;
struct cdev cdev;
struct kfifo kfifo;


struct class *pclass;
struct device *pdev;


int ret,i;

int pseudo_open(struct inode* inode,struct file* file){
    printk("Pseudo--Open method\n");
    return 0;
}

int pseudo_close(struct inode* inode,struct file* file){
    printk("Pseudo--Close method\n");
    return 0;
}

ssize_t pseudo_read(struct file* file,char __user * ubuf, size_t usize, loff_t *off){
    int rcount = usize;
    char *tbuf;
    if(kfifo_is_empty(&kfifo)){
    	printk("\nkfifo is empty\n");
    	return 0;
    }
    if(rcount > kfifo_len(&kfifo)){
    	rcount = kfifo_len(&kfifo);
    }
    tbuf = kmalloc(rcount,GFP_KERNEL);
    ret = kfifo_out(&kfifo,tbuf,rcount);
    ret = copy_to_user(ubuf,tbuf,rcount);
    kfree(tbuf);
    printk("Pseudo--Read method\n");
    return rcount;
}

ssize_t pseudo_write(struct file* file,const char __user * ubuf, size_t usize, loff_t *off){
    int wcount = usize;
    char *tbuf;
    if(kfifo_is_full(&kfifo)){
    	printk("kfifo is full wrote only %d\n",MAX_SIZE);
    	return -ENOSPC;
    }
    if(wcount>kfifo_avail(&kfifo)){
    	wcount = kfifo_avail(&kfifo);
    }
    if(kfifo_avail(&kfifo)){
    	tbuf = kmalloc(wcount,GFP_KERNEL);
    	ret = copy_from_user(tbuf,ubuf,wcount);
    	if(ret){
    	printk("copy from user failed\n");
    	return -EINVAL;
    }
    	ret = kfifo_in(&kfifo,tbuf,wcount);
    	if(ret){
    	printk("copy to kfifo\n");}
    	printk("write is successfull\n");
    	return wcount;
    }
    printk("Pseudo--Write method\n");
    return wcount;
}

struct file_operations fops = {
	.open 	 = pseudo_open,
	.release = pseudo_close,
	.write   = pseudo_write, 
	.read	 = pseudo_read
};
	
static int __init char_init(void)
{
    char *tempbuf;
    // for char drive allocation
    pclass = class_create(THIS_MODULE,"pseudo_class");
    ret = alloc_chrdev_region(&pdevID,0,ndevices,"char_Device");
    if(ret){
   	    printk("Char_Device : Failed to register driver");
   	    return -EINVAL;
   	    }
    
    tempbuf = kmalloc(MAX_SIZE, GFP_KERNEL);// ALLOCATE MEMORY FOR TEMPBUF	
    if (tempbuf==NULL){
     	printk("failure with kmalloc");
     	return -ENOMEM;
    }
    ret = kfifo_init(&kfifo,tempbuf,MAX_SIZE); // initilize kfifo with tempbuf
    if (ret){
     	printk("failure with kfifo");
     	return -EINVAL;
    }
    
    // for operations initilization
    cdev_init(&cdev,&fops);
    kobject_set_name(&cdev.kobj,"pdevice%d",i);
    ret = cdev_add(&cdev,pdevID+i,1);
    pdev = device_create(pclass,NULL,pdevID,NULL,"psamplle%d",i);
    
    
    printk("Successfully registered, major=%d,minor=%d\n",MAJOR(pdevID),MINOR(pdevID));
    printk("Welcome to char_Driver\n");
    return 0;
}


static void __exit char_exit(void)
{   
    kfifo_free(&kfifo);
    cdev_del(&cdev);
    device_destroy(pclass,pdevID);
    unregister_chrdev_region(pdevID,ndevices);
    class_destroy(pclass);
    printk("Goodbye, Char_Driver\n");
}
module_init(char_init);
module_exit(char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Praveenraj");
MODULE_DESCRIPTION("A Simple Module");
