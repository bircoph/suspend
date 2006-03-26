#CONFIG_COMPRESS=yes
#CONFIG_ENCRYPT=yes

CC_FLAGS=-I/usr/local/include
LD_FLAGS=-L/usr/local/lib

ifdef CONFIG_COMPRESS
CC_FLAGS	+= -DCONFIG_COMPRESS
LD_FLAGS	+= -llzf
endif
ifdef CONFIG_ENCRYPT
CC_FLAGS	+= -DCONFIG_ENCRYPT
LD_FLAGS	+= -lcrypto
endif

SUSPEND_DIR=/usr/local/sbin
CONFIG_DIR=/etc
RESUME_DEVICE=<path_to_resume_device_file>
BOOT_DIR=/boot

ifdef CONFIG_ENCRYPT
all: suspend suspend-keygen resume s2ram
else
all: suspend resume s2ram
endif

S2RAMOBJ=vt.o vbetool/lrmi.o vbetool/x86-common.o vbetool/vbetool.o radeontool.o dmidecode.o

ifeq ($(ARCH), x86_64)
S2RAMOBJ=vt.o vbetool/thunk.o vbetool/x86-common.o vbetool/vbetool.o vbetool/x86emu/libx86emu.a radeontool.o dmidecode.o
endif

clean:
	rm -f suspend suspend-keygen suspend.keys resume s2ram *.o vbetool/*.o vbetool/x86emu/*.o vbetool/x86emu/*.a

s2ram:	s2ram.c dmidecode.c whitelist.c radeontool.c $(S2RAMOBJ)
	$(CC) -g -Wall -O2 s2ram.c $(S2RAMOBJ) -lpci -o s2ram

vbetool/vbetool.o:	vbetool/vbetool.c
	$(CC) -Wall -O2 -DS2RAM -c vbetool/vbetool.c -o vbetool/vbetool.o

vbetool/lrmi.o:	vbetool/lrmi.c
	$(CC) -Wall -O2 -c vbetool/lrmi.c -o vbetool/lrmi.o

vbetool/x86-common.o:	vbetool/x86-common.c
	$(CC) -Wall -O2 -c vbetool/x86-common.c -o vbetool/x86-common.o

vbetool/x86emu/libx86emu.a:
	make -C vbetool/x86emu -f makefile.linux

vbetool/thunk.o:	vbetool/thunk.c
	$(CC) -Wall -O2 -c vbetool/thunk.c -o vbetool/thunk.o

dmidecode.o:	dmidecode.c
	$(CC) -Wall -O2 -DS2RAM -c dmidecode.c -o dmidecode.o

radeontool.o:	radeontool.c
	$(CC) -Wall -O2 -DS2RAM -c radeontool.c -o radeontool.o

md5.o:	md5.c md5.h
	$(CC) -Wall -o md5.o -DHAVE_INTTYPES_H -DHAVE_STDINT_H -c md5.c

encrypt.o:	encrypt.c encrypt.h md5.h
	$(CC) -Wall -DHAVE_INTTYPES_H -DHAVE_STDINT_H $(CC_FLAGS) -c encrypt.c

config.o:	config.c config.h
	$(CC) -Wall $(CC_FLAGS) -c config.c

vt.o:	vt.c vt.h
	$(CC) -Wall -c vt.c

suspend:	md5.o encrypt.o config.o suspend.c swsusp.h config.h encrypt.h md5.h s2ram.c dmidecode.c whitelist.c radeontool.c $(S2RAMOBJ)
	$(CC) -g -O2 -DCONFIG_BOTH -Wall $(CC_FLAGS) md5.o encrypt.o config.o suspend.c s2ram.c -o suspend $(S2RAMOBJ) $(LD_FLAGS) -lpci

resume:	md5.o encrypt.o config.o resume.c swsusp.h config.h encrypt.h md5.h
	$(CC) -Wall $(CC_FLAGS) md5.o encrypt.o config.o resume.c -static -o resume $(LD_FLAGS)

ifdef CONFIG_ENCRYPT
suspend-keygen:	md5.o encrypt.o keygen.c encrypt.h md5.h
	$(CC) -Wall -DHAVE_INTTYPES_H -DHAVE_STDINT_H -DCONFIG_ENCRYPT md5.o keygen.c -o suspend-keygen -lcrypto

install-suspend:	suspend suspend-keygen conf/suspend.conf
	if [ ! -c /dev/snapshot ]; then mknod /dev/snapshot c 10 231; fi
	install --mode=755 suspend-keygen $(SUSPEND_DIR)
	install --mode=755 suspend $(SUSPEND_DIR)
	install --mode=644 conf/suspend.conf $(CONFIG_DIR)
else
install-suspend:	suspend conf/suspend.conf
	if [ ! -c /dev/snapshot ]; then mknod /dev/snapshot c 10 231; fi
	install --mode=755 suspend $(SUSPEND_DIR)
	install --mode=644 conf/suspend.conf $(CONFIG_DIR)
endif

install-resume-initrd:	resume conf/suspend.conf
	BOOT_DIR=$(BOOT_DIR) ./scripts/create-resume-initrd.sh $(RESUME_DEVICE)

install-resume:		resume 
	./scripts/install-resume.sh

