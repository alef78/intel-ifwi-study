#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

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

void wrt(char *name, char *ptr, int len)
{
    int ofd = open(name, O_WRONLY | O_CREAT, 0660);
    write(ofd, ptr, len);
    close(ofd);
}

int xorsum(char *buf, int len)
{
  buf+=len;
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
    if (argc < 2) {
	printf("usage: unp ifwi.bin\n");
	return 1;
    }
    int ifd = open(argv[1],O_RDONLY);
    if (ifd < 0) {
	printf("cannot open %s\n", argv[1]);
	return 1;
    }
    int len = read(ifd, buf, 4096*1024);
    printf("ifwi len=%x\n", len);
    close(ifd);
    len-=sizeof(struct fuph_hdr);
    struct fuph_hdr *hdr = (struct fuph_hdr *)(buf+len);
    printf("ifwi sig=%x, chk=%x\n", hdr->sig, hdr->checksum);
    printf("ifwi xor sum=%x\n", xorsum(buf, len));
    hdr->mip_size*=4;
    hdr->ifwi_size*=4;
    hdr->psfw1_size*=4;
    hdr->psfw2_size*=4;
    hdr->ssfw_size*=4;
    hdr->vedfw_size*=4;
    hdr->sucp_size*=4;

    char *ptr = buf;
    int clen = hdr->mip_size;
    wrt("1-mip.part", ptr, clen);
    ptr+=clen;
    clen = hdr->ifwi_size;
    wrt("2-ifwi.part", ptr, clen);
    ptr+=clen;
    clen = hdr->psfw1_size;
    wrt("3-psfw1.part", ptr, clen);
    ptr+=clen;
    clen = hdr->ssfw_size;
    wrt("4-ssfw.part", ptr, clen);
    ptr+=clen;
    clen = hdr->psfw2_size;
    wrt("5-psfw2.part", ptr, clen);
    ptr=buf+VEDFW_OFFSET;
    clen = hdr->vedfw_size;
    wrt("6-vfw.part", ptr, clen);
    ptr=buf+SUCP_OFFSET;
    clen = hdr->sucp_size;
    wrt("7-sucp.part", ptr, clen);

    return 0;
}
