/*
 * gettfds.cpp
 *
 *  Created on: Feb 9, 2013
 *      Author: jordan
 */

#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
using namespace std;

#define FOLDER "test"
#define EXT "txt"

int main(int argc, char **argv){
	char fileName[BUFSIZ];
	int fd = 0;
	for (int i = 1; fd != -1; ++i){
		sprintf(fileName, "%s/%d.%s", FOLDER, i, EXT);
		fd = open(fileName, O_RDONLY);
		if (fd != -1)
			cout << fd << " ";
	}
	cout << endl;
}
