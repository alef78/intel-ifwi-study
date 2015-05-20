#!/biin/sh

cd $1.unp
cd initrd && find . | cpio -o -H newc |gzip -c -9 >../initrd.cpio.gz && cd ..
../mkboot
../isu -i unsigned.bin -o signed.bin -t 4 -l ../key.pem
../mkbootfinal
cp final.bin ../patched.$1
