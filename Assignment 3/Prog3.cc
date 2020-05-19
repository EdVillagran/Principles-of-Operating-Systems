#include <unistd.h>
#include <cstring>
#include <sys/wait.h>
#include <iostream>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <string.h>
#include <string>
using namespace std;
/***********************************************
 * CSCI 480 -Assignment 3 - Semester(Spring) 2020
 * Programmer: Edgar Villagran
 * Section 1
 * TA: Jingwan Li
 * Due Date: 2/20/2020
 *
 * Program Summary: We are to create a microshell
 * simulator that is capable of running commands.
 * It should reconize '>+' & '<+' as the redirect
 * command origionally > & <.
 * 
 **********************************************/

int main()
{

	// hold temp command and if >+ or <+ found
	string temp, redirectHold;

	// If the user chooses to continue and not quit
	//Defaulted to true to begin the program
	bool continueCondition = true;

	//used to hold redirrect flags
	bool redirFlag = false; //>+
	bool redirFlag2 = false; //<+

	//used for commands with options
	long unsigned int nextCommIndex = 0;
	//file descriptor
	int fd;
	//hold the state of the program for waitpid
	int stat;
	//used for cin to hold command
	char buf[1024];
	//used when parsing
	char * token;
	//used for fork
	pid_t pid;
	//will contain the command as individuals
	vector<char *> command;

	//Begin program, defaulted to true
	while (continueCondition)
	{
		//clear buffer every round
		memset(buf, '\0', 1024);
		cout<<endl;
		cout << "480shell: ";
		//get complete input
		cin.getline(buf, 1024);
		cout << endl;
		temp = buf;

		//Did the user want to quit
		if ((temp == "q") || (temp == "quit"))
			continueCondition = false;

		// Was the 'about' command ran
		else if (temp == "about")
			cout << "The 480shell is the work of Edgar Villagran, 2020.\n";

		//If any other command was ran
		else if (temp != "") {

			//start parsing with token and strtok
			token = strtok(buf, " ");// holds first command now

			// If the token does not reach NULL then keep going
			while (token != nullptr)
			{
				//If the redirect symbol exist, store in redirectHold
				redirectHold = token;


				if (redirectHold == ">+")//
				{
					//set the redirFlag  for >+ to True
					redirFlag = true;

					//Insert a Null into vector
					command.push_back(nullptr);

					//use for command with options
					nextCommIndex = command.size();
				}

				if (redirectHold == "<+")
				{
					// Set the redirFlag2 for <+ True
					redirFlag2 = true;


					//insert nullptr into the command vector
					command.push_back(nullptr);


					//use for command with options
					nextCommIndex = command.size();
				}
				else//If neather then simply add delimer
					command.push_back(token + '\0');

				token = strtok(nullptr, " ");
			}



			//insert NULL Delimer
			command.push_back(NULL);

			//create the parent child process
			pid = fork();

			//If you are currently in the child
			if (pid == 0)
			{
				//no redirect flag
				if (redirFlag == false && redirFlag2 == false)
				{
					//execute the command
					execvp(command[0], &command[0]);
					cerr << "Couldn't execute \"" << command[0] << "\"!\n";
					exit(128);
				}
				else
				{
					pid = fork();

					if (pid == 0)
					{
						//in the child

						//if >+ found begin process
						if (redirFlag == true) {
							//file descriptor creates a file in the command
							fd = open(command[3], O_RDWR | O_CREAT | O_TRUNC, 0600);

							//closing the fd
							close(1);

							//create dup process
							dup(fd);

							//execute commands
							execvp(command[0], command.data());
							exit(0);
						}
						//if <+ found begin process
						if (redirFlag2 == true) {

							//create the process to read into the command
							int fd2 = open(command[2], O_RDONLY, 0);

							//create dup process
							dup2(fd2, 0);

							//closing the fd
							close(fd2);

							//process command
							execvp(command[0], command.data());
							exit(0);
						}


					}
					//parent (2nd command)
					else if (pid > 0)
					{
						// if >+ found
						if (redirFlag == true) {

							int fd1 = open(command[4], O_WRONLY | O_CREAT | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

							//closing the fd
							close(1);

							//create dup process
							dup(fd1);

							//execute command
							execvp(command[nextCommIndex], &command[nextCommIndex]);
							exit(0);
						}

						else
						{

							continueCondition = false;
						}
					}
				}
			}
			//error
			else if (pid < 0)
			{

				continueCondition = false;
			}

			//Wait for process to change
			if ((pid = waitpid(pid, &stat, 0)) < 0)
				cerr << "waitpid error" << endl;

			//reset command vector
			command.clear();

			//reset flag states
			redirFlag = false;
			redirFlag2 = false;
		}

	}
	exit(0);
}
