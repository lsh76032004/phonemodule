obj-m = pmodule.o

KDIR = /root/linux

PWD  = $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean


install :
	 scp  $(patsubst %.o,%.ko, $(obj-m))  root@192.168.137.100:
	 scp myapp.c root@192.168.137.100:
