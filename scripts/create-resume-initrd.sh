#!/bin/bash

RESUME_DEVICE=$1
if [ -n "$2" ]; then
	TMP_FILE=$2;
else
	TMP_FILE="initrd"
fi
if [ -z "$RESUME" ]; then
	RESUME="resume"
fi
if [ -z "$INITRD_SIZE" ]; then
	INITRD_SIZE=2000k
fi
if [ -z "$CONFIG_FILE" ]; then
	CONFIG_FILE="conf/suspend.conf"
fi 
if [ -z "$MOUNT_POINT" ]; then
	MOUNT_POINT="mnt"
	if [ ! -d $MOUNT_POINT ]; then
		mkdir $MOUNT_POINT
		if [ ! -d $MOUNT_POINT ]; then
			echo "Could not create the mount point"
			exit 1
		fi
		RMDIR_MOUNT_POINT="yes"
	else
		RMDIR_MOUNT_POINT=""
	fi
fi
if [ -z "$BOOT_DIR" ]; then
	BOOT_DIR="/boot"
fi
if [ -z "$INITRD_FILE" ]; then
	INITRD_FILE="$BOOT_DIR/resume-initrd"
fi

if [ -f $RESUME -a -d $MOUNT_POINT -a -b $RESUME_DEVICE ]; then
	dd if=/dev/zero of=$TMP_FILE bs=$INITRD_SIZE count=1
	/sbin/mke2fs -F -m0 $TMP_FILE
	mount -t ext2 -o loop $TMP_FILE $MOUNT_POINT
	if [ "$?" = "0" ]; then 
		mkdir $MOUNT_POINT/dev
		cp -r /dev/console  $MOUNT_POINT/dev/
		cp -r /dev/snapshot $MOUNT_POINT/dev/
		cp -r $RESUME_DEVICE $MOUNT_POINT/dev/
		mkdir $MOUNT_POINT/proc
		mkdir $MOUNT_POINT/etc
		cp $CONFIG_FILE $MOUNT_POINT/etc/
		cp $RESUME $MOUNT_POINT/resume
		ln -s resume $MOUNT_POINT/linuxrc
		umount $MOUNT_POINT
		if [ -f $INITRD_FILE ]; then
			mv $INITRD_FILE $INITRD_FILE.orig
		fi
		chmod 644 $TMP_FILE
		mv $TMP_FILE $INITRD_FILE
	else
		echo "Could not mount $TMP_FILE on $MOUNT_POINT"
	fi
else
	echo "Could not install the resume initrd"
fi

if [ -n "$RMDIR_MOUNT_POINT" ]; then
	rmdir $MOUNT_POINT
fi

