#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define  MAXLEN 16384*1024

void wrt(char *name, char *ptr, int len)
{
    int ofd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0660);
    write(ofd, ptr, len);
    close(ofd);
}

int rrd(char *name, char *ptr)
{
    int ifd = open(name,O_RDONLY);
    if (ifd < 0) {
	printf("cannot open %s\n", name);
	return 1;
    }
    int len = read(ifd, ptr, MAXLEN);
    printf("read len=%x\n", len);
    close(ifd);
    return len;
}

int main(int argc, char *argv[])
{
    char *buf=(char *)malloc(MAXLEN);;
    memset(buf, 0, MAXLEN);
    rrd("cmdline",buf);
    rrd("bootstub",buf+0x1000);
    int klen=rrd("bzImage",buf+0x1000+8192);
    int rlen=rrd("initrd.cpio.gz",buf+0x1000+8192+klen);
    *(int *)(buf+0x400)=klen;
    *(int *)(buf+0x404)=rlen;
    buf[0x40c]=0xff;
    buf[0x410]=0xbd;
    buf[0x411]=0x02;
    buf[0x412]=0xbd;
    buf[0x413]=0x02;
    buf[0x414]=0xbd;
    buf[0x415]=0x12;
    buf[0x416]=0xbd;
    buf[0x417]=0x12;
    wrt("unsigned.bin",buf,0x1000+8192+klen+rlen);

    return 0;
}
