#!/bin/bash
fusermount -u mount-point
rm -R -f test
rm -R -f obj
rm virtual-disk
rm fs-fuse
rm -R -f mount-point
echo "rm done"
