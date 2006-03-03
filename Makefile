CC_FLAGS=-I/usr/local/include
# add "-DCONFIG_COMPRESS" for compression
# add "-DCONFIG_ENCRYPT" for encryption
LD_FLAGS=-L/usr/local/lib
# add "-llzf" for compression
# add "-lcrypto" for encryption

SUSPEND_DIR=/usr/local/sbin
CONFIG_DIR=/etc
RESUME_DEVICE=<path_to_resume_device_file>
BOOT_DIR=/boot

all: suspend resume s2ram

S2RAMOBJ=vt.o vbetool/lrmi.o vbetool/x86-common.o

ifeq ($(ARCH), x86_64)
S2RAMOBJ=vt.o vbetool/thunk.o vbetool/x86-common.o vbetool/x86emu/libx86emu.a
endif

clean:
	rm -f suspend resume s2ram *.o vbetool/*.o vbetool/x86emu/*.o vbetool/x86emu/*.a

s2ram:	s2ram.c dmidecode.c whitelist.c $(S2RAMOBJ)
	gcc -g -Wall -O2 s2ram.c $(S2RAMOBJ) -lpci -o s2ram

vbetool/lrmi.o:	vbetool/lrmi.c
	gcc -Wall -O2 -c vbetool/lrmi.c -o vbetool/lrmi.o

vbetool/x86-common.o:	vbetool/x86-common.c
	gcc -Wall -O2 -c vbetool/x86-common.c -o vbetool/x86-common.o

vbetool/x86emu/libx86emu.a:
	make -C vbetool/x86emu -f makefile.linux

vbetool/thunk.o:	vbetool/thunk.c
	gcc -Wall -O2 -c vbetool/thunk.c -o vbetool/thunk.o

md5.o:	md5.c md5.h
	gcc -Wall -o md5.o -DHAVE_INTTYPES_H -DHAVE_STDINT_H -c md5.c

encrypt.o:	encrypt.c encrypt.h md5.h
	gcc -Wall -DHAVE_INTTYPES_H -DHAVE_STDINT_H $(CC_FLAGS) -c encrypt.c

config.o:	config.c config.h
	gcc -Wall $(CC_FLAGS) -c config.c

vt.o:	vt.c vt.h
	gcc -Wall -c vt.c

suspend:	md5.o encrypt.o config.o suspend.c swsusp.h config.h encrypt.h md5.h
	gcc -Wall $(CC_FLAGS) md5.o encrypt.o config.o suspend.c -o suspend $(LD_FLAGS)

resume:	md5.o encrypt.o config.o resume.c swsusp.h config.h encrypt.h md5.h
	gcc -Wall $(CC_FLAGS) md5.o encrypt.o config.o resume.c -static -o resume $(LD_FLAGS)

install-suspend:	suspend conf/suspend.conf
	if [ ! -c /dev/snapshot ]; then mknod /dev/snapshot c 10 231; fi
	install --mode=755 suspend $(SUSPEND_DIR)
	install --mode=644 conf/suspend.conf $(CONFIG_DIR)

install-resume-initrd:	resume conf/suspend.conf
	BOOT_DIR=$(BOOT_DIR) ./scripts/create-resume-initrd.sh $(RESUME_DEVICE)

install-resume:		resume 
	./scripts/install-resume.sh

