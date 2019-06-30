#include <stdio.h>
#include <string>
#include <vector>
#include <charconv>
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void usage(const char *p)
{
	std::cerr << "Usage: " << p << " [lockfile] [n] [program] arguments..."
		  << std::endl;
	exit(1);
}

void go(int fd, int argc, char **argv)
{
	pid_t p=fork();

	if (p < 0)
	{
		perror("fork");
		exit(1);
	}

	if (p == 0)
	{
		close(fd);
		argc -= 3;
		argv += 3;

		std::vector<char *> new_argv;

		new_argv.reserve(argc+1);

		new_argv.insert(new_argv.end(), argv, argv+argc);
		new_argv.push_back(nullptr);

		execvp(argv[0], &new_argv[0]);

		perror(argv[0]);
		exit(1);
	}

	int status=0;

	if (waitpid(p, &status, 0) < 0)
	{
		perror("waitpid");
		exit(1);
	}

	if (WIFEXITED(status))
		exit(WEXITSTATUS(status));
	exit(1);
}

int main(int argc, char **argv)
{
	if (argc < 4)
		usage(argv[0]);

	off_t n;

	if (std::from_chars(argv[2], argv[2]+strlen(argv[1]), n).ec
	    != std::errc{} || n <= 0)
		usage(argv[0]);

	int fd=open(argv[1], O_RDWR|O_CREAT, 0666);

	if (fd < 0)
	{
		perror(argv[1]);
		exit(1);
	}

	int dosleep=0;

	while (1)
	{
		if (dosleep)
			sleep(1);

		for (off_t i=0; i<n; ++i)
		{
			if (lseek(fd, i, SEEK_SET) >= 0 &&
			    lockf(fd, F_TLOCK, 1) == 0)
			{
				go(fd, argc, argv);
			}
			if (errno != EACCES && errno != EAGAIN)
			{
				perror(argv[1]);
				exit(1);
			}
		}
		dosleep=1;
	}

	return 0;
}
