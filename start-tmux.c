#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char *argv[])
{
	pid_t pid = fork();
	switch(pid) {
	case 0:
		argv[0] = "tmux";
		execvp(argv[0], argv);
	case -1:
		perror("Doh'");
		break;
	default:
		waitpid(pid, NULL, 0);
		char *shell = getenv("SHELL");
		if(!shell)
			shell = "sh";
		execlp(shell, shell, "-i", (char *)NULL);
	}

	return 1;
}
