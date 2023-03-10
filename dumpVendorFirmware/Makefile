

all:  string2bin

string2bin: tfw.o
	$(CC) -o string2bin string2bin.c tfw.o

tfw.o:
	$(CC) -c tfw.c


clean:
	rm -f *.o string2bin tags cscope.*
	rm -fr *.mod.c *.mod *.o .*.cmd *.ko .tmp_versions Module.symvers modules.order


