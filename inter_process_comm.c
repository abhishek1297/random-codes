﻿#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

int main() {
	
	pid_t pid;
	int status, dead; //status of the child process
	/* 
		file_descriptors : 
		pip[0] for input stream
		pip[1] for output stream
	*/
	int pip[2];
	char *cmd[] = {"whoami", NULL};
	char str[100]; 
	/*
		The pipe system call finds the first two available positions
		in the process’s open file table and allocates them for 
		the read and write ends of the pipe
	*/
	pipe(pip);
	
	switch(pid = fork()) {
		
		case -1 : // Error creating Child
			printf("Error Creating Child");
			exit(0);
		break;
		
		case 0 : //child created
		
			//Child will execute this part
			close(pip[0]); // It's Needless
			close(1); // close(stdout) It's Needless
			/*
				dup() uses the lowest-numbered unused descriptor for the new descriptor
				or acts an alias. (not in this case)
				0	Standard input	STDIN_FILENO	stdin
				1	Standard output	STDOUT_FILENO	stdout
				2	Standard error	STDERR_FILENO	stderr
				we have closed stdout therefore it is available for use
				then pip[1] = 1
			*/
			dup(pip[1]);
			/*
				Replacing Current Process Image with New Process image
				is required because the child is the exact copy of it's parent's
				therefore they both share the same files, descriptors, and the currently executing
				object (binary) file, etc...
				So if you want the child to execute some other system calls which are object files 
				exec family is used to execute them
				
				pip[1] is stdout for sending the data to parent.
				So Whatever output is generated by execvp will be sent on the pipe rather than stdout.
			*/
			execvp(cmd[0], cmd);
			sleep(10);
		break;
		
		default : //Parent Process will execute this part
			/*
				Exactly opposite will be done for Parent Process
				close stdin
				set pip[0] as stdin
				close pip[1] as It is not used
			*/
			close(0);
			close(pip[1]);
			dup(pip[0]);
			fgets(str, 100, stdin);
			printf("Who Am I (Child >>): %s", str);
			//calling process will wait until child returns from execution.
			dead = wait(&status);
			printf("Child(P%d) has Executed and Returned : %d\n", dead, status);
		break;
	}
	
	return 0;
}