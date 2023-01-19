#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>// for all character devices
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#define MAX_SIZE 1024


dev_t pdevID;
struct class *pclass;
struct device *pdev;


unsigned char *pbuffer;
int rd_offset=0;
int wr_offset=0;
int buflen=0;



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

ssize_t pseudo_read(struct file* file,char __user * ubuf, size_t usize, loff_t *off){
    int rcount =usize;
	 if(buflen==0)
        {
                printk("BUFFER IS EMPTY\n");
                return 0;
        }
        if(rcount > buflen)
                rcount = buflen;

        ret = copy_to_user(ubuf, pbuffer + rd_offset, rcount); //faulty code (destination,source)
        if(ret)
        {
                printk("COPT TO USER FAILED\n");
                return -EFAULT;
        }
        rd_offset += rcount;
        buflen -= rcount;
	printk("PSEUDO --- READ METHOD\n");
        return rcount;
}

ssize_t pseudo_write(struct file* file,const char __user * ubuf, size_t usize, loff_t *off){
    int wcount =usize;
	if(wr_offset >= MAX_SIZE)
	{
		printk("BUFFER IS FULL\n");
		return -ENOSPC;
	}

	if(wcount > MAX_SIZE - wr_offset)
		wcount = MAX_SIZE - wr_offset;

	ret = copy_from_user(pbuffer+wr_offset,ubuf, wcount);
	if(ret)
	{
		printk("COPY FROM USER FAILED\n");
		return -EFAULT;
	}
	wr_offset += wcount;
	buflen += wcount;
	printk("PSEUDO --- WRITE METHOD\n");
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
    // for char drive allocation
    pclass = class_create(THIS_MODULE,"pseudo_class");
    ret = alloc_chrdev_region(&pdevID,0,ndevices,"char_Device");
    if(ret){
   	    printk("Char_Device : Failed to register driver");
   	    return -EINVAL;
   	    }
    pbuffer = kmalloc(MAX_SIZE,GFP_KERNEL);
	if(pbuffer==NULL)
	{
		printk("KMALLOC FAILED\n");
	}
    
    // for operations initilization
    cdev_init(&cdev,&fops);
    kobject_set_name(&cdev.kobj,"pdevice%d",i);
    ret = cdev_add(&cdev,pdevID+i,1);
    pdev = device_create(pclass,NULL,pdevID,NULL,"psamplle%d",i);
    
    
    printk("Successfully registered notFault, major=%d,minor=%d\n",MAJOR(pdevID),MINOR(pdevID));
    printk("Welcome to char_Driver\n");
    return 0;
}


static void __exit char_exit(void)
{   
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
