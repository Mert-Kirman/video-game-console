#!/bin/bash

# Ensure mount directory exists
MOUNT_DIR="mount"
DEVICE_FILE="vgc_device"
SYMLINK="./<device-file>"

mkdir -p $MOUNT_DIR

# Attach the image to a loopback device
LOOP_DEVICE=$(sudo losetup -f)
sudo losetup $LOOP_DEVICE storage_vgc.img

# Mount the loopback device
sudo mount $LOOP_DEVICE $MOUNT_DIR

# Create a symbolic link for the device file
ln -sf $LOOP_DEVICE $SYMLINK
