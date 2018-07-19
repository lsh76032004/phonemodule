#ARCH='arm'
#HOST?=$(ARCH)
#CROSS-COMPILE=$(ARCH)-elf-
CC=$(CROSS-COMPILE)gcc
PWD=$(shell pwd)

CFLAGS=-O2 -fPIC --shared
LDFLAGS=
LIBS=-ldl -lwiringPi

TARGET=phone_module.so
OBJ=src/phone_module.c src/parson.c src/com_arduino.c

%o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

install: 
	sudo mkdir -p /etc/3team/dl/
	sudo cp dl.rc /etc/3team/
	sudo cp $(TARGET) /etc/3team/dl/

.PHONY: clean

clean:
	rm -f src/*.o $(TARGET)
