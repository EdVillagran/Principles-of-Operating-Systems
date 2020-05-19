#Edgar Villagran
#Assignment 5
CCFLAGS = -Wall -std=c++11 -pthread

Assign5: Assign5.o
	g++ $(CCFLAGS) -o Assign5 Assign5.o

 	# Rules to compile source code file to object code
Assign5.o: Assign5.cc
		g++ $(CCFLAGS) -c Assign5.cc


clean:
	-rm *.o Assign5
