#include<linux/module.h>
#include<linux/init.h>
#include<linux/interrupt.h>
#include<linux/kernel.h>

#define DRIVER_NAME "tasklet_irq_driver"
#define IRQ_NUM 1

static int irq_counter=0;
static void my_tasklet_fn(struct tasklet_struct *t){
	pr_info("tasklet: bottom half executed...\n");
	pr_info("count: %d\n",irq_counter);
}

DECLARE_TASKLET(my_tasklet,my_tasklet_fn);

static irqreturn_t interrupt_handler(int irq, void *dev_id){
	irq_counter++;
	pr_info("irq: keyboard interrupt occured...\n");
	tasklet_schedule(&my_tasklet);
	return IRQ_HANDLED;
}

static int __init tasklet_interrupt_init(void){
	int ret;
	ret=request_irq(IRQ_NUM,interrupt_handler,IRQF_SHARED,DRIVER_NAME,(void*)interrupt_handler);
	if(ret){
		pr_err("failed to request IRQ %d\n",IRQ_NUM);
		return ret;
	}
	pr_info("tasklet module is loaded...\n");
	return 0;
}
static void __exit tasklet_interrupt_exit(void){
	tasklet_kill(&my_tasklet);
	free_irq(IRQ_NUM,(void*)interrupt_handler);
	pr_info("tasklet module is unloaded...\n");
}

module_init(tasklet_interrupt_init);
module_exit(tasklet_interrupt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEJA");
MODULE_DESCRIPTION("BASIC TASKLET INTERRUPT DRIVER IN KERNEL");

