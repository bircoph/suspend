#CONFIG_COMPRESS=yes
#CONFIG_ENCRYPT=yes
#CONFIG_SPLASHY=yes
#CONFIG_UDEV=yes
#CONFIG_RESUME_DYN=yes
#CONFIG_EMBEDDED_LIBX86=yes

SUSPEND_DIR=/usr/local/sbin
RESUME_DIR=/usr/local/lib/suspend
CONFIG_DIR=/etc
RESUME_DEVICE=<path_to_resume_device_file>
BOOT_DIR=/boot
CONFIGFILE=suspend.conf
CFLAGS := -O2 -Wall

###############################################################

ARCH:=$(shell uname -m)

CC_FLAGS=-I/usr/local/include -DS2RAM $(CFLAGS)
LD_FLAGS=-L/usr/local/lib

ifdef CONFIG_EMBEDDED_LIBX86
CC_FLAGS += -Ilibx86/
endif

BINARIES=s2disk s2both s2ram swap-offset resume
BINARIES_MIN=s2disk swap-offset

S2RAM_OBJ=vt.o vbetool/x86-common.o vbetool/vbetool.o radeontool.o dmidecode.o
SWSUSP_OBJ=vt.o md5.o encrypt.o config.o loglevel.o splash.o bootsplash.o 

S2RAM_LD_FLAGS = $(LD_FLAGS) -lpci -lz
SWSUSP_LD_FLAGS = $(LD_FLAGS)

ifndef CONFIG_RESUME_DYN
STATIC_LD_FLAGS = -static
endif

ifdef CONFIG_COMPRESS
CC_FLAGS	+= -DCONFIG_COMPRESS
SWSUSP_LD_FLAGS	+= -llzf
endif

ifdef CONFIG_ENCRYPT
BINARIES	+= suspend-keygen 
BINARIES_MIN	+= suspend-keygen 
CC_FLAGS	+= -DCONFIG_ENCRYPT
GCRYPT_CC_FLAGS	= $(shell libgcrypt-config --cflags)
SWSUSP_CC_FLAGS	+= $(GCRYPT_CC_FLAGS)
GCRYPT_LD_FLAGS = $(shell libgcrypt-config --libs)
SWSUSP_LD_FLAGS += $(GCRYPT_LD_FLAGS)
INSTALL_KEYGEN	= install-keygen
endif

ifndef CONFIG_UDEV
SNAPSHOT=$(DESTDIR)/dev/snapshot
endif

ifdef CONFIG_SPLASHY
SWSUSP_OBJ	+= splashy_funcs.o 
CC_FLAGS	+= -DCONFIG_SPLASHY
SWSUSP_LD_FLAGS	+= -lsplashy
ifndef CONFIG_RESUME_DYN
STATIC_LD_FLAGS += -lsplashycnf \
		$(shell directfb-config  --libs --input=keyboard \
		--imageprovider=jpeg,gif,png\
		--font=ft2,default) \
		$(shell pkg-config --static --libs glib-2.0)
STATIC_CC_FLAGS=$(shell directfb-config --cflags)\
		$(shell pkg-config --static --cflags glib-2.0)
endif
endif

###
ifdef CONFIG_EMBEDDED_LIBX86
ifeq ($(ARCH), x86_64)
LIBX86_MAKE = BACKEND=x86emu
endif

S2RAM_OBJ += libx86/libx86.a 
else
S2RAM_LD_FLAGS += -lx86
endif

all: $(BINARIES)

clean:
	make -C libx86 clean || true
	rm -f $(BINARIES) suspend-keygen suspend.keys *.o vbetool/*.o


#### Rules for objects
s2ram-both.o: s2ram.c s2ram.h whitelist.c
	$(CC) $(CC_FLAGS) -DCONFIG_BOTH -c $< -o $@

s2ram.o: s2ram.c s2ram.h whitelist.c
	$(CC) $(CC_FLAGS) -c $< -o $@

md5.o encrypt.o: %.o : %.c %.h md5.h
	$(CC) $(CC_FLAGS) -DHAVE_INTTYPES_H -DHAVE_STDINT_H -c $< -o $@

# Simple objects with header
config.o vt.o bootsplash.o splash.o splashy_funcs.o vbetool/x86-common.o vbetool/vbetool.o: %.o : %.c %.h
	$(CC) $(CC_FLAGS) -c $< -o $@

# Simple object without header
dmidecode.o radeontool.o : %.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@

libx86/libx86.a:
	ln -sf lrmi.h libx86/libx86.h
	make -C libx86 $(LIBX86_MAKE) static

libx86/lrmi.o libx86/thunk.o: %.o: %.c
	$(CC) $(CC_FLAGS) -c $< -o $@


#### Rules for binaries
s2disk:	$(SWSUSP_OBJ) suspend.c
	$(CC) -g $(CC_FLAGS)  $^ -o $@ $(SWSUSP_LD_FLAGS)

s2ram:	$(S2RAM_OBJ) s2ram.o
	$(CC) -g $(CC_FLAGS)  $^ -o $@ $(S2RAM_LD_FLAGS)

s2both:	$(SWSUSP_OBJ) $(S2RAM_OBJ) s2ram-both.o suspend.c 
	$(CC) -g $(CC_FLAGS) -DCONFIG_BOTH  $^ -o $@ $(SWSUSP_LD_FLAGS) $(S2RAM_LD_FLAGS)

resume:	resume.c $(SWSUSP_OBJ)
	$(CC) $(CC_FLAGS) $(STATIC_CC_FLAGS) $^ -o $@ $(STATIC_LD_FLAGS) $(SWSUSP_LD_FLAGS)

swap-offset: swap-offset.c
	$(CC) $(CC_FLAGS) $< -o $@ $(LD_FLAGS)

suspend-keygen:	 keygen.c md5.o encrypt.h 
	$(CC) $(CC_FLAGS) $(GCRYPT_CC_FLAGS) -DHAVE_INTTYPES_H -DHAVE_STDINT_H md5.o $< -o $@ $(LD_FLAGS) $(GCRYPT_LD_FLAGS)




#### Install targets
$(SNAPSHOT):
	mknod $(SNAPSHOT) c 10 231;

install-conf: conf/$(CONFIGFILE)
	if [ -f $(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE) ]; then \
		install --mode=644 conf/$(CONFIGFILE) \
			$(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE).new;\
	else \
		install -D --mode=644 conf/$(CONFIGFILE) \
			$(DESTDIR)$(CONFIG_DIR)/$(CONFIGFILE); \
	fi

install-resume:
	install -D --mode=755 resume $(DESTDIR)$(RESUME_DIR)/resume

install-% : %
	install -D --mode=755 $< $(DESTDIR)$(SUSPEND_DIR)/$<

# Some convenience targets
install-resume-new-initrd:	resume conf/$(CONFIGFILE)
	BOOT_DIR=$(DESTDIR)$(BOOT_DIR) ./scripts/create-resume-initrd.sh $(RESUME_DEVICE)

install-resume-on-initrd:	resume 
	./scripts/install-resume.sh

install-minimal: $(patsubst %,install-%,$(BINARIES_MIN)) $(SNAPSHOT) install-conf

install: $(patsubst %,install-%,$(BINARIES)) $(SNAPSHOT) install-conf


