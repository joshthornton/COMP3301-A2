OBJ=crypto.o

obj-m += $(OBJ)
MOD_DIR=/lib/modules/$(shell uname -r)/build

crypto-objs := a2-fops.o buffer.o fd.o


.PHONY: all
all:
	make -C $(MOD_DIR) M=$(PWD) KBUILD_EXTRA_SYMBOLS=/home/comp3301/a2/cryptodev-1.0/Module.symvers modules

.PHOTO: clean
clean:
	#make -C $(MOD_DIR) M=$(PWD) clean
	rm -f *.o
	rm -f *.ko
