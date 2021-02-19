#include <linux/module.h>

#include <linux/moduleparam.h>

#include <linux/cdev.h>

#include <linux/fs.h>

#include <linux/wait.h>

#include <linux/poll.h>

#include <linux/sched.h>

#include <linux/slab.h>


#define BUFFER_MAX    (10)

#define OK            (0)

#define ERROR         (-1)


struct cdev *gDev;

struct file_operations *gFile;

dev_t  devNum;

unsigned int subDevNum = 1;

int reg_major  =  232;    

int reg_minor =   0;

char *buffer;


static int trace_open(struct inode *p, struct file *f)

{

    printk(KERN_INFO "hello_open\r\n");

    return 0;

}


static ssize_t trace_write(struct file *f, const char __user *u, size_t s, loff_t *l)

{

    printk(KERN_INFO "hello_write\r\n");

    return 0;

}

static ssize_t trace_read(struct file *f, char __user *u, size_t s, loff_t *l)

{

    printk(KERN_INFO "hello_read\r\n");      

    return 0;

}

int trace_init(void)
{

    

    devNum = MKDEV(reg_major, reg_minor);

    if(OK == register_chrdev_region(devNum, subDevNum, "trace-chrdev")){

        printk(KERN_INFO "register_chrdev_region ok \n"); 

    }else {

    printk(KERN_INFO "register_chrdev_region error n");

        return ERROR;

    }

    printk(KERN_INFO " hello driver init \n");

    gDev = kzalloc(sizeof(struct cdev), GFP_KERNEL);

    gFile = kzalloc(sizeof(struct file_operations), GFP_KERNEL);

    gFile->open = trace_open;

    gFile->read = trace_read;

    gFile->write = trace_write;

    gFile->owner = THIS_MODULE;

    cdev_init(gDev, gFile);

    cdev_add(gDev, devNum, 1);

    return 0;

}


void __exit trace_exit(void)

{

  printk(KERN_INFO " hello driver exit \n");

    cdev_del(gDev);

    kfree(gFile);

    kfree(gDev);

    unregister_chrdev_region(devNum, subDevNum);

    return;

}

module_init(trace_init);

module_exit(trace_exit);

MODULE_LICENSE("GPL");