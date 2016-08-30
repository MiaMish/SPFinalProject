/*
 * mainaux.c
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>

#include "SPPoint.h"
#include "SPConfig.h"
#include "SPConfigUtils.c"
#include "SPLogger.h"

#define extMsg "Exiting…\n"

/** error massages for logger **/
#define featsPathErr "can't get path of feature file\n"
#define featsFileErr "can't open features file\n"
#define featsFileInvalid "the features file for one of the images is invalid\n"
#define allocFail "memory allocation failure\n"
#define imPathErr "can't get path of image\n"
#define unknownErr "unknown error\n"


/*
 * creats logger according to information from config
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

	return spLoggerCreate(filename, (SP_LOGGER_LEVEL)level);
}


/*
 * @param imFeatures - the features extracted from the index-th image in te directory
 * @param numOfFeats - the number of elements in imFeatures
 *
 * Creates features file for the index-th image in directory
 *
 * @return SP_CONFIG_UNKNOWN_ERROR if fopens or sprintf fails
 * @return SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * @return SP_CONFIG_ALLOC_FAIL - if some allocation failed
 * @return SP_CONFIG_SUCCESS if successful
 */
SP_CONFIG_MSG* createFeaturesFile(SPPoint* imFeatures, SP_CONFIG_MSG* msg,
		int* numOfFeats, SPConfig config, int index) {

	FILE* featsFile;
	char* featsPath = (char*)malloc(sizeof(char));
	char* line;
	int sprintfRes;

	*msg = spConfigGetImageFeatsPath(featsPath, config, index);
	if (*msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(featsPathErr, __FILE__, __func__, __LINE__);
		return msg;
	}

	featsFile = fopen(featsPath, "w");
	if (featsFile == NULL) {
		*msg = SP_CONFIG_UNKNOWN_ERROR;
		spLoggerPrintError(featsFileErr, __FILE__, __func__, __LINE__);
		return msg;
	}

	line = (char*) malloc(sizeof(*line));
	if (line == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		spLoggerPrintError(unknownErr, __FILE__, __func__, __LINE__);
		return msg;
	}

	sprintfRes = sprintf(line, "index = %d, number of features = %d",
			index, numOfFeats);
	if (sprintfRes < 0) {
		//sprintf failed
		*msg = SP_CONFIG_UNKNOWN_ERROR;
		spLoggerPrintError(unknownErr, __FILE__, __func__, __LINE__);
		return msg;
	}

	//TODO Dear Mia, please write features to file

	free(line);
	free(featsPath);
	fclose(featsFile);
	return msg;
}

/*
 * @param config - the configuration dictated by the user
 * @param msg - a pointer
 * @param numOfImages - the number of images in directory
 *
 * @return false if there exists an image in directory whose feats file
 * doesn't exists or whose feats file contains more features than allowed
 * according to the configuration
 * @return true otherwise
 */
bool nonExtractionModeLegal(SPConfig config, SP_CONFIG_MSG* msg,
		int numOfImages) {

	FILE* featsFile;
	char* featsPath = (char*) malloc(sizeof(char));
	char* line;
	int* fileIndex = (int*) malloc(sizeof(int));
	int* fileNumOfFeatures;
	int maxNumOfFeatures;

	line = (char*) malloc(sizeof(*line));
	if (line == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		spLoggerPrintError(unknownErr, __FILE__, __func__, __LINE__);
		return false;
	}

	for(int i = 1; i <= numOfImages; i++) {
		*msg = spConfigGetImageFeatsPath(featsPath, config, i);
		featsFile = fopen(featsPath, "r");
		if (featsFile == NULL) {
			*msg = SP_CONFIG_UNKNOWN_ERROR;
			spLoggerPrintError(featsFileErr, __FILE__, __func__, __LINE__);
			return false;
		}
		if (fgets(line, 1024, featsFile) == NULL) {
			*msg = SP_CONFIG_UNKNOWN_ERROR;
			spLoggerPrintError(featsFileErr, __FILE__, __func__, __LINE__);
			fclose(featsFile);
			return false;
		}

		//extracting only 2 integers from first line of file
		sscanf(line, "%*[^0123456789]%d%*[^0123456789]%d%",
				fileIndex, fileNumOfFeatures);

		maxNumOfFeatures = spConfigGetNumOfFeatures(config, msg);

		if(*fileIndex != i || maxNumOfFeatures < *fileNumOfFeatures) {
			*msg = SP_CONFIG_UNKNOWN_ERROR;
			spLoggerPrintError(featsFileInvalid, __FILE__, __func__, __LINE__);
			fclose(featsFile);
			return false;
		}
		fclose(featsFile);
	}

	return true;
}

/*
 * frees config and msg
 * @return 1
 */
int terminate(SPConfig config, SP_CONFIG_MSG* msg) {
	printf(extMsg);

	if (config != NULL) {
		spConfigDestroy(config);
	}
	if (msg != NULL) {
		free(msg);
	}
	spLoggerDestroy();
	return 1;
}

/*
 * TODO depending on what needed for KD-TREE sort,
 * extract features from .feats file
 */


