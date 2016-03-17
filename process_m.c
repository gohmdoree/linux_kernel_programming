#define MODULE
#define __KERNEL__
#define PAGE_SHIFT		12
#define PAGE_SIZE		(1UL << PAGE_SHIFT)
#define PAGE_MASK		(~(PAGE_SIZE-1))
#define YOO_PAGE_ALIGN(addr)	(((addr)+PAGE_SIZE-1)&(PAGE_MASK))
#define PAGE_CNT(num1,num2)	(YOO_PAGE_ALIGN(num1-num2)/PAGE_SIZE)
#define SUCCESS 		0
#define DEVICE_NAME		"mydev"

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

static int __init init_c_module(void){
	int count = 0;
	struct task_struct *pi = NULL;

	printk("YOO: init_module\n");
	for_each_task(pi)
	{
		if(pi->mm){
			printk("YOO: PROCESS ADDRESS SPACE\n");
			printk("YOO: #############################################\n");
			printk("YOO: Process Memory for PID [%d] TGID [%d]\n", pi->pid, pi->tgid);
			printk("YOO: [%s]\n", pi->comm);
			printk("YOO: code address, [%lu]PAGES: [%lu] - [%lu]\n", PAGE_CNT(pi->mm->end_code, pi->mm->start_code), pi->mm->start_code, pi->mm->end_code);
			printk("YOO: data address, [%lu]PAGES: [%lu] - [%lu]\n", PAGE_CNT(pi->mm->end_data, pi->mm->start_data), pi->mm->start_data, pi->mm->end_data);
			printk("YOO: heap address, [%lu]PAGES: [%lu] - [%lu]\n", PAGE_CNT(pi->mm->brk, pi->mm->start_brk), pi->mm->start_brk, pi->mm->brk);

			struct vm_area_struct *mmap = NULL;
			long unsigned start_addr = 0;
			count = 0;
			for(mmap = pi->mm->mmap; (mmap) && (start_addr != mmap->vm_start); mmap = mmap->vm_next_share){
				if(count == 0){
					start_addr = pi->mm->mmap->vm_start;
					count = 1;
				}
				printk("YOO: shared libraries, [%lu] pages: [%lu] - [%lu]\n", PAGE_CNT(mmap->vm_end, mmap->vm_start), mmap->vm_start, mmap->vm_end);
			}
			printk("YOO: stack address, [%lu] pages: [%lu] - [%x]\n", PAGE_CNT(0xbfffffff, pi->mm->start_stack), pi->mm->start_stack, 0xbfffffff);

			if(pi->mm->mmap->vm_flags & VM_READ){
				printk("Pages can be read\n");
			}
			if(pi->mm->mmap->vm_flags & VM_WRITE){
				printk("Pages can be write\n");
			}
			if(pi->mm->mmap->vm_flags & VM_EXEC){
				printk("Pages can be exec\n");
			}
			if(pi->mm->mmap->vm_flags & VM_SHARED){
				printk("Pages can be shared\n");
			}
		}
		printk("YOO: #############################################\n");
	}
	return 0;
}

static void __exit cleanup_c_module(void){
	printk("YOO: cleanup module\n");
}

module_init(init_c_module);
module_exit(cleanup_c_module);
MODULE_LICENSE("GPL");
