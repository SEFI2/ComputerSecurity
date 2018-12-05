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

static int attempts_open = 0;
static int attempts_delete = 0;
static int failed_open = 0;
static int failed_delete = 0;
static char current_name[201];

static int thook_file_open(struct file *file,  const struct cred* cred) {
	int ret = 0;
	char buf[500];
	char* path;
	char* must_be = "/home/kadyr/secret";
	char* name_must_be = "bp";
		
	path = d_path(&file->f_path,buf,500);
	
	
	if (strncmp(path, must_be, strlen(must_be)) == 0) {
		attempts_open ++;	
		if (strncmp(current->comm, name_must_be, sizeof(name_must_be)) != 0) {
			failed_open ++;	
			ret = 1;			
		}
		
    		memcpy(current_name, current->comm, strlen(current->comm));
	}

	
		
    return ret;
}


static int thook_path_unlink(struct path *dir, struct dentry *dentry) {
int ret = 0;
	char buf[500];
	char data[500];
	char* path;
	char* must_be = "/home/kadyr/secret";
	char* name_must_be = "bp";
		
	path = d_path(dir,buf,500);
	int data_len = sprintf(data,"%s/%s", path, dentry->d_iname);
	
		memcpy(current_name, data, strlen(data));
			
	
	if (strncmp(data, must_be, strlen(must_be)) == 0) {
		attempts_delete ++;	
		if (strncmp(current->comm, name_must_be, sizeof(name_must_be)) != 0) {
			failed_delete ++;	
			ret = 1;			
		}
		
    	}

	
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


    int data_read = 0;
    char data[300];
    int data_len = sprintf(data, "Attempts to open: %d\nFailed to open: %d\nAttempts to unlink: %d\nFailed to unlink: %d\nLast program %s\nhahah\n", attempts_open, failed_open, attempts_delete, failed_delete, current_name);

    printk(KERN_INFO "%s: we are now in read function\n",__func__);
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
    current_name[200] = '\0';
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
