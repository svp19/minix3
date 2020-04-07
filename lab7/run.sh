cp ./vfs/open.c /usr/src/minix/servers/vfs/open.c;
cp ./vfs/read.c /usr/src/minix/servers/vfs/read.c;
cp ./vfs/link.c /usr/src/minix/servers/vfs/link.c;
cp ./mfs/read.c /usr/src/minix/fs/mfs/read.c;
cp ./mfs/write.c /usr/src/minix/fs/mfs/write.c;
cp ./const.h /usr/src/minix/include/minix/const.h;
cp ./fslib.c /usr/src/minix/lib/libc/gen/fslib.c;
cd /usr/src;
make build MKUPDATE=yes >log.txt 2>log.txt;