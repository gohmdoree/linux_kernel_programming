#define MODULE
#define __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
MODULE_LICENSE("GPL");

static short int myid = 1;
static int mystreet = 100;
static long int myzip = 90006;
static char *myname = "Foo Bar";

MODULE_PARM(myid, "h");
MODULE_PARM(mystreet, "i");
MODULE_PARM(myzip, "l"); 
MODULE_PARM(myname, "s");

static int init_hello_5(void){
	printk(KERN_ALERT "kernel: Hello World 5\n");
	printk(KERN_ALERT "kernel: ID number : %hd\n", myid);
	printk(KERN_ALERT "kernel: Street : %d\n", mystreet);
	printk(KERN_ALERT "kernel: ZIP code : %ld\n", myzip);
	printk(KERN_ALERT "kernel: Name : %s\n", myname);
	return 0;
}

static void cleanup_hello_5(void){
	printk(KERN_ALERT "kernel: Goodbye World 5\n");
}

module_init(init_hello_5);
module_exit(cleanup_hello_5);

