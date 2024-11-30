#!/bin/bash

# Create a new disk image
dd if=/dev/zero of=storage_vgc.img bs=1M count=64

# Format the disk image as ext4
/sbin/mkfs.ext4 storage_vgc.img
