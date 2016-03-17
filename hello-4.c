#define MODULE
#define __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int init_hello_4(void){
	printk(KERN_ALERT "kernel: Hello World 4\n");
	return 0;
}

static void cleanup_hello_4(void){
	printk(KERN_ALERT "kernel: Goodbye World 4\n");
}

module_init(init_hello_4);
module_exit(cleanup_hello_4);

MODULE_LICENSE("GPL");
