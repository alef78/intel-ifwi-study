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
    memset(buf, 0xff, MAXLEN);
    rrd("header.bin",buf);
    int klen=rrd("signed.bin",buf+512);
    klen+=511;
    klen-=klen % 512;
    *(int*)(buf+0x30)=klen/512;

    wrt("final.bin",buf,512+klen);

    return 0;
}
