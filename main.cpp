/*
 * main.cpp
 */

#include <cstdlib> //include c library
#include <stdio>

extern "C"{
//include your own C source files
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
#include "mainaux.c"
}

int main(int argc, char* argv[]) {
	char* filename = "spcbir.config"; //default name
	FILE* file = NULL;
	SP_CONFIG_MSG* msg;
	SPConfig config;
	int i;

	/* should be at most 2 arguments: program name and config name */
	if(argc > 2) {
		printf("Invalid command line : use -c <config_filename>\n");
	}
	/* not sure if we really need to check that case */
	if(strcmp(argv[0], ".\SPFinalProject") != 0) {
		printf("Invalid command line : use -c <config_filename>\n");
	}

	/* the user entered config name, so default name shouldn't be used*/
	if(argc == 2) {
		filename = argv[1];
	}

	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	if (msg == NULL) {
		//TODO terminate program
		return 0;
	}

	config = spConfigCreate(filename, msg);

	if(msg == SP_CONFIG_CANNOT_OPEN_FILE) {
		if(strcmp(filename, "spcbir.config") == 0) {
			printf("The default configuration file spcbir.config couldn’t be open\n");
		} else {
			printf("The configuration file <filename> couldn’t be open\n");
		}
		//TODO terminate program
		return 0;
	} else if (msg != SP_CONFIG_SUCCESS) {
		//TODO terminate program
		return 0;
	}

	if (spConfigIsExtractionMode(config, msg)) {
		for (int i = 1; i <= spConfigGetNumOfImages(config, msg); i++) {
			extractFeaturesFromIm(config, msg, i);
		}
	}


	return 1;
}


