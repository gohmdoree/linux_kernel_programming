#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/yoofoo.h>

int errno;

asmlinkage int sys_yoofoo(){
	printk("YOO: Hello World!\n");

	return 1;
}
