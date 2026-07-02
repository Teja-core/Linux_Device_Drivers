#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/interrupt.h>

#define IRQ_NUMBER 1
#define DRIVER_NAME "irq_demo_driver"

static int irq_counter=0;

static irqreturn_t irq_demo_isr(int irq, void * dev_id){
	irq_counter++;
	pr_info("%s: interrupt recieved : IRQ=%d count=%d\n",DRIVER_NAME,IRQ_NUMBER,irq_counter);
	return IRQ_HANDLED;
}

static int __init my_init(void){
	int ret;
	pr_info("%s : Initialization is done\n",DRIVER_NAME);
	ret=request_irq(IRQ_NUMBER,irq_demo_isr,IRQF_SHARED,DRIVER_NAME,(void *)irq_demo_isr);
	if(ret){
		pr_err("%s: Failed to request ISR %d\n",DRIVER_NAME,IRQ_NUMBER);
		return ret;
	}
	pr_info("%s IRQ %d registered successfully..\n",DRIVER_NAME,IRQ_NUMBER);
	return 0;
}

static void __exit my_exit(void){
	pr_info("%s: cleaning up\n",DRIVER_NAME);
	free_irq(IRQ_NUMBER, (void *)irq_demo_isr);
	pr_info("%s :IRQ freed \n",DRIVER_NAME);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEJA");
MODULE_DESCRIPTION("sample IRQ handling example in linux kernel");

