#!/bin/sh

mkdir $1.unp
cd $1.unp
../unpboot ../$1
mkdir initrd
cd initrd
gzip -c -d ../initrd.cpio.gz|cpio -iv
cd ../..

