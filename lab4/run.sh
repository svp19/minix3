cp schedule.c /usr/src/minix/servers/sched/;
cp system.c /usr/src/minix/kernel/;
cd /usr/src/;
make build MKUPDATE=yes >log.txt 2>log.txt
