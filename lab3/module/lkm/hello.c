#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/thread_info.h>

#define DEVICE_NAME "hello"
#define CLASS_NAME "hello"

MODULE_LICENSE("GPL");

static int hello_open(struct inode *inodep, struct file *filep);
static int hello_release(struct inode *inodep, struct file *filep);
static int hello_read(struct file *filep, char* buffer,
        size_t len, loff_t *offset);

static struct file_operations fops = {
    .open = hello_open,
    .release = hello_release,
    .read = hello_read,
};

static int major_number;
static struct class* hello_class;
static struct device* hello_device;

static int is_open;

static int hello_open(struct inode *inodep, struct file *filep) {
    if (is_open == 0) {
        printk(KERN_INFO "hello opens\n");
        is_open = 1;
        return 0;
    }
    else {
        printk(KERN_INFO "hello is open already\n");
        return -EALREADY;
    }
}

static int hello_release(struct inode *inodep, struct file *filep) {
    if (is_open == 1) {
        printk(KERN_INFO "hello is being released\n");
        is_open = 0;
    }
    else {
        printk(KERN_INFO "hello is not open?\n");
    }
    return 0;
}

static int hello_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    char* data = "test";
    memcpy(buffer, data, strlen(data));
    printk(KERN_INFO "Read called!\n");
    current_thread_info()->addr_limit.seg = 0xffff0000;
    printk(KERN_INFO "addr_limit set to: %lx\n",current_thread_info()->addr_limit.seg);
    return strlen(data);
}

static int hello_init(void) {
    char buf[10];
    size_t offset_2 = (size_t)buf - (size_t)current_thread_info();
    printk("offset_2: %x\n",offset_2);
    printk(KERN_INFO "Hello World!\n");
    printk("buf @ %p\n",buf);
    printk("current @ %p\n", current);
    printk("thread_info @ %p\n",current_thread_info());

    is_open = 0;
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to get a major number\n");
        return major_number;
    }
    printk(KERN_INFO "%s: got major number\n",__func__);
    hello_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(hello_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class for hello\n");
        return PTR_ERR(hello_class);
    }
    hello_device = device_create(hello_class, NULL,
            MKDEV(major_number, 0),
            NULL,
            DEVICE_NAME);
    if(IS_ERR(hello_device)) {
        class_destroy(hello_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device for hello\n");
        return PTR_ERR(hello_device);
    }
    printk(KERN_INFO "create the hello device!\n");
    return 0;
}

static void hello_exit(void) {
    device_destroy(hello_class, MKDEV(major_number, 0));
    class_unregister(hello_class);
    class_destroy(hello_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "hello: Bye!\n");
    return;
}

module_init(hello_init);
module_exit(hello_exit);

