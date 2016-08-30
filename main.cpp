/*
 * main.cpp
 */

#include <cstdlib> //include c library
#include <stdio.h>
#include <string.h>
#include "SPImageProc.h"

extern "C"{
//include your own C source files
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
#include "main_aux.c"
}

int main(int argc, char* argv[]) {
	char* filename = "spcbir.config"; //default name
	FILE* file = NULL;
	SP_CONFIG_MSG* msg = NULL;
	SP_LOGGER_MSG* logMsg = NULL;
	SPConfig config = NULL;
	char* imagePath = NULL;
	SPPoint* imFeatures = NULL;
	int* numOfFeats = NULL;
	char* query = NULL;
	int numOfImages;
	int i;

	/* should be at most 2 arguments: program name and config name */
	if(argc > 2) {
		printf("Invalid command line : use -c <config_filename>\n");
	}
	/* not sure if we really need to check that case */
	if(strcmp(argv[0], ".\\SPFinalProject") != 0) {
		printf("Invalid command line : use -c <config_filename>\n");
	}

	/* the user entered config name, so default name shouldn't be used*/
	if(argc == 2) {
		filename = argv[1];
	}

	/** creating SPConfig **/

	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	if (msg == NULL) {
		return terminate(config, msg);
	}

	config = spConfigCreate(filename, msg);

	if(*msg == SP_CONFIG_CANNOT_OPEN_FILE) {
		if(strcmp(filename, "spcbir.config") == 0) {
			printf("The default configuration file spcbir.config couldn’t be open\n");
		} else {
			printf("The configuration file <filename> couldn’t be open\n");
		}
		return terminate(config, msg);;
	} else if (*msg != SP_CONFIG_SUCCESS) {
		return terminate(config, msg);
	}

	/** creating SPLogger **/

	*logMsg = createLogger(config);
	if (*logMsg != SP_LOGGER_SUCCESS) {
		return terminate(config, msg);
	}

	/** extracting features from images or from feats file **/

	numOfFeats = (int*)malloc(sizeof(int));
	if (numOfFeats == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		spLoggerPrintError(allocFail, __FILE__, __func__, __LINE__);
		return terminate(config, msg);
	}

	numOfImages = spConfigGetNumOfImages(config, msg);
	ImageProc(config);

	if (spConfigIsExtractionMode(config, msg)) {
		for (i = 1; i <= numOfImages; i++) {
			*msg = spConfigGetImagePath(imagePath, config, i);
			if (*msg != SP_CONFIG_SUCCESS) {
				spLoggerPrintError(imPathErr, __FILE__, __func__, __LINE__);
				free(numOfFeats);
				return terminate(config, msg);
			}
			imFeatures = getImageFeatures(imagePath, i, numOfFeats);
			if (imFeatures == NULL) {
				spLoggerPrintError(unknownErr, __FILE__, __func__, __LINE__);
				free(numOfFeats);
				return terminate(config, msg);
			}

			createFeaturesFile(imFeatures, msg, numOfFeats, config, i);
			free(imFeatures);
		}
	} else if (!nonExtractionModeLegal(config, msg, numOfImages)) {
		spLoggerPrintError(featsFileInvalid, __FILE__, __func__, __LINE__);
		free(numOfFeats);
		return terminate(config, msg);
	}

	free(numOfFeats);

	/** for each query find and present nearest images according to features **/

	printf("Please enter an image path:\n");
	while (gets(query) != NULL && strcmp(query, "<>") != 0) {

		//TODO now we need to sort!
		/*
		 * 1 - store all features in KD-Tree
		 * 2 - for each feature of query image, find the k-nearest features
		 * 3 - for each image, keep track of the number of times it was among
		 *     the nearest features
		 * 4 - display the nearest images
		 */

		if (spConfigMinimalGui(config, msg)) {

			for (i = 0; i < spConfigGetNumOfSimIms(config, msg); i++) {

				showImage(imagePath);
				getchar();
				//until user presses some key, the same image will
				//show on screen
				free(imagePath);
			}
		} else {
			printf("Best candidates for - <query image path> - are:\n");

			for (i = 0; i < spConfigGetNumOfSimIms(config, msg); i++) {

				printf("%s\n", imagePath);
				free(imagePath);
			}
		}
	}


	return terminate(config, msg);
}


