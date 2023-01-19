#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/cdev.h>  // for all character devices
#include<linux/device.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

#define MAX_SIZE 1024

struct device *pdev;
struct class *pclass;

unsigned char *pbuffer;
int rd_offset=0;
int wr_offset=0;
int buflen=0;

dev_t pdevID;
int ndevices=1;
struct cdev cdev;
int ret,i;

int pseudo_open(struct inode*, struct file* file)
{
	printk("PSEUDO --- OPEN METHOD\n");
	return -EINVAL;
}

int pseudo_close(struct inode*, struct file*file)
{
	printk("PSEUDO-- CLOSE METHOD\n");
	return 0;
}

ssize_t pseudo_read(struct file* file, char __user *ubuf, size_t rsize, loff_t *off)
{
	int rcount =rsize;
	 if(buflen==0)
        {
                printk("BUFFER IS EMPTY\n");
                return 0;
        }
        if(rcount > buflen)
                rcount = buflen;

        ret = copy_to_user(pbuffer, ubuf, rcount);
        if(ret)
        {
                printk("COPT TO USER FAILED\n");
                return -EFAULT;
        }
        rd_offset += rcount;
        buflen -= rcount;
	printk("PSEUDO --- READ METHOD\n");
        return 0;
}

ssize_t pseudo_write(struct file* file, const char __user *ubuf,size_t usize, loff_t *off)
{
	int wcount =usize;
	if(wr_offset >= MAX_SIZE)
	{
		printk("BUFFER IS FULL\n");
		return -ENOSPC;
	}

	if(wcount > MAX_SIZE - wr_offset)
		wcount = MAX_SIZE - wr_offset;

	ret = copy_from_user(ubuf, pbuffer+wr_offset, wcount);
	if(ret)
	{
		printk("COPY FROM USER FAILED\n");
		return -EFAULT;
	}
	wr_offset += wcount;
	buflen += wcount;
	printk("PSEUDO --- WRITE METHOD\n");
	return 0;
}



struct file_operations fops={
	.open   = pseudo_open,
	.release= pseudo_close,
	.write  = pseudo_write,
	.read   = pseudo_read
};

static int __init char_init(void)
{
	pbuffer= kmalloc(MAX_SIZE,GFP_KERNEL);
	pclass = class_create(THIS_MODULE, "Pseudo_class");
	//for char driver alloaction
	ret = alloc_chrdev_region(&pdevID,0,ndevices, "char_DEVICE");
	if(ret)
	{
	printk("CHAR DRIVER: FAILED TO REGISTER DRIVER\n");
	return -EINVAL;
	}
	
	pbuffer = kmalloc(MAX_SIZE,GFP_KERNEL);
	if(pbuffer==NULL)
	{
		printk("KMALLOC FAILED\n");
	}

	//for oeprations initlization
	cdev_init(&cdev,&fops);
	kobject_set_name(&cdev.kobj, "pdevice%d",i);
	ret = cdev_add(&cdev,pdevID+i,1);
	
	pdev=device_create(pclass,NULL,pdevID+i,NULL,"psample%d",i);

	printk("SUCCESSFULLY REGISTERED, MAJOR=%d, MINOR=%d\n",MAJOR(pdevID), MINOR(pdevID));
	printk("welcome to char_driver\n");
	return 0;
}

static void __exit char_exit(void)
{
	kfree(pbuffer);
	device_destroy(pclass,pdevID);
	cdev_del(&cdev);
	unregister_chrdev_region(pdevID,ndevices);
	class_destroy(pclass);
	printk("Goodbye, char_driver\n");
}

module_init(char_init);
module_exit(char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NAGENDRA");
MODULE_DESCRIPTION("A SIMPLE CHARACTER DRIVER");

