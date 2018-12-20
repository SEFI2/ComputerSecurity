/*
        vfs_hook.c           author : daehee
	updated: hyungon
        hook some kernel vfs pointer(/mnt/root/data/data/snu.csor.towelroot/malicious.file)
*/

//#include <string.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fdtable.h>
#include <linux/namei.h>
#include <linux/slab.h>

#include <asm/cacheflush.h>
#include <linux/mm.h>

#include <asm/io.h>
static void **orig_val = NULL;

ssize_t my_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	printk ("sdfasdfasdf\n");

	char* data = "test\0";
    int data_read = 0;
    //memcpy(buffer, data, strlen(data));
    if (offset == NULL) {
	printk("offset is null\n");
	return -EINVAL;
   }
    if (*offset >= 5) {
	printk("offset is greater than four\n"); 
	return -EINVAL;
    }

    while (*offset < 5 && data_read < len) {
	put_user(data[*offset], &buffer[data_read]);	
	data_read ++;    
        *offset = *offset + 1;
    }
    printk("Read data: %s\n", data);
    return data_read;
}

static void hook_read(char *filename){

	
    struct file *file;

printk("SUKKSDFSDFK");
	file = filp_open(filename, O_RDONLY,0);

    if (IS_ERR(file))
	    return;
printk("dfgdfGDFG");

	struct file_operations *fop = kmalloc(sizeof(struct file_operations), GFP_KERNEL);

	
	fop->owner = file->f_op->owner;
	fop->llseek = file->f_op->llseek;
	fop->read = NULL; // here
	fop->write = file->f_op->write;
	fop->aio_read = NULL;
	fop->aio_write = file->f_op->aio_write;
	fop->read_iter = file->f_op->read_iter;
	fop->write_iter = file->f_op->write_iter;
	fop->iterate = file->f_op->iterate;
						
	fop->poll = file->f_op->poll;
	fop->unlocked_ioctl = file->f_op->unlocked_ioctl;
	fop->compat_ioctl = file->f_op->compat_ioctl;
	fop->mmap = file->f_op->mmap;
	fop->open = file->f_op->open;
	fop->flush = file->f_op->flush;
	fop->release = file->f_op->release;
	fop->fsync = file->f_op->fsync;
	fop->aio_fsync = file->f_op->aio_fsync;
	fop->fasync = file->f_op->fasync;
	fop->lock = file->f_op->lock;
	fop->sendpage = file->f_op->sendpage;
	fop->get_unmapped_area = file->f_op->get_unmapped_area;
	fop->check_flags = file->f_op->check_flags;
	fop->setfl = file->f_op->setfl;	
	fop->flock = file->f_op->flock;
	fop->splice_write = file->f_op->splice_write;
	fop->splice_read = file->f_op->splice_read;
	fop->setlease = file->f_op->setlease;
	fop->fallocate = file->f_op->fallocate;
	fop->show_fdinfo = file->f_op->show_fdinfo;
				
		

	file->f_op = fop;
	printk("HAHA finally done\n");

	  filp_close(file, NULL);
	
}


static int __init init(void){
	printk("vfshook init\n");
		hook_read("/home/kadyr/malicious");
        return 0;
}

static void __exit exit(void){
	printk("vfshook exit\n");
        return;
}
MODULE_LICENSE("GPL");
module_init(init);
module_exit(exit);
