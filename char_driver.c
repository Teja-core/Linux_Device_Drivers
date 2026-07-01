#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "basic_char"
#define BUF_SIZE 256

static dev_t dev_num; 
static struct cdev basic_cdev;
static struct class *basic_class;

static char k_buf[BUF_SIZE];
static int buffer_size;

static int my_open(struct inode * inode, struct file *file)
{
	pr_info("basic_char: opened succesfully...\n");
	return 0;
}
static int my_close(struct inode * inode, struct file * file)
{
	pr_info("basic_char: closed successfully...\n");
	return 0;
}
static ssize_t my_read(struct file * file, char __user *u_buf, size_t len, loff_t *offset)
{
	int bytes_to_copy;
	if(*offset >=buffer_size)
		return 0;
	
	bytes_to_copy=min(len,(size_t)(buffer_size - *offset));
	if(copy_to_user(u_buf,k_buf +*offset, bytes_to_copy))
	{
		return -EFAULT;
	}
	*offset+=bytes_to_copy;
	pr_info("basic_char:read %d bytes \n",bytes_to_copy);
	return bytes_to_copy;
}
static ssize_t my_write(struct file *file,const char __user *u_buf,size_t len, loff_t *offset)
{
	int bytes_to_copy;
	bytes_to_copy=min(len,(size_t)BUF_SIZE);
	if(copy_from_user(k_buf,u_buf,bytes_to_copy))
	{
		return -EFAULT;
	}
	buffer_size=bytes_to_copy;
	return bytes_to_copy;
}

struct file_operations fop={
	.owner=THIS_MODULE,
	.open=my_open,
	.write=my_write,
	.read=my_read,
	.release=my_close,
};
static int __init my_init(void){
	pr_info("module is inserted succesfull...\n");
	int ret;
	ret=alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
	if(ret){
		pr_info("unable to allocate device...\n");
		return ret;
	}
	pr_info( " char device is aloocated successfully...\n");
	cdev_init(&basic_cdev,&fop);
	 ret=cdev_add(&basic_cdev,dev_num,1);
	if(ret){
		goto unregister_region;
		}
	basic_class=class_create(THIS_MODULE,DEVICE_NAME);
	if(IS_ERR(basic_class)){
		ret=PTR_ERR(basic_class);
		goto  del_cdev;
	}
	if(IS_ERR(device_create(basic_class,NULL,dev_num,NULL,DEVICE_NAME)))
	{
		ret=-EINVAL;
		goto destroy_class;
	}
	pr_info("driver loaded...\n");
	pr_info("major =%d Minor =%d\n",MAJOR(dev_num), MINOR(dev_num));

	return 0;
	destroy_class:
		class_destroy(basic_class);
	del_cdev:
		cdev_del(&basic_cdev);
	unregister_region:
		unregister_chrdev_region(dev_num,1);

	return ret;
}

static void __exit my_exit(void){
	pr_info("module is removed successfully...\n");
	device_destroy(basic_class,dev_num);
	class_destroy(basic_class);
	cdev_del(&basic_cdev);
	unregister_chrdev_region(dev_num,1);
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Teja");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("basic char_device_driver module....\n");
