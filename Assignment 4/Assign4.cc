#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <cstring>
#include <string.h>
using namespace std;
/*************************************************************
 * CSCI 480 -Assignment 4 - Semester(Spring) 2020
 * Programmer: Edgar Villagran
 * Section 1
 * TA: Jingwan Li
 * Due Date: 3/24/2020
 *
 * Program Summary: We are to create a virtual
 * memory simulation and using FIFO and Demand Paging alogorithms for
 * page replacement. 
 *
 * **********************************************************/

 /*Setinfo takes the input file and inserts the number value into
 * vector pageNumber, and the char into vector modifiedBit
 */
void setInfo(vector<int> &pageNumber, vector<char> &modifiedBit)
{
	//Opening file
	fstream file;
	string word;
	string filename = "data.txt";
	file.open(filename.c_str());

	//flag to go back and fourth between number and char.
	int isnum = 0;

	//iterate through the words in the file
	while (file >> word)
	{
		if (isnum == 0) {
			//Its a number
			int number = stoi(word);
			pageNumber.push_back(number);
			isnum++;
		}
		else {
			//Its the letter
			char c = word[0];
			modifiedBit.push_back(c);
			isnum--;

		}
	}
	file.close();

}
/*
 * setDefault takes in vector frames and map for pages.
 * it gives correct values into the vector for frames
 * and map has the key of # ofpages up to 25 and element of frame.
 * Default frame is -1.
 *
 */
void setDefault(vector<int> &frames, map<int, int> &pages)
{

	for (int i = 100; i <= 300; i += 100)
	{
		for (int j = 1; j <= 5; j++)

			frames.push_back(i + j);
	}

	for (int count = 0; count <= 25; count++) {

		pages.insert(pair<int, int>(count, -1));
	}

}
/*
 * print takes in the frames, pages, modified, and last access as paramaters.
 * it prints the table in the appropriate way.
 * modified will display none,yes,or no, lastaccess will display the int
 * for the last time a page was accessed.
 */
void print(vector<int> frames, map<int, int> pages, vector<bool> modified, vector<int> lastaccess) {

	//heading
	cout << "\n================================================================\n";
	cout << "   Page Number\t   Frame Number    Last Access   \tModified?\n";

	//keep tab on what number the iterator for pages is at
	int count = 0;

	for (auto i = pages.begin(); i != pages.end(); i++) {

		//Dont want to print 0 or No if the page isnt accessed yet. To prevent do following...
		string printm = "  ";
		string plastacc = "  ";

		//if 'W' is on then print yes
		if (modified[count] == true)printm = "Yes";
		// if 'R' is on print no
		if (modified[count] == false)printm = "No";
		//if the page has been accessed then print # of times instead of just 0
		if (lastaccess[count] != 0) plastacc = to_string(lastaccess[count]);
		//if that page has not been accessed yet dont print yes or no for modified
		if (lastaccess[count] == 0)printm = "  ";

		//print map and values
		cout << "\t" << i->first << "\t\t" << i->second << "\t\t" << plastacc << "\t\t" << printm << endl;

		//increase count flag
		count++;
	}
	cout << "==============================================================\n";
}
/*
 * onDemand_FIFO takes in frames, pages, pageNumber,letter as paramaters
 * frames- vector of frames available
 * pages- map that holds page # as key, frames as element.
 * pageNumber-the vector of addresses from the input file.
 * letter- char vector of letters from the input file.
 *
 * called when ./Assign4 F D
 */
void onDemand_FIFO(vector<int> &frames, map<int, int> &pages, vector<int> &pageNumber, vector<char> &letter) {

	//size of total pages
	int vsize = pageNumber.size();
	//times looped
	int time = 0;

	//Initializing all values of lastaccessed and modified bits
	vector<int> lastaccess(26, 0);
	vector<bool> modified(26, false);

	//keep track of the frame location.. comparing unsinged to unsigned values
	unsigned int framelocation = 0;

	//Display Initial Empty Table
	cout << "Initial page table state:\n";
	print(frames, pages, modified, lastaccess);

	cout << "\nBegining Virtual Memory Simulation with on-demand paging and FIFO\n";

	//print flag
	int HOWOFTEN = 10;

	for (int x = 0; x<vsize; x++) {
		time++;
		//reset framelocation if it reaches number of total frames
		if (framelocation == frames.size())framelocation = 0;

		//page location of where it goes
		int pagelocation = pageNumber[x] / 200;


		//defaulting read and write to false
		bool read = false;
		bool write=false;

		//If w set write to true then input true for the modified vector
		if (letter[x] == 'W') {
			write = true;
			modified[pagelocation] = write;
		}

		//if r set true to read and input false in the modified vector.
		if (letter[x] == 'R') {
				read = true;
				modified[pagelocation] = write;
		}

		//Displaying information
		cout << "\nMemory access at " << pageNumber[x] << "-> page " << pagelocation << " at Time = " << time << " : " <<
			"Type = " << letter[x] << endl;

		if (read == true)
			cout << "Read page " << pagelocation << " from disk to frame " << frames[framelocation];

		if (write == true)
			cout << "Write page " << pagelocation << " from disk to frame " << frames[framelocation];

		//input the proper frames into the pages at location of current page
		pages[pagelocation] = frames[framelocation];
		//input the last access info the current page index
		lastaccess[pagelocation] = time;

		//if its been 10 times then print the complete table
		HOWOFTEN--;
		if (HOWOFTEN == 0) {
			print(frames, pages, modified, lastaccess);
			HOWOFTEN = 10;
		}
		//adjust flags

		framelocation++;
	}

}
/*
 * main takes in 2 arguments that determine which algorithm to run
 */
int main(int argc, char** argv){

	//setting initial values
	vector<int> frames;
	map<int,int> pages;
	char* option1=argv[1];
	char* option2=argv[2];
	vector<int> pageNumber;
	vector<char>letter;

	//Because my program is not complete it will disregard the options entered
	//instead of failing when not recieving F D
	cout<<"\nDoing options F D instead of "<<option1<<" "<<option2<<"\n";


	//setting info from input data
	setInfo(pageNumber, letter);

	//setting initial table
	setDefault(frames,pages);

	//calling the function
	onDemand_FIFO(frames,pages,pageNumber,letter);

	return 0;
}
