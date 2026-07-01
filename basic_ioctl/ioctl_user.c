#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>

#define MY_IOCTL_MAGIC 'T'
#define SET_VALUE _IOW(MY_IOCTL_MAGIC, 1, int)
#define GET_VALUE _IOR(MY_IOCTL_MAGIC,2, int)

int main(){
	int fd;
	int value=12;
	fd=open("/dev/ioctl_dev",O_RDWR);
	if(fd<0){
		perror("open");
		return 0;
	}
	ioctl(fd,SET_VALUE, &value);
	value=0;

	ioctl(fd,GET_VALUE,&value);
	printf("value=%d\n",value);
	close(fd);
	return 0;
}


