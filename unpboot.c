#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define  MAXLEN 16384*1024

void wrt(char *name, char *ptr, int len)
{
    int ofd = open(name, O_WRONLY | O_CREAT, 0660);
    write(ofd, ptr, len);
    close(ofd);
}

int main(int argc, char *argv[])
{
    char *buf=(char *)malloc(MAXLEN);;
    if (argc < 2) {
	printf("usage: unpboot boot.img\n");
	return 1;
    }
    int ifd = open(argv[1],O_RDONLY);
    if (ifd < 0) {
	printf("cannot open %s\n", argv[1]);
	return 1;
    }
    int len = read(ifd, buf, MAXLEN);
    printf("img len=%x\n", len);
    close(ifd);
    wrt("header.bin", buf, 512);
    wrt("cmdline",buf+512+480,strlen(buf+512+480));
    int klen=*(int *)(buf+0x7e0);
    int rlen=*(int *)(buf+0x7e4);

    buf+=0x13e0;
    wrt("bootstub",buf,8192);
    buf+=8192;
    wrt("bzImage",buf,klen);
    wrt("initrd.cpio.gz",buf+klen, rlen);

    return 0;
}
