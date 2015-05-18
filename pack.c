#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define u32 unsigned int
#define MAX_FW_CHUNK	(128*1024)
#define SUCP_OFFSET	0x1D8000
#define VEDFW_OFFSET	0x1A6000

struct fuph_hdr {
    u32 sig;
    u32 mip_size;
    u32 ifwi_size;
    u32 psfw1_size;
    u32 psfw2_size;
    u32 ssfw_size;
    u32 sucp_size;
    u32 vedfw_size;
    u32 checksum;
};

int rrd(char *name, char *ptr)
{
    int ifd = open(name,O_RDONLY);
    if (ifd < 0) {
	printf("cannot open %s\n", name);
	return 1;
    }
    int len = read(ifd, ptr, 1024*1024);
    printf("read len=%x\n", len);
    close(ifd);
    return len;
}

void wrt(char *name, char *ptr, int len)
{
    int ofd = open(name, O_WRONLY | O_CREAT, 0660);
    write(ofd, ptr, len);
    close(ofd);
}

int xorsum(char *buf)
{
  int c = 0;
  int *ptr = (int *)buf;
  c^=*ptr++;
  c^=*ptr++;
  c^=*ptr++;
  c^=*ptr++;
  c^=*ptr++;
  c^=*ptr++;
  c^=*ptr++;
  c^=*ptr++;

  return c;
}

int main(int argc, char *argv[])
{
    char buf[4096*1024];
    struct fuph_hdr hdr;

    hdr.sig = 0x24485055;

    memset(buf, 0 , 4096*1024);
    char *ptr = buf;
    hdr.mip_size = rrd("1-mip.part", ptr);
    ptr+=hdr.mip_size;
    hdr.ifwi_size = rrd("2-ifwi.part", ptr);
    ptr+=hdr.ifwi_size;
    hdr.psfw1_size = rrd("3-psfw1.part", ptr);
    ptr+=hdr.psfw1_size;
    hdr.ssfw_size = rrd("4-ssfw.part", ptr);
    ptr+=hdr.ssfw_size;
    hdr.psfw2_size = rrd("5-psfw2.part", ptr);

    hdr.vedfw_size = rrd("6-vfw.part", buf+VEDFW_OFFSET);
    hdr.sucp_size = rrd("7-sucp.part", buf+SUCP_OFFSET);

    ptr = buf+SUCP_OFFSET+hdr.sucp_size;
    hdr.mip_size/=4;
    hdr.ifwi_size/=4;
    hdr.psfw1_size/=4;
    hdr.psfw2_size/=4;
    hdr.ssfw_size/=4;
    hdr.vedfw_size/=4;
    hdr.sucp_size/=4;
    hdr.checksum = xorsum((char *)&hdr);
    memcpy(ptr, (char *)&hdr, 36);

    wrt("newifwi.bin", buf, 36+ptr-buf);

    return 0;
}
