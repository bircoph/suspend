#CONFIG_COMPRESS=yes
#CONFIG_ENCRYPT=yes

ARCH:=$(shell uname -m)

CC_FLAGS=-I/usr/local/include
LD_FLAGS=-L/usr/local/lib

CFLAGS := -O2 -Wall

ifdef CONFIG_COMPRESS
CC_FLAGS	+= -DCONFIG_COMPRESS
LD_FLAGS	+= -llzf
endif
ifdef CONFIG_ENCRYPT
GCRYPT_CC_FLAGS := $(shell libgcrypt-config --cflags)
GCRYPT_LD_FLAGS := $(shell libgcrypt-config --libs)
CC_FLAGS	+= -DCONFIG_ENCRYPT
CC_FLAGS	+= $(GCRYPT_CC_FLAGS)
LD_FLAGS	+= $(GCRYPT_LD_FLAGS)
endif

SUSPEND_DIR=/usr/local/sbin
CONFIG_DIR=/etc
RESUME_DEVICE=<path_to_resume_device_file>
BOOT_DIR=/boot

S2BOTH=s2both
S2DISK=s2disk
CONFIGFILE=suspend.conf

ifdef CONFIG_ENCRYPT
all: $(S2DISK) $(S2BOTH) suspend-keygen resume s2ram
else
all: $(S2DISK) $(S2BOTH) resume s2ram
endif

S2RAMOBJ=vt.o vbetool/lrmi.o vbetool/x86-common.o vbetool/vbetool.o radeontool.o dmidecode.o

ifeq ($(ARCH), x86_64)
S2RAMOBJ=vt.o vbetool/thunk.o vbetool/x86-common.o vbetool/vbetool.o vbetool/x86emu/libx86emu.a radeontool.o dmidecode.o
endif

SPLASHOBJ = splash.o bootsplash.o

