#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/thread_info.h>

#define DEVICE_NAME "hello_device"
#define CLASS_NAME "hello_device"

MODULE_LICENSE("GPL");

// We first declare the functions
// that will handle accesses to the device from the user program.
static int hello_device_open(struct inode *inodep, struct file *filep);
static int hello_device_release(struct inode *inodep, struct file *filep);
static int hello_device_read(struct file *filep, char* buffer,
        size_t len, loff_t *offset);
static int hello_device_write(struct file *filep, char *buffer, size_t len, loff_t *offset);

// This is a struct containing our function pointers.
// WE can put much more, but here we use a small one like this.
static struct file_operations fops = {
    .open = hello_device_open,
    .release = hello_device_release,
    .read = hello_device_read,
    .write = hello_device_write,
};	

static int major_number;
static struct class* hello_class;
static struct device* hello_device;
static int is_open;

// This will be called when our device file is opened.
// The kernel will call this function instead of the others
// as we register it at init.
static int hello_device_open(struct inode *inodep, struct file *filep) {
    if (is_open == 0) {
	char buf[30];
	char* path;
	char* must_be = "/dev/hello_device";			
	path = d_path(&filep->f_path,buf,30);
	pr_info("Hello Open, path: %s\n", path);
	
	if (strncmp(path, must_be, sizeof(must_be)) == 0) {
		pr_info ("The path is correct!\n");
	} else {
		pr_info ("The path is not correct\n");
	}	
	
        is_open = 1;
        return 0;
    }
    else {
        pr_info("hello is open already\n");
        return -EALREADY;
    }
}

// This will be called when the file is closed.
// Our handler records that the file is closed.
static int hello_device_release(struct inode *inodep, struct file *filep) {
    if (is_open == 1) {
        pr_info("hello is being released\n");
        is_open = 0;
    }
    else {
        pr_info("hello is not open?\n");
    }
    return 0;
}

// This is our simple read handler.
// It will always be read as test.
static int hello_device_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    char* data = "test\0";
    //memcpy(buffer, data, strlen(data));
    if (offset == NULL) {
	printk("offset is null\n");
	return -EINVAL;
   }
    if (*offset >= 5) {
	printk("offset is greater than four\n"); 
	return -EINVAL;
    }
    int data_read = 0;

    while (*offset < 5 && data_read < len) {
	put_user(data[*offset], &buffer[data_read]);	
	data_read ++;    
        *offset = *offset + 1;
    }
    printk("Read data: %s\n", data);
		
    return data_read;
}



static int hello_device_write(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    if (offset == NULL)
	return -EINVAL;
    if (*offset >= 10)
	return -EINVAL;
           
    int data_written = 0;
    char data[11];
    data[10] = '\0';
  
    while (*offset < 10 && data_written < len) {
        get_user(data[*offset], &buffer[data_written]);
	*offset = *offset + 1;
        data_written ++;			 		 		    
    }

    while (data_written < len) {
	char temp;
        get_user(temp, &buffer[data_written]);
	*offset = *offset + 1; 
	data_written ++;   		
    }
    printk("Written data: %s\n", data);

    return data_written;
}

static int hello_device_init(void) {
    
    // This is a global variable that our handlers will use.
    is_open = 0;


    // The following is an idiomatic way of registering a device and
    // creating a corresponding device file.

    // We first need to obtain a device number (called major number).
    // the third argument is a struct of function pointers
    // which lets the kernel know which function it should call
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to get a major number\n");
        return major_number;
    }
    printk(KERN_INFO "%s: got major number\n",__func__);


    // Then create a new device class
    hello_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(hello_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class for hello\n");
        return PTR_ERR(hello_class);
    }

    // This will a device file under /dev with which we can communicate
    // with this module.
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

static void hello_device_exit(void) {
    // Our destructor now needs to remove the device file
    // and unregister the device file.
    device_destroy(hello_class, MKDEV(major_number, 0));
    class_unregister(hello_class);
    class_destroy(hello_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "hello: Bye!\n");
    return;
}

module_init(hello_device_init);
module_exit(hello_device_exit);

