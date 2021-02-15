
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define ARDUINO_DEV_NAME "arduino"
static struct class *arduino_class = NULL;
static dev_t dev = 0;

struct arduino_i2c_cdev {
    struct i2c_client *client;
    struct cdev cdev;
};

static ssize_t arduino_i2c_open(struct inode *inode, struct file *filp)
{
    struct arduino_i2c_cdev *arduino = container_of(inode->i_cdev, struct arduino_i2c_cdev, cdev);
    if (!arduino) {
    pr_err("Cannot extrace aduino structure from i_cdev.\n");
return -EINVAL;
    }
    filp -> private_data = arduino -> client;
    return 0;
}

static ssize_t arduino_i2c_write(struct file *filp, const char __user *buf, size_t count,
loff_t *offset)
{
    struct i2c_client *client = filp -> private_data;
    if (!client) {
    pr_err("Failed to get struct i2c_client.\n");
return -EINVAL;
    }
    for (int i = 0; i < count; i++) {
        i2c_smbus_write_byte(client, buf[i]);
    }
    return 0;
}

struct file_operations arduino_i2c_fops = {
    .open = arduino_i2c_open,
    .write = arduino_i2c_write,
};


static int dummy_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    pr_info("Dummy device is being probed.\n");
/*
    char *info = "Hello, Arduino!";
    for (int i = 0; i < strlen(info); i++) {
      i2c_smbus_write_byte (client, info[i]);
    }
*/
    int err = 0;

    err = alloc_chrdev_region(&dev, 0, 1, ARDUINO_DEV_NAME);
    if (err < 0) {
        pr_err ("Failed in alloc_chrdev_reion for arduino.\n");
goto out_alloc_chrdev;
    }

    arduino_class = class_create(THIS_MODULE, ARDUINO_DEV_NAME);
    if (!arduino_class) {
    pr_err ("Failed to create sysfs class.\n");
goto out_sysfs_class;
    }

    struct arduino_i2c_cdev *arduino = kzalloc(sizeof(struct arduino_i2c_cdev), GFP_KERNEL);
    if (!arduino) {
pr_err("Failed to allocate memory.\n");
    goto out_oom;
    }
    arduino -> client = client;

    cdev_init(&(arduino -> cdev), &arduino_i2c_fops);
    arduino->cdev.owner = THIS_MODULE;
    err = cdev_add(&(arduino -> cdev), dev, 1);
    if (err) {
pr_err("Failed to register cdev.\n");
    goto out_cdev_add;
    }

    struct device *device = device_create(arduino_class, NULL, dev, NULL, ARDUINO_DEV_NAME);
    if (!device) {
    pr_err("Failed to create device entry under sysfs.\n");
goto out_device;
    }
    i2c_set_clientdata(client, arduino);
    return 0;

out_device:
    cdev_del(&arduino->cdev);
out_cdev_add:
    kfree(arduino);
out_oom:
    class_destroy(arduino_class);
out_sysfs_class:
    unregister_chrdev_region(dev, 1);
out_alloc_chrdev:
    return err;    
}

static int dummy_remove(struct i2c_client *client)
{
    pr_info("Dummy device is removing.\n");
    struct arduino_i2c_cdev *arduino = i2c_get_clientdata(client);
    device_destroy(arduino_class, dev);
    cdev_del(&(arduino->cdev));
    kfree(arduino);
    class_destroy(arduino_class);
    unregister_chrdev_region(dev, 1);
    return 0;
}

static struct of_device_id dummy_id_tables [] = {
    { .compatible="arduino", },
    { }
};

MODULE_DEVICE_TABLE(of, dummy_id_tables);

static struct i2c_driver dummy_drv = {
    .probe = dummy_probe,
    .remove = dummy_remove,
    .driver = {
    .name = "dummy device 0.1",
.owner = THIS_MODULE,
.of_match_table = dummy_id_tables,
    },
};

module_i2c_driver(dummy_drv);
MODULE_LICENSE("GPL");