clean:
	rm -f $(S2DISK) suspend-keygen suspend.keys resume s2ram *.o vbetool/*.o vbetool/x86emu/*.o vbetool/x86emu/*.a

s2ram:	s2ram.c dmidecode.c whitelist.c radeontool.c $(S2RAMOBJ)
	$(CC) $(CFLAGS) -g s2ram.c $(S2RAMOBJ) -lpci -o s2ram

vbetool/vbetool.o:	vbetool/vbetool.c
	$(CC) $(CFLAGS) -DS2RAM -c vbetool/vbetool.c -o vbetool/vbetool.o

vbetool/lrmi.o:	vbetool/lrmi.c
	$(CC) $(CFLAGS) -c vbetool/lrmi.c -o vbetool/lrmi.o

vbetool/x86-common.o:	vbetool/x86-common.c
	$(CC) $(CFLAGS) -c vbetool/x86-common.c -o vbetool/x86-common.o

vbetool/x86emu/libx86emu.a:
	make -C vbetool/x86emu -f makefile.linux

vbetool/thunk.o:	vbetool/thunk.c
	$(CC) $(CFLAGS) -c vbetool/thunk.c -o vbetool/thunk.o

dmidecode.o:	dmidecode.c
	$(CC) $(CFLAGS) -DS2RAM -c dmidecode.c -o dmidecode.o

radeontool.o:	radeontool.c
	$(CC) $(CFLAGS) -DS2RAM -c radeontool.c -o radeontool.o

md5.o:	md5.c md5.h
	$(CC) $(CFLAGS) -DHAVE_INTTYPES_H -DHAVE_STDINT_H -c md5.c -o md5.o

encrypt.o:	encrypt.c encrypt.h md5.h
	$(CC) $(CFLAGS) -DHAVE_INTTYPES_H -DHAVE_STDINT_H $(CC_FLAGS) -c encrypt.c -o encrypt.o

config.o:	config.c config.h
	$(CC) $(CFLAGS) $(CC_FLAGS) -c config.c -o config.o

vt.o:	vt.c vt.h
	$(CC) $(CFLAGS) -c vt.c -o vt.o

bootsplash.o: bootsplash.h bootsplash.c
	$(CC) -g $(CFLAGS) $(CC_FLAGS) -c bootsplash.c -o bootsplash.o

splash.o: splash.h splash.c bootsplash.o vt.o
	$(CC) -g $(CFLAGS) $(CC_FLAGS) -c splash.c -o splash.o

$(S2DISK):	vt.o md5.o encrypt.o config.o suspend.c swsusp.h config.h encrypt.h md5.h $(SPLASHOBJ)
	$(CC) -g $(CFLAGS) $(CC_FLAGS) vt.o md5.o encrypt.o config.o suspend.c -o $@ $(SPLASHOBJ) $(LD_FLAGS)

$(S2BOTH):	md5.o encrypt.o config.o suspend.c swsusp.h config.h encrypt.h md5.h s2ram.c dmidecode.c whitelist.c radeontool.c $(S2RAMOBJ) $(SPLASHOBJ)
	$(CC) -g $(CFLAGS) -DCONFIG_BOTH $(CC_FLAGS) md5.o encrypt.o config.o suspend.c s2ram.c -o $@ $(S2RAMOBJ) $(SPLASHOBJ) $(LD_FLAGS) -lpci

resume:	md5.o encrypt.o config.o resume.c swsusp.h config.h encrypt.h md5.h $(SPLASHOBJ)
	$(CC) $(CFLAGS) $(CC_FLAGS) md5.o encrypt.o config.o vt.o resume.c $(SPLASHOBJ) -static -o resume $(LD_FLAGS)

ifdef CONFIG_ENCRYPT
suspend-keygen:	md5.o keygen.c encrypt.h md5.h
	$(CC) $(CFLAGS) -DHAVE_INTTYPES_H -DHAVE_STDINT_H $(CC_FLAGS) md5.o keygen.c -o suspend-keygen $(LD_FLAGS)

install-s2disk: $(S2DISK) suspend-keygen conf/$(CONFIGFILE)
	if [ ! -c /dev/snapshot ]; then mknod /dev/snapshot c 10 231; fi
	install --mode=755 suspend-keygen $(DESTDIR)$(SUSPEND_DIR)
	install --mode=755 $(S2DISK) $(DESTDIR)$(SUSPEND_DIR)
	if [ -f $(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE) ]; then install --mode=644 conf/$(CONFIGFILE) $(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE).new; else install --mode=644 conf/$(CONFIGFILE) $(DESTDIR)$(CONFIG_DIR); fi

install-suspend: $(S2DISK) $(S2BOTH) suspend-keygen conf/$(CONFIGFILE)
	if [ ! -c /dev/snapshot ]; then mknod /dev/snapshot c 10 231; fi
	install --mode=755 suspend-keygen $(DESTDIR)$(SUSPEND_DIR)
	install --mode=755 $(S2DISK) $(DESTDIR)$(SUSPEND_DIR)
	install --mode=755 $(S2BOTH) $(DESTDIR)$(SUSPEND_DIR)
	if [ -f $(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE) ]; then install --mode=644 conf/$(CONFIGFILE) $(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE).new; else install --mode=644 conf/$(CONFIGFILE) $(DESTDIR)$(CONFIG_DIR); fi
	install --mode=755 s2ram $(DESTDIR)$(SUSPEND_DIR)
else
install-s2disk: $(S2DISK) conf/$(CONFIGFILE)
	if [ ! -c /dev/snapshot ]; then mknod /dev/snapshot c 10 231; fi
	install --mode=755 $(S2DISK) $(DESTDIR)$(SUSPEND_DIR)
	if [ -f $(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE) ]; then install --mode=644 conf/$(CONFIGFILE) $(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE).new; else install --mode=644 conf/$(CONFIGFILE) $(DESTDIR)$(CONFIG_DIR); fi

install-suspend: $(S2DISK) $(S2BOTH) conf/$(CONFIGFILE)
	if [ ! -c /dev/snapshot ]; then mknod /dev/snapshot c 10 231; fi
	install --mode=755 $(S2DISK) $(DESTDIR)$(SUSPEND_DIR)
	install --mode=755 $(S2BOTH) $(DESTDIR)$(SUSPEND_DIR)
	if [ -f $(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE) ]; then install --mode=644 conf/$(CONFIGFILE) $(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE).new; else install --mode=644 conf/$(CONFIGFILE) $(DESTDIR)$(CONFIG_DIR); fi
	install --mode=755 s2ram $(DESTDIR)$(SUSPEND_DIR)
endif

install-resume-initrd:	resume conf/$(CONFIGFILE)
	BOOT_DIR=$(DESTDIR)$(BOOT_DIR) ./scripts/create-resume-initrd.sh $(RESUME_DEVICE)

install-resume:		resume 
	./scripts/install-resume.sh

