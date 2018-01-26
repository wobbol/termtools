#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char *pname;

int bg_set(char *path)
{
	int fd[2], written;
	pipe(fd);
	pid_t pid = fork();

	if(pid == 0) { //child
		close(fd[1]);
		if(dup2(fd[0], 0) < 0) {
			perror("Could not setup pipe:");
			exit(1);
		}
		printf("\e[?25l");
		char *thing = "echo";
		execlp(thing,thing,"-ciuker", path, (char *)NULL);
	} else if(pid == -1) {
		perror("Could not fork:");
		goto err;
	} else { //parent
		close(fd[0]);
		do{
			if((written = write(fd[1],"q",1)) < 0) {
				perror("failed to peacefully close fbv");
				fprintf(stderr, "Killing fbv...\n");
				kill(pid, 9); //TODO: can this fail?
				goto err;
			}
		} while(written > 1); // fragle to changes in the loop
		wait(NULL);
	}

	return 1;
err:
	fprintf(stderr, "Wallpaper not set.\n");
	return 0;
}

char *file_to_buf(char *p)
{
	FILE *fp = fopen(p, "r");

	if(!fp) {
		perror("Error opening file");
		goto error;
	}

	long len;
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *buf = malloc(len);
	if(buf) {
		if(len != fread(buf, 1, len, fp)){
			fprintf(stderr,"Error reading %s\n", p);
			goto error;
		}
	} else {
		perror("Failed to allocate buffer");
	}

	fclose(fp);

	/*
	 * A file normaly has a newline as the last char,
	 * but we don't care about it in this case.
	 */
	if(buf[len-1] == '\n')
		buf[len-1] = '\0';

	return buf;
error:
	buf = NULL;
	fprintf(stderr, "Could not obtain config data\n");
	return NULL;
}

void usage(void)
{
	fprintf(stderr, "usage: %s [wallpaper] --  [fbterm-options]...\n", pname);
}

int main(int argc, char *argv[])
{
	pname = argv[0];
	char *bg_path = NULL;

	for(int i = 0; argv[i]; ++i) {
		if(!strcmp(argv[i],"--")) {
			argc -= i; //TODO: check correctness
			argv = &argv[i];
			break;
		} else if( i == 1) {
			bg_path = argv[i];
		}
	}
	if(argv[0] == pname) {
		fprintf(stderr, "\"--\" not found in arguments.\n");
		usage();
		return 1;
	}
	if(!bg_path){
		char *bgfile = "/.fbtermbg";
		char *home = getenv("HOME");
		char *tmp = malloc(strlen(home) + strlen(bgfile) + 1);
		strcpy(tmp, home);
		strcat(tmp, bgfile);

		bg_path = file_to_buf(tmp);
		free(tmp);
		tmp = NULL;
	}
	if(bg_path){
		if(bg_set(bg_path)) {
			if(setenv("FBTERM_BACKGROUND_IMAGE", "1", 1)) {
				perror("Could not set FBTERM_BACKGROUND_IMAGE");
			}
		}
	} else {
		fprintf(stderr, "Could not find a background to use.\n"
				"Starting fbterm without one");
	}
	argv[0] = "echo";
	execvp(argv[0], argv);
}
