#define MODULE
#define __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void){
	printk(KERN_ALERT "kernel: Hello World 1\n");
	return 0;
}

void cleanup_module(void){
	printk(KERN_ALERT "kernel: Goodbye World 1\n");
}
