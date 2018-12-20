#include <asm/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/thread_info.h>

MODULE_LICENSE("GPL");
static int file_device_init(void) { 
    char *filename = "/home/kadyr/temp";
	
    struct file* file;
    const struct file_operations* fop;
		
    if ((file = filp_open(filename, O_RDONLY,0)) == NULL)
	    return;
	fop = file->f_path.dentry->d_inode->i_fop;
	//printk("INIT FUNCTION HAHAH:\n");
	printk("open:%p release:%p read:%p write:%p", fop->open, fop->release, fop->read, fop->write);

    return 0;
}

static void file_device_exit(void) {
    return;
}

module_init(file_device_init);
module_exit(file_device_exit);

