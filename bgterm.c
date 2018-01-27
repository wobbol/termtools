#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

char *pname;
char *term_name = "fbterm";
char *const set_bg[] = {
	"fbv",
	"-ciuker",
	"PATH", /* image file path goes here. */
	(char *)NULL};


int main(int argc, char *argv[])
{
	pname = argv[0];

	int fd[2];
	pipe(fd);
	pid_t pid = fork();
	if(pid == 0) { //child
		close(fd[1]);
		if(dup2(fd[0], 0) < 0) {
			perror("Could not setup pipe:");
			exit(1);
		}
		printf("\e[?25l");
		fflush(stdout);
		execvp(set_bg[0], set_bg);
	} else if(pid == -1) {
		perror("Could not fork:");
		return 1;
	}
	close(fd[0]);
	write(fd[1], "q", 1);
	wait(NULL);

	if(setenv("FBTERM_BACKGROUND_IMAGE", "1", 1)) {
		perror("Could not set FBTERM_BACKGROUND_IMAGE");
	}

	argv[0] = term_name;
	pid = fork();
	if(pid == 0) { //child
	execvp(argv[0], argv);
	} else if(pid == -1) {
		perror("Could not fork:");
	}
	wait(NULL);
	if(setenv("FALLBACK_SHELL", "1", 1)) {
		perror("Could not set FALLBACK_SHELL");
		return 1;
	}
	execlp("bash","bash", "-i", (char *)NULL);
}
