/*
 * main.cpp
 */

#include <cstdlib> //include c library
#include <stdio.h>
#include <string.h>
#include "SPImageProc.h"

using sp::ImageProc;

extern "C" {
#include "SPPoint.h"
#include "SPLogger.h"
#include "SPConfig.h"
#include "SPFeaturesSerializer.h"
#include "SPKDTree.h"
}

#ifndef MAX_PATH
#define MAX_PATH 1024
#endif

/*
 * Creates logger according to information from config
 *
 * @para config
 *
 * @return SP_LOGGER_INVAlID_ARGUMENT - if config == NULL
 * @return SP_LOGGER_DEFINED 			- The logger has been defined
 * @return SP_LOGGER_OUT_OF_MEMORY 		- In case of memory allocation failure
 * @return SP_LOGGER_CANNOT_OPEN_FILE 	- If the file given by filename cannot be opened
 * @return SP_LOGGER_SUCCESS - if logger was created successfully
 */
SP_LOGGER_MSG createLogger(SPConfig config) {
	char* filename;
	int level;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;

	level = spConfigGetLogLevel(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	filename = spConfigGetLogName(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	return spLoggerCreate(filename, (SP_LOGGER_LEVEL) level);
}

/*
 * wraps up the program and exists with a given message code
 * @return 1
 */
int terminate(SPConfig config, SP_CONFIG_MSG msg) {
	if (msg == SP_CONFIG_SUCCESS) {
		printf("Exiting...\n");
	}

	if (config != NULL) {
		spConfigDestroy(config);
	}

	spLoggerDestroy();
	exit(msg);
}

/*
 * main entry point, returns status code
 */
int main(int argc, char* argv[]) {
	const char* filename = "spcbir.config"; //default name
	SP_CONFIG_MSG msg;
	SP_LOGGER_MSG logMsg;
	SPConfig config = NULL;
	char imagePath[MAX_PATH];
	SPPoint* imFeatures = NULL;
	int numOfFeats;
	int numOfImages;
	int i, j;
	ImageProc* imageProc = NULL;
	SPPoint** featuresPerImage;
	int* featureCountPerImage;
	int totalFeatures = 0;
	SPPoint* allFeatures;
	int featureIndex;
	SPKDArray* kdArray;
	SPKDTreeNode* kdTree;
	char queryPath[MAX_PATH];
	
	setvbuf (stdout, NULL, _IONBF, BUFSIZ);

	// should be at most 2 arguments: program name and config name
	if (argc > 3 || argc == 2) {
		return terminate(NULL, SP_CONFIG_INVALID_COMMANDLINE);
	}

	// the user entered config name, so default name shouldn't be used
	if (argc == 3) {
		if (strcmp("-c", argv[1]) != 0) {
			return terminate(NULL, SP_CONFIG_INVALID_COMMANDLINE);
		}
		filename = argv[2];
	}

	// creating SPConfig

	config = spConfigCreate(filename, &msg);

	if (msg == SP_CONFIG_CANNOT_OPEN_FILE) {
		if (strcmp(filename, "spcbir.config") == 0) {
			printf("The default configuration file spcbir.config couldn’t be open\n");
		} else {
			printf("The configuration file %s couldn’t be open\n", filename);
		}
		return terminate(config, msg);
	} else if (msg != SP_CONFIG_SUCCESS) {
		return terminate(config, msg);
	}

	// creating SPLogger

	logMsg = createLogger(config);
	if (logMsg != SP_LOGGER_SUCCESS) {
		return terminate(config, SP_CONFIG_UNKNOWN_ERROR);
	}

	// extracting features from images or from feats file

	numOfImages = spConfigGetNumOfImages(config, &msg);
	imageProc = new ImageProc(config);

	if (spConfigIsExtractionMode(config, &msg)) {
		for (i = 0; i < numOfImages; i++) {
			msg = spConfigGetImagePath(imagePath, config, i);
			if (msg != SP_CONFIG_SUCCESS) {
				spLoggerPrintError(imPathErr, __FILE__, __func__, __LINE__);
				return terminate(config, msg);
			}
			imFeatures = imageProc->getImageFeatures(imagePath, i, &numOfFeats);
			if (imFeatures == NULL) {
				spLoggerPrintError(unknownErr, __FILE__, __func__, __LINE__);
				return terminate(config, msg);
			}

			msg = writeImageFeaturesToFile(imFeatures, numOfFeats, config, i);
			if (msg != SP_CONFIG_SUCCESS) {
				terminate(config, msg);
			}
			for (j = 0; j<numOfFeats; j++) {
				spPointDestroy(imFeatures[j]);
			}
			free(imFeatures);
		}
	}

	// importing features from files

	featuresPerImage = (SPPoint**) malloc(sizeof(SPPoint*) * numOfImages);
	VERIFY_ALLOC(featuresPerImage);
	featureCountPerImage = (int*) malloc(sizeof(int) * numOfImages);
	VERIFY_ALLOC(featureCountPerImage);

	for (i = 0; i < numOfImages; i++) {
		readImageFeaturesFromFile(&(featuresPerImage[i]),
				&(featureCountPerImage[i]), config, i);
		if (msg != SP_CONFIG_SUCCESS) {
			terminate(config, msg);
		}
		totalFeatures += featureCountPerImage[i];
	}

	allFeatures = (SPPoint*) malloc(sizeof(SPPoint) * totalFeatures);
	VERIFY_ALLOC(allFeatures);
	featureIndex = 0;
	for (i = 0; i < numOfImages; i++) {
		for (j = 0; j < featureCountPerImage[i]; j++, featureIndex++) {
			allFeatures[featureIndex] = featuresPerImage[i][j];
		}
	}

	// building kd-array
	
	kdArray = spKDArrayInit(allFeatures, totalFeatures,
			spConfigGetPCADim(config, &msg));
	VERIFY_ALLOC(kdArray);

	free(allFeatures);
	for (i = 0; i < numOfImages; i++) {
		for (j = 0; j < featureCountPerImage[i]; j++, featureIndex++) {
			spPointDestroy(featuresPerImage[i][j]);
		}
		free(featuresPerImage[i]);
	}
	free(featuresPerImage);
	free(featureCountPerImage);

	// building kd-tree

	kdTree = spKDTreeInit(kdArray,
			spConfigGetSplitMethod(config, &msg));
	VERIFY_ALLOC(kdTree);
	spKDArrayDestroy(kdArray);

	// getting user query until hitting "<>"

	while (true) {
		int queryNumOfFeats;
		SPPoint* queryFeats;
		int* histogram;
		printf("Please enter an image path:\n");

		if (!scanf("%s", queryPath)) {
			break;
		}
		if (strcmp(queryPath, "<>") == 0) {
			break;
		}

		// calculate feats of given query

		
		queryFeats = imageProc->getImageFeatures(queryPath, 0,	&queryNumOfFeats);
		if (queryFeats == NULL) {
			spLoggerPrintError(unknownErr, __FILE__, __func__, __LINE__);
			terminate(config, SP_CONFIG_UNKNOWN_ERROR);
		}

		// compare query feats to all feats in kd-tree and apply to histogram

		histogram = (int*) calloc(sizeof(int), numOfImages);
		VERIFY_ALLOC(histogram);

		for (i = 0; i < queryNumOfFeats; i++) {
			SPBPQueue queue = spKDTreeNearestNeighbor(kdTree, queryFeats[i],
					spConfigGetSpKNN(config, &msg));
			while (!spBPQueueIsEmpty(queue)) {
				SPListElement element = spBPQueuePeekLast(queue);
				int imageIndex = spListElementGetIndex(element);
				histogram[imageIndex]++;
				spListElementDestroy(element);
				spBPQueueDequeue(queue);
			}
			spBPQueueDestroy(queue);
		}

		// extract and display similar images from histogram

		int simIms = spConfigGetNumOfSimIms(config, &msg);
		for (i = 0; i < simIms; i++) {

			int maxImageIndex = 0;
			for (j = 1; j < numOfImages; j++) {
				if (histogram[maxImageIndex] < histogram[j]) {
					maxImageIndex = j;
				}
			}
			histogram[maxImageIndex] = -1;

			msg = spConfigGetImagePath(imagePath, config, maxImageIndex);
			if (msg != SP_CONFIG_SUCCESS) {
				terminate(config, msg);
			}

			if (spConfigMinimalGui(config, &msg)) {
				imageProc->showImage(imagePath);
			} else {
				if (i == 0) {
					printf("Best candidates for - %s - are:\n", queryPath);
				}

				printf("%s\n", imagePath);
			}
		}

		free(histogram);
	}

	spKDTreeDestroy(kdTree);
	delete imageProc;

	return terminate(config, SP_CONFIG_SUCCESS);
}

