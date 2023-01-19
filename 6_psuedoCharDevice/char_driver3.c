#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/cdev.h>  // for all character devices
#include<linux/device.h>

struct device *pdev;
struct class *pclass;

dev_t pdevID;
int ndevices=1;
struct cdev cdev;
int ret,i;

int pseudo_open(struct inode*, struct file* file)
{
	printk("PSEUDO --OPEN METHOD\n");
	return 0;
}

int pseudo_close(struct inode*, struct file*file)
{
	printk("PSEUDO-- CLOSE METHOD\n");
	return 0;
}

ssize_t pseudo_read(struct file* file, char __user *buf, size_t size, loff_t *off)

{
	printk("PSEUDO--READ METHID\n");
	return 0;
}

ssize_t pseudo_write(struct file* file, const char __user *biuf,size_t size, loff_t *off)
{
	printk("PSEUDO--WRITE METHOD\n");
	return -ENOSPC;
}



struct file_operations fops={
	.open   = pseudo_open,
	.release= pseudo_close,
	.write  = pseudo_write,
	.read   = pseudo_read
};

static int __init char_init(void)
{
	pclass = class_create(THIS_MODULE, "Pseudo_class");
	//for char driver alloaction
	ret = alloc_chrdev_region(&pdevID,0,ndevices, "char_DEVICE");
	if(ret)
	{
	printk("CHAR DRIVER: FAILED TO REGISTER DRIVER\n");
	return -EINVAL;
	}

	//for oeprations initlization
	cdev_init(&cdev,&fops);
	kobject_set_name(&cdev.kobj, "pdevice%d",i);
	ret = cdev_add(&cdev,pdevID+i,1);
	
	pdev=device_create(pclass,NULL,pdevID+i,NULL,"psample %d",i);

	printk("SUCCESSFULLY REGISTERED, MAJOR=%d, MINOR=%d\n",MAJOR(pdevID), MINOR(pdevID));
	printk("welcome to char_driver\n");
	return 0;
}

static void __exit char_exit(void)
{
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



