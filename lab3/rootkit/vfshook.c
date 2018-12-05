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

#include <asm/cacheflush.h>
#include <linux/mm.h>

#include <asm/io.h>
ssize_t my_read(struct file * a, char __user * b, size_t c, loff_t * d)
{
    int ret = 0; 
    return ret;
}

static void hook_read(char *filename){

    struct file *file;
    file = file;
    return;
}


static int __init init(void){

        return 0;
}

static void __exit exit(void){
        return;
}
MODULE_LICENSE("GPL");
module_init(init);
module_exit(exit);
