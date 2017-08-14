/* MIT License Copyright (c) 2017 Hiroshi Shimamoto */
#define _POSIX_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <signal.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

void usage(void)
{
	puts("Usage: pwnsh <binary|ip:port> <programs>");
	exit(0);
}

int rw(int r, int w)
{
	char buf[4096];
	char *ptr = buf;
	int len = read(r, buf, 4096);

	if (len <= 0)
		return -1;

	while (len > 0) {
		int ret = write(w, ptr, len);

		if (ret == -1) {
			if (errno == EAGAIN || errno == EINTR)
				continue;
			return -1;
		}

		len -= ret;
		ptr += ret;
	}

	return len;
}

int control(int cnt)
{
	char buf[4096];
	int len = read(cnt, buf, 4096);

	if (len <= 0)
		return -1;

	// this message should be info
	char msg[4096 + 8];
	char *ptr, *lf;

	ptr = buf;
	while ((lf = strstr(ptr, "\n"))) {
		*lf = '\0';
		snprintf(msg, 4096 + 8, "[i] %s\n", ptr);
		write(2, msg, strlen(msg));
		ptr = lf + 1;
	}

	return 0;
}

int shell(int r, int w, int in, int out, int cnt)
{
	int max;
	char buf[4096];

	max = r;
	if (max < w)
		max = w;
	if (max < in)
		max = in;
	if (max < out)
		max = out;
	if (max < cnt)
		max = cnt;
	max++;

	for (;;) {
		fd_set fds;

		FD_ZERO(&fds);
		FD_SET(r, &fds);
		FD_SET(in, &fds);
		if (cnt > 0)
			FD_SET(cnt, &fds);

		int ret = select(max, &fds, NULL, NULL, NULL);

		if (ret == -1) {
			if (errno == EINTR)
				continue;
			return -1;
		}

		if (FD_ISSET(in, &fds)) {
			if (rw(in, w) == -1)
				return 0;
		}
		if (FD_ISSET(r, &fds)) {
			if (rw(r, out) == -1)
				return 1;
		}
		if (cnt > 0 && FD_ISSET(cnt, &fds)) {
			if (control(cnt))
				return 1;
		}
	}
}

pid_t launch(const char *path, int *r, int *w, int *c)
{
	int rd[2], wr[2], er[2];

	if (pipe(rd) == -1) {
		puts("pipe failed");
		exit(1);
	}
	if (pipe(wr) == -1) {
		puts("pipe failed");
		exit(1);
	}
	if (pipe(er) == -1) {
		puts("pipe failed");
		exit(1);
	}

	pid_t pid = fork();

	switch (pid) {
	case -1:
		puts("unable to fork()");
		exit(1);
		break;
	case 0:
		// child
		close(0);
		close(1);
		close(2);
		close(rd[1]);
		close(wr[0]);
		close(er[0]);
		dup2(rd[0], 0); // stdin
		dup2(wr[1], 1); // stdout
		dup2(er[1], 2); // stderr
		//
		execl(path, path, NULL);
		exit(1);
		break;
	}

	// parent
	close(rd[0]);
	close(wr[1]);
	close(er[1]);

	*r = wr[0];	// read side of stdout
	*w = rd[1];	// write side of stdin
	*c = er[0];	// read side of stderr

	return pid;
}

void run(const char *path, int r, int w)
{
	int in, out, cnt;

	pid_t pid = launch(path, &in, &out, &cnt);

	shell(r, w, in, out, cnt);

	kill(pid, SIGTERM);
}

void binary(const char *path, int nr, char **progs)
{
	int r, w, c;

	pid_t pid = launch(path, &r, &w, &c);

	for (int i = 0; i < nr; i++)
		run(progs[i], r, w);

	puts(">>> interactive");
	shell(r, w, 0, 1, -1);
	puts("<<< done");

	// kill child here
	kill(pid, SIGTERM);
	sleep(1);
	// paranoia
	kill(pid, SIGKILL);
}

void netcat(const char *ip, int nr, char **progs)
{
	puts("Not implemented yet");
	exit(1);
}

int main(int argc, char **argv)
{
	if (argc < 2)
		usage();

	// check argv[1] is file
	int fd = open(argv[1], O_RDONLY);

	if (fd > 0)
		binary(argv[1], argc - 2, &argv[2]);
	else
		netcat(argv[1], argc - 2, &argv[2]);

	return 0;
}
