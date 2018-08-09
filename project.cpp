#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include "project.h"

/*
 * prompt is a void function that is called in every loop of main
 * in order to display the current working directory
 * and custom ./main shell prompt
 */

void prompt(){

	write(1, "1730sh:", 7);
	char* wd = getenv("PWD");
	write(1, wd, strlen(wd));
	write(1, "$ ", 2);

}
