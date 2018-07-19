#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/io.h>

#include <asm/io.h>
#include <asm/uaccess.h>

static volatile unsigned int* gpio;

// TODO: GPIO 주소에 관한 초기화 매크로 함수를 작성 하세요.
#define BCM_IO_BASE 		0x3F000000                       
#define GPIO_BASE           (BCM_IO_BASE + 0x200000)     
#define GPIO_SIZE           (256) 


// TODO: 매크로 함수 작성하세요.
#define GPIO_IN(g)  	(*(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))) 
#define GPIO_OUT(g) 	(*(gpio+((g)/10)) |= (1<<(((g)%10)*3)))
#define GPIO_SET(g) 	(*(gpio+7) = 1<<g)
#define GPIO_CLR(g)		(*(gpio+10) = 1<<g)
#define GPIO_GET(g)		(*(gpio+13)&(1<<g))
//struct cdev gpio_cdev;


#define SEG_DIG 22
#define LED_RATIO _IOW('c', 1, int)





// TODO: 회로도를 보고 기본 LED의 적절한 GPIO 값을 초기화하세요. 
static int led_gpios[] = {16, 19, 20, 21 };

// TODO: 회로도를 보고 7Segment 의 적절한 GPIO 값을 초기화하세요. 
static int fnd_gpios[] = {7,8,9,10,11,14,15,0 };
static int num[10][8] = {
	      {	0, 0, 0, 0, 0, 0, 1, 1 },
 	      { 1, 0, 0, 1, 1, 1, 1, 1 },
   	      { 0, 0, 1, 0, 0, 1, 0, 1 },
          { 0, 0, 0, 0, 1, 1, 0, 1 },
          { 1, 0, 0, 1, 1, 0, 0, 1 },
          { 0, 1, 0, 0, 1, 0, 0, 1 },
          { 0, 1, 0, 0, 0, 0, 0, 1 },
          { 0, 0, 0, 1, 1, 1, 1, 1 },
          { 0, 0, 0, 0, 0, 0, 0, 1 },
          { 0, 0, 0, 0, 1, 0, 0, 1 }};
 
static void update_led(int cds)
{
    // TODO: 적절한 로직을 작성하세요.
    //       밝기에 따라 기본 LED를 출력 개수를 조절하세요.
    if(cds<=255)
    {
    	GPIO_SET(led_gpios[0]);
    	GPIO_CLR(led_gpios[1]);
    	GPIO_CLR(led_gpios[2]);
    	GPIO_CLR(led_gpios[3]);
	}else if(cds<=511)
	{
		GPIO_SET(led_gpios[0]);
    	GPIO_SET(led_gpios[1]);
    	GPIO_CLR(led_gpios[2]);
    	GPIO_CLR(led_gpios[3]);
	}else if(cds<=767){
		GPIO_SET(led_gpios[0]);
    	GPIO_SET(led_gpios[1]);
    	GPIO_SET(led_gpios[2]);
    	GPIO_CLR(led_gpios[3]);
	
	}else if(cds<=1023){
		GPIO_SET(led_gpios[0]);
    	GPIO_SET(led_gpios[1]);
    	GPIO_SET(led_gpios[2]);
    	GPIO_SET(led_gpios[3]);
	}
}


static void update_fnd(int cds)
{
    // TODO: 적절한 로직을 작성하세요.
    //       밝기에 따라 7 segment 값을 조절하세요.
    int i;
 	int pin = 0;
 	int n;
 	n = cds;
 	GPIO_SET(SEG_DIG);
 	//gpio_set_value(SEG_DIG,1);
 	
 	//for(i=0,pin=0;i<8;i++,pin++)
 	//	gpio_set_value(fnd_gpios[pin],!num[n][i]);
 	
 	for(i=0,pin=0;i<8;i++,pin++){
		if(!num[n][i])
			GPIO_SET(fnd_gpios[pin]);
		else
			GPIO_CLR(fnd_gpios[pin]);
	}
}

// TODO: ioctl 프로토콜을 작성하세요.
static long my_ioctl(struct file* filp, unsigned int arg, unsigned long opt)
{
 
    // TODO: ioctl 로직을 작성하세요.a
	int ret, num, size, per;
	printk("my_ioclt()\n");
	if( _IOC_TYPE(arg) != 'c')
		return -EINVAL;
	size = _IOC_SIZE(arg);
	switch(arg)
	{
	case LED_RATIO:
		ret = copy_from_user(&num, (void*)opt, size);
		printk("cds = %d\n",num);
		per = num*100/2047;
		update_fnd(per/10);
		update_led(num);
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
	

	printk("my_init\n");


	// TODO: ioremap을 이용하여 gpio 주소를 가상주소로 매핑 하세요.
	map = ioremap(GPIO_BASE, GPIO_SIZE);
	gpio = (volatile unsigned int*)map;
	misc_register(&misc);

	// TODO: GPIO LED 초기화 로직을 작성하세요.
	for(i=0;i<4;i++)
	{
		GPIO_IN(led_gpios[i]);
		GPIO_OUT(led_gpios[i]);
	}

	//for(i = 0; i<4;i++)
	//	gpio_direction_output(led_gpios[i],0);

	// TODO: 7Segment 초기화 로직을 작성하세요.
	//gpio_direction_output(SEG_DIG,0);
	GPIO_OUT(SEG_DIG);
	for(i=0;i<8;i++)
		GPIO_OUT(fnd_gpios[i]);

	return 0;
}

static void my_exit(void)
{
	/*
	if(gpio){
		iounmap(gpio);
	}
	*/

	misc_deregister(&misc);
	printk("my_exit\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
