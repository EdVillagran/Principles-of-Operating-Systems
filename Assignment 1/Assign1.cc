#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/wait.h>
using namespace std;

/***********************************************
 * CSCI 480 -Assignment 1 - Semester(Spring) 2020
 * Programmer: Edgar Villagran
 * Section 1
 * TA: Jingwan Li
 * Program Summary: This program is designed to test
 * functions such as fork(), getpid(), getppid(), sleep(),
 * wait(), and system().
 **********************************************/

int main () {
	//In the parent process
	//calling pid, ppid
	cerr << "\nParent PID: "<<getppid()<<endl;
	cerr<<"Current PID: "<<getpid()<<endl;
	
	//using for to create child process
	cerr << "Creating Child...\n";
	int child = fork();

	//if fork failed
	if(child < 0) {
		cout << "Fork Failed returning -1";
		return -1;
	}
	//if fork =0 it succeeded
	if(child == 0){
		
		//in The Child process
		cout<<"\nIn The Child! \n";
		
		//calling pid, ppid
		cerr << "Parent PID: "<<getppid()<<endl;
		cerr<<"Current PID: "<<getpid()<<endl;
		
		//using fork to create a grandchild process
		cerr << "\nCreating Grandchild Process\n";
		int grandchild = fork();
		
		//if fork failed
		if(grandchild < 0) {
			cerr << "The second fork failed... returning -1";
			return -1;
		}
		// If fork succeeded and = 0
		if(grandchild == 0) {
			//In Grandchild process
			cout<<"\nIn the Grandchild!\n";
			
			//calling pid, ppid
			cerr << "Parent PID: "<<getppid()<<endl;
			cerr<<"Current PID: "<<getpid()<<endl<<endl;
			
			//calling sleep function
			cout<<"\nSleeping for 3 seconds\n";
			sleep(3);
			
			//child process terminated so now grandchild
			//is an orphan
			cout<<"\nGrandchild is an Orphan now!\n";
			
			//calling orphan grandchild pid, ppid
			cerr << "Parent PID: "<<getppid()<<endl;
			cerr<<"Current PID: "<<getpid()<<endl;
		
			//calling system ps
			cerr << "Grandchild calling 'ps'\n";
			system("ps");
			
			//grandchild exiting succesfully
			cerr << "\nGrandchild is about to exit\n\n";
			return 0;
		}
		else {
			//in the child process
			cout<<"In Child after second fork()...\n";
			//calling pid,ppid
			cerr << "Parent PID: "<<getppid()<<endl;
			cerr<<"Current PID: "<<getpid()<<endl;
			
			cout<<"Sleeping for 2 sec.\n";
			sleep(2);
			
			//child exciting succesfully
			cout << "\nChild will exit...\n";
			return 0;
		}
	}
	else {
		//back in the parent process
		sleep(2);
		cout<<"\nBack in Parent Process\n";
		
		//calling pid, ppid
		cerr << "Parent PID: "<<getppid()<<endl;
		cerr<<"Current PID: "<<getpid()<<endl;

		//ps should show zombie child in the process
		cout<<"\nCalling 'Ps' Child will appear as zombie\n";
		system("ps");
		sleep(3);
		
		// wait for its child to exit succesfully
		wait(0);
	
		//after child excited it should no longer show in ps
		cout<<"\nParent waited for child to terminate, now printing 'ps'...\n";
		system("ps");
		
		//parent exciting succesfully
		cerr << "\nParent is about to exit succesfully\n";
		return 0;
	}
	return 0;
}
