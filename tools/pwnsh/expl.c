/* MIT License Copyright (c) 2017 Hiroshi Shimamoto */
#include <unistd.h>

int main(int argc, char **argv)
{
	write(2, "exploiting\n", 11);

	return write(1, "/bin/sh", 7);
}
