cp ./open.c /usr/src/minix/servers/vfs/open.c
cp ./read.c /usr/src/minix/servers/vfs/read.c
cp ./link.c /usr/src/minix/servers/vfs/link.c
make build MKUPDATE=yes >log.txt 2>log.txt