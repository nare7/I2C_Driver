/*
 * I2C Character Driver for Raspberry Pi 4
 * Communicates with STM32F401RE slave at address 0x30
 * Sends data byte 0xAA
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define DRIVER_NAME "i2c_stm32"
#define DEVICE_NAME "i2c_stm32"
#define STM32_I2C_ADDR 0x30

static dev_t dev_number;
static struct class *dev_class;
static struct cdev my_cdev;
static struct i2c_client *stm32_client;
static struct i2c_adapter *i2c_adapter;

/* Function to write data to STM32 */
static int stm32_i2c_write(struct i2c_client *client, uint8_t *data, uint8_t len)
{
    int ret;
    struct i2c_msg msg;
    
    msg.addr = client->addr;
    msg.flags = 0; /* Write operation */
    msg.len = len;
    msg.buf = data;
    
    ret = i2c_transfer(client->adapter, &msg, 1);
    
    if (ret < 0) {
        pr_err("I2C write failed: %d\n", ret);
        return ret;
    }
    
    pr_info("I2C write successful, sent %d bytes\n", len);
    return 0;
}

/* Function to read data from STM32 */
static int stm32_i2c_read(struct i2c_client *client, uint8_t *data, uint8_t len)
{
    int ret;
    struct i2c_msg msg;
    
    msg.addr = client->addr;
    msg.flags = I2C_M_RD; /* Read operation */
    msg.len = len;
    msg.buf = data;
    
    ret = i2c_transfer(client->adapter, &msg, 1);
    
    if (ret < 0) {
        pr_err("I2C read failed: %d\n", ret);
        return ret;
    }
    
    pr_info("I2C read successful, received %d bytes\n", len);
    return 0;
}

/* File operations - open */
static int my_open(struct inode *inode, struct file *file)
{
    pr_info("Device opened\n");
    return 0;
}

/* File operations - close */
static int my_release(struct inode *inode, struct file *file)
{
    pr_info("Device closed\n");
    return 0;
}

/* File operations - read */
static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    uint8_t data[256];
    int ret;
    
    if (len > 256)
        len = 256;
    
    if (!stm32_client) {
        pr_err("I2C client not initialized\n");
        return -ENODEV;
    }
    
    ret = stm32_i2c_read(stm32_client, data, len);
    if (ret < 0)
        return ret;
    
    if (copy_to_user(buf, data, len)) {
        pr_err("Failed to copy data to user space\n");
        return -EFAULT;
    }
    
    pr_info("Read %zu bytes from STM32\n", len);
    return len;
}

/* File operations - write */
static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *off)
{
    uint8_t *data;
    int ret;
    
    if (len > 256)
        len = 256;
    
    if (!stm32_client) {
        pr_err("I2C client not initialized\n");
        return -ENODEV;
    }
    
    data = kmalloc(len, GFP_KERNEL);
    if (!data)
        return -ENOMEM;
    
    if (copy_from_user(data, buf, len)) {
        pr_err("Failed to copy data from user space\n");
        kfree(data);
        return -EFAULT;
    }
    
    ret = stm32_i2c_write(stm32_client, data, len);
    kfree(data);
    
    if (ret < 0)
        return ret;
    
    pr_info("Wrote %zu bytes to STM32\n", len);
    return len;
}

/* File operations structure */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_release,
    .read = my_read,
    .write = my_write,
};

/* Module initialization */
static int __init i2c_driver_init(void)
{
    int ret;
    
    pr_info("I2C Character Driver Loading...\n");
    
    /* Allocate device number */
    ret = alloc_chrdev_region(&dev_number, 0, 1, DRIVER_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate device number\n");
        return ret;
    }
    pr_info("Device number allocated: Major=%d, Minor=%d\n", 
            MAJOR(dev_number), MINOR(dev_number));
    
    /* Initialize cdev structure */
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;
    
    /* Add cdev to kernel */
    ret = cdev_add(&my_cdev, dev_number, 1);
    if (ret < 0) {
        pr_err("Failed to add cdev\n");
        unregister_chrdev_region(dev_number, 1);
        return ret;
    }
    
    /* Create device class */
    dev_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(dev_class)) {
        pr_err("Failed to create device class\n");
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(dev_class);
    }
    
    /* Create device */
    if (IS_ERR(device_create(dev_class, NULL, dev_number, NULL, DEVICE_NAME))) {
        pr_err("Failed to create device\n");
        class_destroy(dev_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);
        return -1;
    }
    
    /* Get I2C adapter for i2c-1 */
    i2c_adapter = i2c_get_adapter(1);
    if (!i2c_adapter) {
        pr_err("Failed to get I2C adapter\n");
        device_destroy(dev_class, dev_number);
        class_destroy(dev_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);
        return -ENODEV;
    }
    
    /* Create I2C client */
    struct i2c_board_info board_info = {
        I2C_BOARD_INFO("stm32_slave", STM32_I2C_ADDR)
    };
    
    stm32_client = i2c_new_client_device(i2c_adapter, &board_info);
    if (IS_ERR(stm32_client)) {
        pr_err("Failed to create I2C client\n");
        i2c_put_adapter(i2c_adapter);
        device_destroy(dev_class, dev_number);
        class_destroy(dev_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev_number, 1);
        return PTR_ERR(stm32_client);
    }
    
    pr_info("I2C Character Driver Loaded Successfully\n");
    pr_info("Device created: /dev/%s\n", DEVICE_NAME);
    
    return 0;
}

/* Module exit */
static void __exit i2c_driver_exit(void)
{
    pr_info("I2C Character Driver Unloading...\n");
    
    /* Cleanup I2C client */
    if (stm32_client)
        i2c_unregister_device(stm32_client);
    
    if (i2c_adapter)
        i2c_put_adapter(i2c_adapter);
    
    /* Cleanup character device */
    device_destroy(dev_class, dev_number);
    class_destroy(dev_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_number, 1);
    
    pr_info("I2C Character Driver Unloaded\n");
}

module_init(i2c_driver_init);
module_exit(i2c_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("I2C Character Driver for STM32F401RE Slave");
MODULE_VERSION("1.0");
