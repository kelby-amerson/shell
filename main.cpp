#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include "project.h"
#include <iostream>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
using namespace std;

/* 
 * This project's goal is to implement our own copy of a unix shell utilizing system calls
 * such as fork and exec. The program will be able to parse user inputs into commands to be 
 * executed. Environment variables can also be changed using the syntax "export NAME=VAL".
 * It also supports redirection through changing STDIN and STDOUT file descriptors
 * to the preferred file. The program will run indefinitely until user input as led to a fatal error
 * or the user types "exit"
 */

int main(){

	//while loop to run until user inputs "exit"
	while(true){
	
		string s1;

		//print directory as prompt
		prompt();	

		//get string of user input
		getline(cin, s1);
	
		//exit conditional
		if(s1 == "exit"){
			cout << "Exiting..." << endl;
			exit(EXIT_SUCCESS);
		}

		//use a stringstream to extract user input into arrays
		stringstream ss(s1);

		//declare arrays for user input
		const char* arr[15] = { NULL };
		char arr2[100][15];
		string s[15];

		//loop to store user input in string array
		int count = 0;
		string s2;
		while(ss >> s2){
			s[count] = s2;
			count++;
		}

		//convert string array to const cstring
		for(int i = 0; i < count; i++){
			arr[i] = s[i].c_str();
		}

		//convert const cstring to non const cstring for later use
		for(int i = 0; i < count; i++){
			if(arr[i] == NULL)
				perror("array at i is null");
			else
				strcpy(arr2[i], arr[i]);
		}
		
		//check if the input command is export to change environment variable
		if(s[0] == "export"){

			//two while loops to split user NAME=VAR input into two separate cstring
			int i = 0;
			char name[100];
			while(arr2[1][i] != '='){
				name[i] = arr2[1][i];
				i++;
			}

			i++;
			char val[100];
			int k = 0;
			while(arr2[1][i] != '\0'){
				val[k] = arr2[1][i];
				i++;
				k++;
			}
			
			//set desired env
			if(setenv(name, val, 1) != 0){
				perror("Error inputting environment variable");
				exit(6);
			}
			continue;
		}

		//setup variables for redirection
		int redir = 0;
		int fd = 0;
		int storeStd = 0;
		bool isRedir = false;
		
		//for loop to check if there is a file rediriction in the input command
		//if there is a redirection, open file and redirect STDIN or STDOUT
		for(int i = 0; i < count; i++){
			if(s[i] == ">" || s[i] == ">>"){
				redir = i;
				isRedir = true;
				if(s[i] == ">")
					fd = open(arr[i + 1], O_TRUNC | O_WRONLY | O_CREAT);
				else
					fd = open(arr[i + 1], O_APPEND | O_WRONLY | O_CREAT);
				
				if(fd == -1){
					perror("Error opening file");
					exit(3);
				}
	
				storeStd = dup(STDOUT_FILENO);
				int dupStd = dup2(fd, STDOUT_FILENO);
				if(dupStd == -1){
					perror("Error setting STDOUT to file");
					exit(8);
				}
				break;
			}
			else if(s[i] == "<"){
				redir = i;
				isRedir = true;
				
				fd = open (arr[i + 1], O_RDONLY);
				if(fd == -1){
					perror("Error opening file");
					exit(5);
				}
				storeStd = dup(STDIN_FILENO);
				int dupStd = dup2(fd, STDIN_FILENO);
				if(dupStd == -1){
					perror("Error setting STDIN to file");
					exit(7);
				}
				break;	
			}		
		}

		//double char array to store arguments for execvp
		//first element is path for commands
		char* arr3[15];
		char path[6] = {'/', 'b', 'i', 'n', '/', '\0'};
		arr3[0] = path;

		//check if there is a redirection to build a correct argument array with NULL ending
		if(isRedir){
			for(int i = 1; i < redir; i++){
				arr3[i] = arr2[i];
			}
			arr3[redir] = NULL;
		}
		else{
			for(int i = 1; i < count; i++){
				arr3[i] = arr2[i];
			}
			arr3[count] = NULL;
		}

		//fork a child process to perform execvp command and error check
		int pid = 0;
		if((pid = fork()) < 0){
			perror("error forking");
			exit(1);
		}
		else if(pid == 0){
			if(execvp(arr[0], arr3) < 0){
				perror("exec error");
				exit(2);
			}
		}
		else {
			int w;
			wait(&w);
		}


		//if there is a file redirection, restore STDIN or STDOUT, close the file, and error check
		if(isRedir){
			if(s[redir] == "<"){
				dup2(storeStd, STDIN_FILENO);
			}
			else{
				dup2(storeStd, STDOUT_FILENO);
			}
			
			if(close(fd) == -1){
				perror("Error closing file");
				exit(4);
			}
		}	
	}

	return 0;

}
