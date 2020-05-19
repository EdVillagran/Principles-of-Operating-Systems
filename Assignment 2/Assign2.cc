
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <iostream>
using namespace std;

/***********************************************
 * CSCI 480 -Assignment 2 - Semester(Spring) 2020
 * Programmer: Edgar Villagran
 * Section 1
 * TA: Jingwan Li
 * Due Date: 2/7/2020
 * 
 * Program Summary: The purpose of this assignment
 * is to have 3 processes communicate with each other
 * using pipes. To create the 3 porcess I will use 
 * fork() and have a parent, child, grandchild process.
 * Parent reads from child who reads from grandchild
 * who reads from parent. 
 * 
 **********************************************/

//Creating the Pipes as an array of 2 intigers.
//[0] = Read, [1] = Write
int PipeA[2];
int PipeB[2];
int PipeC[2];

//Creating the FUnction prototypes
void PWork();
void CWork();
void GWork();


int main()
{
	// If the pipe fails exit with error -5
	if (pipe(PipeA) == -1) {
		cerr << "Pipe #1 (A) error";
		exit(-5);
	}
	// If the pipe fails exit with error -5
	if (pipe(PipeB) == -1) {
		cerr << "Pipe #2 (B) error";
		exit(-5);
	}
	// If the pipe fails exit with error -5
	if (pipe(PipeC) == -1) {
		cerr << "Pipe #1 (C) error";
		exit(-5);
	}
	// Using fork to create the child process
	pid_t pid = fork();
	
	// If fork fails print error and exit with -5
	if (pid < 0) 
	{
		cerr << "Fork #1 error";
		exit(-5);
	}
	
	//If the first fork succeeds now in the child process 
	else if (pid == 0)
	{
		//Using fork in the child to create grandchild process
		pid_t pid2 = fork();
		
		//if the grandchild process fails exit with -5
		if (pid2== -1)
		{
			cerr << "Fork #2 error";
			exit(-5);
		}
		
		//fork success now in the grandchild process
		else if (pid2 == 0)
		{
			//close All unused piped and fd's
			close(PipeA[0]);
			close(PipeA[1]);
			close(PipeB[0]);
			close(PipeC[1]);
			
			//Calling Gwork 
			cout<<"The Grandchild Process is ready to proceed.\n";
			GWork();
			
			//close the pipes used after calling GWork()
			close(PipeB[1]);
			close(PipeC[0]);
			
			//exit with successfull status
			exit(0);
		}
		
		else
		{
			//In the Child Process
			
			//close All unused piped and fd's
			close(PipeA[0]);
			close(PipeB[1]);
			close(PipeC[0]);
			close(PipeC[1]);
			
			//Calling CWork
			cout<<"The Child Process is ready to proceed.\n";
			CWork();
			
			//Closing the pipes and fd's used in CWork()
			close(PipeA[1]);
			close(PipeB[0]);
			
			//Child waits for grandchild to finish before exiting
			wait(0);
			
			//exiting with successfull status
			exit(0);
		}
	}
	
	else
	{
		//In the Parent Process
		
		//close all unused piped and fd's
		close(PipeA[1]);
		close(PipeB[0]);
		close(PipeB[1]);
		close(PipeC[0]);
		
		//Calling PWork
		PWork();
		
		//Closing pipes and fd's used in PWork()
		close(PipeC[1]);
		close(PipeA[0]);
		
		//Parent waits for children to finish before exiting
		wait(0);
		
		//Exiting with Succesfull Status
		exit(0);
		
	}
	//Good practice to use return 0
	return 0;
}

/***********************************************
 * PWork()-
 * Pwork starts by writing value to PipeC as a c++ string
 * It will begin the pipe process for CWork and GWork
 * 
 * PWork reads from PipeA[0] and writes to PipeC[1]
 * 
 ***********************************************/
void PWork() 
{
	//initalize the variables to be used in PWork
	//Using C++ strings for Buffer, Value which are 
	//input and output strings
	string Buffer;
	string Value;
	long int M = 1; // long int used to store computations
	char Delimiter='@'; //Using a delimiter to identify the end
	
	Buffer = to_string(M); //Convert to string
	Buffer += Delimiter; //Add delimer to buffer
	
	//Begin the write process
	write(PipeC[1], Buffer.c_str(), Buffer.length());
	cerr<<"Parent Process is ready to proceed.\n";
	cerr << "Parent:       Value = " << M << endl;
	
	//While M is within range keep working
	while (M <= 999999999 && M >= -999999999)
	{
		Value = "";
		Buffer = "";
		char EndChar = '\0';
		
		//read from previous process, stop at delimiter
		while (EndChar != Delimiter)
		{
			read(PipeA[0], &EndChar, 1);
			
			if (EndChar != Delimiter)
				Value += EndChar;
		}
		
		M = stol(Value);// Extract the number from M
		M = 200 -3 *M;  //Apply given calculation
		Buffer = to_string(M); //Convert BAck to string then = to buffer
		
		//print values
		cerr << "Parent:       Value = " << Buffer << endl;
		
		Buffer += Delimiter;
		
		//write newly calculated value to the next process 
		write(PipeC[1], Buffer.c_str(), Buffer.length());
	}
	
	//Exit Successfuly
	exit(0);

}
/***********************************************
 * CWork()-
 * 			Will Read from PipeB convert the string
 * to number, do calculation given then convert
 * back to string and write to PipeA
 * ********************************************/
void CWork()
{
	//initalize
	string Buffer;
	string Value;
	long int M = 1;
	char Delimiter='@';
	char EndChar = '\0';
	
	while (M <= 999999999 && M >= -999999999)
	{
		Value = "";
		Buffer = "";
		EndChar = '\0';
		
		
		//read from previous process, stop at Delimiter
		while (EndChar!= Delimiter)
		{
			//Read from pipeB change char ReadBuff
			read(PipeB[0], &EndChar, 1);
			
			if (EndChar != Delimiter)
				Value += EndChar;
		}
		
		M = stol(Value); //Turn to int
		M = 7 * M - 6; //Do Calculations
		Buffer = to_string(M); // Store as string
		
		cerr << "Child:        Value = " << Buffer << endl;
		Buffer += Delimiter;
		
		//write newly calculated value to the next process 
		write(PipeA[1], Buffer.c_str(), Buffer.length());
	}
	
	exit(0);
}
/***********************************************
 * GWork()-
 * 			Will Read from PipeC convert the string
 * to number, do calculation given then convert
 * back to string and write to PipeB
 * ********************************************/
void GWork()
{
	//initalize
	string Buffer;
	string Value;
	long int M = 1;
	char Delimiter='@';
	char EndChar = '\0';
	
	//While in Range
	while (M <= 99999999 && M >= -999999999)
	{
		Value = "";
		Buffer = "";
		EndChar = '\0';
		
		//read from previous process, stop at delimiter
		while (EndChar!= Delimiter)
		{
			//Read from pipeC change char ReadBuff
			read(PipeC[0], &EndChar, 1);
			
			//If it doesnt equal the delimer then add readbuff to value
			if (EndChar!= Delimiter)
				Value += EndChar;
		}
		
		M = stol(Value); //turn to int
		M = 30 - 4 * M;  //Do calculations
		Buffer = to_string(M); //store as string

		//print values
		cerr << "Grandchild:   Value = " << Buffer << endl;

		Buffer += Delimiter;
		
		//write to pipeB
		write(PipeB[1], Buffer.c_str(), Buffer.length());
	}
	
	//Exit Successfully
	exit(0);
	
}
