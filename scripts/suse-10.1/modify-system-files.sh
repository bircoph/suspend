#!/bin/bash

if [ ! -f do_acpi_sleep.patch -a ! -f mkinitrd.patch ]; then
	echo "Required patches not found!"
	exit 1
fi

if [ ! -x /usr/local/sbin/resume ]; then
	echo "The resume binary not found!"
	exit 2;
fi

if [ ! -f /etc/suspend.conf ]; then
	echo "The suspend configuration file not found!"
	exit 3;
fi

CURRENT_DIR=$(pwd)

cd /sbin
cp mkinitrd mkinitrd.orig
cat $CURRENT_DIR/mkinitrd.patch | patch -p1

cd /usr/lib/powersave
cp do_acpi_sleep do_acpi_sleep.orig
cat $CURRENT_DIR/do_acpi_sleep.patch | patch -p1

cd $CURRENT_DIR/

