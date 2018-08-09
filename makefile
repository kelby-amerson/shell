Executables: main.o project.o
	g++ -o main main.o project.o

main.o: main.cpp
	g++ -c -Wall -std=c++14 -g -O0 -pedantic-errors main.cpp

project.o: project.cpp
	g++ -c -Wall -std=c++14 -g -O0 -pedantic-errors project.cpp

clean:
	/bin/rm *.o main
