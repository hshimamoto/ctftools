/* MIT License Copyright (c) 2017 Hiroshi Shimamoto */
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
	char buf[256];
	int ret;

	puts("start vuln");

	ret = read(0, buf, 256);

	if (!strcmp(buf, "/bin/sh")) {
		puts("exploit works");
		execl("/bin/sh", "/bin/sh", NULL);
	}

	return 0;
}
