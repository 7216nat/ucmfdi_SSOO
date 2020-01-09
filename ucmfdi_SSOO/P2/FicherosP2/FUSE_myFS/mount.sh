#!/bin/bash
mkdir mount-point
./fs-fuse -t 2097152 -a virtual-disk -f '-d -s mount-point'
