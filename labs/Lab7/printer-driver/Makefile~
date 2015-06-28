obj-m += hello_file.o

LINUX_SRC = ../linux

all:
	make -C $(LINUX_SRC) M=$(PWD) modules

clean:
	make -C $(LINUX_SRC) M=$(PWD) clean

