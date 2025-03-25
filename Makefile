all:
	@gcc thread_pooling.c -o thread_pooling

sysinfo:
	@gcc sysinfo.c -o sysinfo

clean:
	@rm thread_pooling sysinfo