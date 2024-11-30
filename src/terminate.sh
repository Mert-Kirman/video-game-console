#!/bin/bash

# Ensure mount directory exists
MOUNT_DIR="mount"
DEVICE_FILE="vgc_device"
SYMLINK="./<device-file>"

# Unmount the loopback device
sudo umount $MOUNT_DIR

# Detach the loopback device
LOOP_DEVICE=$(sudo losetup -l | grep "storage_vgc.img" | awk '{print $1}')
if [ -n "$LOOP_DEVICE" ]; then
    sudo losetup -d $LOOP_DEVICE
fi

# Remove symbolic link
sudo rm -f $SYMLINK
