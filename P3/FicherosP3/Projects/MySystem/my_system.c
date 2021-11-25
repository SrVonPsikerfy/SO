#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

int my_system(const char* command) {
	pid_t child_pid;
	int child;

	child_pid = fork());
	if (child_pid == 0) { //child
		// NULL defines that there are no more args on the execl
		execl("/bin/bash", "/bin/bash", "-c", command, NULL); 
		exit(-1);
	} 
	else { // parent
		// waits to any child process to end
		// &child returns the child process that has ended
		wait(&child);
	}

	// exclusive to the parent process
	return child;
}

int main(int argc, char* argv[])
{
	if (argc!=2){
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

	return my_system(argv[1]);
}

