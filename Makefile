CC=gcc -Os -Wl,--strip-debug
all:
	$(CC) unp.c -o unp
	$(CC) pack.c -o pack
	$(CC) unpboot.c -o unpboot
	$(CC) mkboot.c -o mkboot
	$(CC) mkbootfinal.c -o mkbootfinal

