#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/uaccess.h>

MODULE_AUTHOR("Nikita Krylov");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION( "My second/finished scull driver");

static ssize_t scull_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t scull_write(struct file *, const char __user *, size_t, loff_t *);
static int scull_open(struct inode *, struct file *);
static int scull_release(struct inode *, struct file *);

int scull_minor = 0;
int scull_major = 0;

struct char_device {
	char data[100];
} device;

struct cdev *p_cdev;

static struct file_operations scull_fops = {		
	.read = scull_read,
	.write = scull_write,
	.open = scull_open,
	.release = scull_release,
};

static ssize_t scull_read(struct file *flip, char __user *buf, size_t count, loff_t *f_pos)
{
	int tmp;

	printk(KERN_INFO "Scull: read from device\n");

	tmp = copy_to_user(buf, device.data, count);
	return tmp;
}

static ssize_t scull_write(struct file *flip, const char __user *buf, size_t count, loff_t *f_pos)
{
	int tmp;

	printk(KERN_INFO "Scull: write to device\n");

	tmp = copy_from_user(device.data, buf, count);
	return tmp;
}

static int scull_open(struct inode *inode, struct file *flip)
{
	printk(KERN_INFO "Scull: device is opened\n");
	return 0;
}

static int scull_release(struct inode *inode, struct file *flip)
{
	printk(KERN_INFO "Scull: device is closed\n");
	return 0;
}

static int scull_init_module(void)
{
	int tmp;
	dev_t dev;

	tmp = alloc_chrdev_region(&dev, scull_minor, 1, "scull");	

	if (tmp) 
    {
		printk(KERN_WARNING "Scull: can't get major %d\n", scull_major);
		return tmp;
	}

	scull_major = MAJOR(dev);

	p_cdev = cdev_alloc();
	cdev_init(p_cdev, &scull_fops);

	//p_cdev->owner = THIS_MODULE;
	p_cdev->ops = &scull_fops;

	tmp = cdev_add(p_cdev, dev, 1);

	if (tmp)
		printk(KERN_NOTICE "Error %d adding scull", tmp);

	printk(KERN_INFO "Scull: register device major = %d minor = %d\n", scull_major, scull_minor);
    printk(KERN_INFO "Please, create a dev file with << mknod /dev/scull c %d %d >> \n", scull_major, scull_minor);
	return 0;
}

static void scull_cleanup_module(void)
{
	dev_t devno = MKDEV(scull_major, scull_minor);
	cdev_del(p_cdev);
	unregister_chrdev_region(devno, 1);

    printk( KERN_ALERT "Scull: unregister device\n" );
}

module_init(scull_init_module);
module_exit(scull_cleanup_module);