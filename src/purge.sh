#!/bin/bash

# Terminate first
./terminate.sh

# Remove the disk image
sudo rm -f storage_vgc.img

# Clean up mount directory
sudo rm -rf mount
