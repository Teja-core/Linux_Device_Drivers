#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/cdev.h>

#define DEVICE_NAME "ioctl_dev"
#define MY_IOCTL_MAGIC 'T'
#define SET_VALUE _IOW(MY_IOCTL_MAGIC, 1, int)
#define GET_VALUE _IOR(MY_IOCTL_MAGIC,2,int)


static dev_t dev_num;
static struct cdev basic_cdev;
static struct class * basic_class;
static int k_value=0;

static int my_open(struct inode *inode, struct file *file){
	pr_info("ioctl_dev: is opened successfully...\n");
	return 0;
}

static int my_close(struct inode * inode, struct file * file){
	pr_info("ioctl_dev: closed successfully...\n");
	return 0;
}

static long my_ioctl(struct file * file, unsigned int cmd, unsigned long arg){
	switch (cmd){
		case SET_VALUE:
			if(copy_from_user(&k_value,(int __user*) arg,sizeof(k_value)))
				return -EFAULT;
			pr_info("set value=%d \n",k_value);
		break;
		case GET_VALUE:
			if(copy_to_user((int __user*)arg,&k_value,sizeof(k_value)))
				return -EFAULT;
			pr_info("get value=%d\n",k_value);
		break;
		default :
			return -EINVAL;
	}
	return 0;
}
static struct file_operations fops={
	.owner=THIS_MODULE,
	.open=my_open,
	.release=my_close,
	.unlocked_ioctl=my_ioctl,
};
static int __init my_init(void){
	int ret;
	ret=alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
	if(ret)
		return ret;
	cdev_init(&basic_cdev,&fops);
	ret=cdev_add(&basic_cdev,dev_num,1);
	if(ret)
		goto unregister_region;
	basic_class=class_create(THIS_MODULE,DEVICE_NAME);
	if(IS_ERR(basic_class)){
		ret=PTR_ERR(basic_class);
		goto del_cdev;
	}
	if(IS_ERR(device_create(basic_class,NULL,dev_num,NULL,DEVICE_NAME)))
	{
		ret=-EINVAL;
		goto destroy_class;
	}
	pr_info("ioctl_dev: major=%d and minor=%d\n",MAJOR(dev_num),MINOR(dev_num));
	return 0;

	destroy_class:
		class_destroy(basic_class);
	del_cdev:
		cdev_del(&basic_cdev);
	unregister_region:
		unregister_chrdev_region(dev_num,1);
	
	return ret;
}

static void __exit my_exit(void)
{
	device_destroy(basic_class,dev_num);
	class_destroy(basic_class);
	cdev_del(&basic_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TEJA");
MODULE_DESCRIPTION("BASIC IOCTL DRIVER IN KENRL");
