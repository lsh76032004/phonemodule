#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>

#include <asm/io.h>
#include <asm/uaccess.h>

static volatile unsigned int* gpio;

#define BCM_IO_BASE 		0x3F000000                       
#define GPIO_BASE           (BCM_IO_BASE + 0x200000)     
#define GPIO_SIZE           (256) 

#define GPIO_IN(g)  	(*(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))) 
#define GPIO_OUT(g) 	(*(gpio+((g)/10)) |= (1<<(((g)%10)*3)))
#define GPIO_SET(g) 	(*(gpio+7) = 1<<g)
#define GPIO_DSET(g) 	(*(gpio+7) = 1<<g) 
#define GPIO_CLR(g)		(*(gpio+10) = 1<<g)
#define GPIO_GET(g)		(*(gpio+13)&(1<<g))


#define SEG_DIG 22
#define STU_NUM _IOW('c', 1, int)

static int fnd_gpios[] = {7,8,9,10,11,14,15,0 };
static int num[10][8] = {
	{ 0, 0, 0, 0, 0, 0, 1, 1 },
	{ 1, 0, 0, 1, 1, 1, 1, 1 },
	{ 0, 0, 1, 0, 0, 1, 0, 1 },
	{ 0, 0, 0, 0, 1, 1, 0, 1 },
	{ 1, 0, 0, 1, 1, 0, 0, 1 },
	{ 0, 1, 0, 0, 1, 0, 0, 1 },
	{ 0, 1, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 1, 1, 1, 1, 1 },
	{ 0, 0, 0, 0, 0, 0, 0, 1 },
	{ 0, 0, 0, 0, 1, 0, 0, 1 }};

static void update_fnd(int numOfStu)
{
	int j;
	int pin = 0;
	int n = numOfStu;
	GPIO_SET(SEG_DIG);
	for( j=0, pin=0; j<8; j++, pin++ ){
		if(!num[n][j])
			GPIO_SET(fnd_gpios[pin]);
		else
			GPIO_CLR(fnd_gpios[pin]);
	}

}

static long my_ioctl(struct file* filp, unsigned int arg, unsigned long opt)
{
	int ret, size, numOfStu;
	//printk("my_ioclt()\n");
	if( _IOC_TYPE(arg) != 'c')
		return -EINVAL;
	size = _IOC_SIZE(arg);
	switch(arg)
	{
		case STU_NUM:
			ret = copy_from_user(&numOfStu, (void*)opt, size);
	//		printk("NumOfStu : %d\n",numOfStu);
			update_fnd(numOfStu);
			break;
	}
	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = my_ioctl,
	.compat_ioctl = my_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "mydev",
	.fops = &fops,
};

static int my_init(void)
{	
	static void *map;
	int i;

	printk("my_init()\n");

	map = ioremap(GPIO_BASE, GPIO_SIZE);
	gpio = (volatile unsigned int*)map;
	misc_register(&misc);


	GPIO_OUT(SEG_DIG);
	for(i=0;i<8;i++)
		GPIO_OUT(fnd_gpios[i]);


	return 0;
}

static void my_exit(void)
{
	printk("my_exit()\n");

	if(gpio){
		iounmap(gpio);
	}

	misc_deregister(&misc);
	printk("my_exit\n");
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("GPL");
