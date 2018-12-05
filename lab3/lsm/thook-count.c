#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/security.h>
#include <linux/dcache.h>
#include <linux/sched.h>

/*
 * This implements a minimal LSM that hooks file opens.
 * Start to read the code from thook_init.
 */

MODULE_LICENSE("GPL");


/*
 * This is our skeleton hook that does nothing but returning 0, which
 * means that it always allows the access.
 *
 * Please note that you should not call any printing function (e.g. printk)
 * from withing this, because it may open a file again and cause an endless
 * recursion.
 */

static int file_open_count = 0;

static int thook_file_open(struct file *file,  const struct cred* cred) {
    int ret = 0;
	char buf[500];
	char* path;
	path = d_path(&file->f_path,buf,500);
	char* must_be = "/home/kadyr/secret";			
	int len = 17;
	int i = 0;
	int same = 1;
	

	for (i = 0 ; i < len ; ++i) {
		if (path[i] == '\0' || path[i] != must_be[i]) {
			same = 0;			
			break;		
		}	
	}
	if (same == 1) 
		file_open_count ++;
	
		
    return ret;
}


static int thook_path_unlink(struct path *dir, struct dentry *dentry) {
    int ret = 0;
    return ret;
}

/*
 * This is an object that defines the list of our hooks.
 * The Linux kernel core will place the function pointers in this object
 * at appropriate locations so that they are called when needed.
 * In particular, security_file_open @ security/security.c:804 calls
 * our hook, which is called @ fs/open.c:712.
 *
 */
static struct security_operations thook_ops = {
    .file_open = thook_file_open,
    .path_unlink = thook_path_unlink,
};


/*
 * __init at the beginning and the security_initcall(thook_init) informs the
 * kernel core that this function should be called to initialize our thook
 * module at the kernel bootstrap. As you see, this function calls
 * register_security (@security/security.c:121)
 * too insert the hooks that the thook_ops describes.
 */
static int __init thook_init(void) {
    pr_info("Hello World! From %s\n",__func__);
    register_security(&thook_ops);
    return 0;
}


security_initcall(thook_init);


#define DEVICE_NAME "thook"
#define CLASS_NAME "thook"

static int thook_open(struct inode *inodep, struct file *filep);
static int thook_release(struct inode *inodep, struct file *filep);
static int thook_read(struct file *filep, char* buffer,
        size_t len, loff_t *offset);


static struct file_operations fops = {
    .open = thook_open,
    .release = thook_release,
    .read = thook_read,
};

static int major_number;
static struct class* thook_class;
static struct device* thook_device;

static int is_open;

static int thook_open(struct inode *inodep, struct file *filep) {
    if (is_open == 0) {
        printk(KERN_INFO "thook opens\n");
        is_open = 1;
        return 0;
    }
    else {
        printk(KERN_INFO "thook is open already\n");
        return -EALREADY;
    }
}

static int thook_release(struct inode *inodep, struct file *filep) {
    if (is_open == 1) {
        printk(KERN_INFO "thook is being released\n");
        is_open = 0;
    }
    else {
        printk(KERN_INFO "thook is not open?\n");
    }
    return 0;
}

/*
 *
 *
 */

static int thook_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {


    printk(KERN_INFO "%s: we are now in read function\n",__func__);
    int data_read = 0;
    char data[70];
    int data_len = sprintf(data, "/home/vagrant/secret has been opened %d times\0", file_open_count);

    //memcpy(buffer, data, strlen(data));
    if (offset == NULL) {
	printk("offset is null\n");
	return -EINVAL;
   }
    if (*offset >= data_len) {
	printk("offset is greater than four\n"); 
	return -EINVAL;
    }


    while (*offset < data_len && data_read < len) {
	put_user(data[*offset], &buffer[data_read]);	
	data_read ++;    
        *offset = *offset + 1;
    }
    printk("%s, Read data: %s\n",__func__, data);
		
    return data_read;
}


/*
 * This is another function that is called at boot time.
 * This registers the thook device and creates the corresponding device
 * file called /dev/thook so that user programs (with root) can interact
 * with this module.
 */

static int __init thook_device_init(void) {
	
    is_open = 0;
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "Failed to get a major number\n");
        return major_number;
    }
    printk(KERN_INFO "%s: YEAH MFK, got major number\n",__func__);
    thook_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(thook_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class for thook\n");
        return PTR_ERR(thook_class);
    }
    thook_device = device_create(thook_class, NULL,
            MKDEV(major_number, 0),
            NULL,
            DEVICE_NAME);
    if(IS_ERR(thook_device)) {
        class_destroy(thook_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device for thook\n");
        return PTR_ERR(thook_device);
    }
    pr_info("hahaha called %s\n",__func__);
    //printk(KERN_INFO "create the thook device!\n");
    return 0;
}

__initcall(thook_device_init);
