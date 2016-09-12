#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "SPConfig.h"
#include "SPLogger.h"

/** default values for configuration **/
#define spPCADimensionDefault 20
#define spPCAFilenameDefault "pca.yml"
#define spNumOfFeaturesDefault 100
#define spNumOfSimilarImagesDefault 1
#define spKNNDefault 1
#define spLoggerLevelDefault 3
#define spLoggerFilenameDefault "stdout"
#define spKDTreeSplitMethodDefault MAX_SPREAD
#define spExtractionModeDefault true
#define spMinimalGuiDefault false

/**the range of spPCADimension **/
#define PCADimUpperBound 28
#define PCADimLowerBound 10

/**Error massages to be printed in case configuration create failed **/
#define filenameIsNull "filename is NULL\n"
#define invalidLine "Invalid configuration line\n"
#define invalidValue "Invalid value - constraint not met\n"
#define directoryNotSet "Parameter spImagesDirectory is not set\n"
#define preffixNotSet "Parameter spImagesPrefix is not set\n"
#define suffixNotSet "Parameter spImagesSuffix is not set\n"
#define imageNumNotSet "Parameter spNumOfImages is not set\n"

/** Error massages reported to logger **/
#define configIsNull "config is NULL\n"
#define msgIsNull "msg is NULL\n"
#define argIsNull "one of the arguments is NULL\n"
#define allocFail "memory allocation failure\n"

/** Debug massages reported to logger **/
#define funcCalled "Beginning of function\n"

struct sp_config_t {
	char spImagesDirectory[MAX_SIZE];
	char spImagesPrefix[MAX_SIZE];
	char spImagesSuffix[MAX_SIZE];
	int spNumOfImages;
	int spPCADimension;
	char spPCAFilename[MAX_SIZE];
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SplitMethod spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
	char spLoggerFilename[MAX_SIZE];
};

/*
 * @param config - pointer to an uninitialized SPConfig
 * puts default values in config
 */
void initConfiguration(SPConfig config);

/*
 * recieves a  string and tries to split it into field and value
 * @returns SP_CONFIG_SUCCESS if managed to update a field in config
 * @returns SP_CONFIG_INVALID_STRING if line is not in the correct format
 * @returns SP_CONFIG_INVALID_INTEGER if value is supposed to be an integer,
 * @return SP_CONFIG_INVALID_LINE if field doesn't exist
 * but is not one
 */
void parseConfigLine(char* line, SPConfig config, SP_CONFIG_MSG* msg);

/*
 *
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith file with the suffix given.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages or index < 1
 * - SP_CONFIG_SUCCESS - in case of success
 */

SP_CONFIG_MSG createFilePath(char* imagePath, const SPConfig config, int index,
		char* suffix, const char* func);

/*
 * @assert msg != NULL
 * @returns true if config != NULL
 * and false otherwise
 */
bool getterAssert(const SPConfig config, SP_CONFIG_MSG* msg, const char* func);


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	SPConfig config = NULL;
	FILE* file = NULL;
	char line[MAX_SIZE] = {'\0'};
	int lineCounter = 0;
	int i;

	assert(msg != NULL);

	if (filename == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		printf(filenameIsNull);
		return config;
	}
	file = fopen(filename, "r");
	if (file == NULL) {
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return config;
	}

	config = (SPConfig) malloc(sizeof(struct sp_config_t));
	if (config == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		free(config);
		config = NULL;
		return config;
	}

	initConfiguration(config);

	while (fgets(line, MAX_SIZE, file)) {
		lineCounter++;
		parseConfigLine(line, config, msg);
		if (*msg != SP_CONFIG_SUCCESS) {
			fclose(file);
			if (*msg == SP_CONFIG_INVALID_LINE) {
				printError(filename, lineCounter, invalidLine);
			} else if (*msg == SP_CONFIG_INVALID_INTEGER
					|| *msg == SP_CONFIG_INVALID_STRING
					|| *msg == SP_CONFIG_INVALID_BOOLEAN) {
				printError(filename, lineCounter, invalidValue);
			} else {
				printError(filename, lineCounter,
						"Some error I didn't think about");
			}

			spConfigDestroy(config);
			config = NULL;
			return config;
		}

		//initialize line to be an array of '\0'
		for (i = 0; i < MAX_SIZE; i++) {
			line[i] = '\0';
		}
	}

	fclose(file);

	if (strcmp(config->spImagesDirectory, "") == 0) {
		*msg = SP_CONFIG_MISSING_DIR;
		printError(filename, lineCounter, directoryNotSet);
	} else if (strcmp(config->spImagesPrefix, "") == 0) {
		*msg = SP_CONFIG_MISSING_PREFIX;
		printError(filename, lineCounter, preffixNotSet);
	} else if (strcmp(config->spImagesSuffix, "") == 0) {
		*msg = SP_CONFIG_MISSING_SUFFIX;
		printError(filename, lineCounter, suffixNotSet);
	} else if (config->spNumOfImages == -1) {
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		printError(filename, lineCounter, imageNumNotSet);
	} else {
		*msg = SP_CONFIG_SUCCESS;
	}

	if (*msg != SP_CONFIG_SUCCESS) {
		spConfigDestroy(config);
		config = NULL;
	}

	return config;
}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return false;
	}
	return (config->spExtractionMode == true);
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return false;
	}
	return (config->spMinimalGUI == true);
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return -1;
	}
	return config->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return -1;
	}
	return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return -1;
	}
	return config->spPCADimension;
}

int spConfigGetNumOfSimIms(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return -1;
	}
	return config->spNumOfSimilarImages;
}

int spConfigGetLogLevel(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return -1;
	}
	return config->spLoggerLevel;
}

int spConfigGetSpKNN(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return -1;
	}
	return config->spKNN;
}

char* spConfigGetLogName(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return NULL;
	}
	return config->spLoggerFilename;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index) {

	return createFilePath(imagePath, config, index, config->spImagesSuffix,
			__func__);
}

SP_CONFIG_MSG spConfigGetImageFeatsPath(char* imagePath, const SPConfig config,	int index) {

	return createFilePath(imagePath, config, index, spFeatsSuffix, __func__);
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
	int pathLength;

	if (pcaPath == NULL || config == NULL) {
		spLoggerPrintWarning("The function was called with an invalid argument", __FILE__, __func__, __LINE__);
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	pathLength = sprintf(pcaPath, "%s%s", config->spImagesDirectory,
			config->spPCAFilename);
	if (pathLength < 1) {
		spLoggerPrintError("sprintf function has failed", __FILE__, __func__, __LINE__);
		return SP_CONFIG_UNKNOWN_ERROR;
	}

	return SP_CONFIG_SUCCESS;
}

char* spConfigGetDirectory(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return NULL;
	}
	return config->spImagesDirectory;
}

char* spConfigGetPrefix(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return NULL;
	}
	return config->spImagesPrefix;
}

char* spConfigGetSuffix(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return NULL;
	}
	return config->spImagesSuffix;
}

char* spConfigGetPCAFilename(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return NULL;
	}
	return config->spPCAFilename;
}

SplitMethod spConfigGetSplitMethod(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg, __func__)) {
		return UNDEFINED;
	}
	return config->spKDTreeSplitMethod;
}

void spConfigDestroy(SPConfig config) {
	if (config != NULL) {
		if (logger != NULL) {
			spLoggerDestroy();
		}
		free(config);
		config = NULL;
	}
}

const char* convertMsgToString(SP_CONFIG_MSG* msg) {
	switch (*msg) {
	case 0:
		return "SP_CONFIG_MISSING_DIR";
	case 1:
		return "SP_CONFIG_MISSING_PREFIX";
	case 2:
		return "SP_CONFIG_MISSING_SUFFIX";
	case 3:
		return "SP_CONFIG_MISSING_NUM_IMAGES";
	case 4:
		return "SP_CONFIG_CANNOT_OPEN_FILE";
	case 5:
		return "SP_CONFIG_ALLOC_FAIL";
	case 6:
		return "SP_CONFIG_INVALID_INTEGER";
	case 7:
		return "SP_CONFIG_INVALID_STRING";
	case 8:
		return "SP_CONFIG_INVALID_ARGUMENT";
	case 9:
		return "SP_CONFIG_INVALID_BOOLEAN";
	case 10:
		return "SP_CONFIG_INVALID_LINE";
	case 11:
		return "SP_CONFIG_INDEX_OUT_OF_RANGE";
	case 12:
		return "SP_CONFIG_UNKNOWN_ERROR";
	case 13:
		return "SP_CONFIG_INVALID_COMMANDLINE";
	case 14:
		return "SP_CONFIG_SUCCESS";
	}

	//should not reach this line
	return NULL;
}

/*
 * Helper functions
 */

void parseConfigLine(char* line, SPConfig config, SP_CONFIG_MSG* msg) {
	char value[MAX_SIZE] = {0};
	int fieldId;
	int valueAsNum;
	const char* typeString;

	fieldId = extractFieldAndValue(line, value);
	if (fieldId <= 0) {
		if (fieldId == 0) {
			*msg = SP_CONFIG_SUCCESS;
		} else if (fieldId == -1) {
			*msg = SP_CONFIG_INVALID_LINE;
		}
		return;
	}

	/*
	 * if field is of type int, first we'll make sure the value is positive
	 */
	valueAsNum = convertStringToNum(value);
	if (fieldId == 4 || fieldId == 5 || fieldId == 7 || fieldId == 9
			|| fieldId == 11 || fieldId == 13) {
		if (valueAsNum < 0) {
			*msg = SP_CONFIG_INVALID_INTEGER;
			return;
		}
	}

	switch (fieldId) {
	case 1:
		strcpy(config->spImagesDirectory, value);
		break;

	case 2:
		strcpy(config->spImagesPrefix, value);
		break;

	case 3:
		if (value[0] == '.') {
			for (ImageType imageType = jpg; imageType <= gif; imageType++) {
				typeString = convertTypeToString(imageType);
				if (strcmp(value, typeString) == 0) {
					strcpy(config->spImagesSuffix, typeString);
					return;
				}
			}
		}
		printf("wrong suffix. got '%s'\n", value);
		*msg = SP_CONFIG_INVALID_STRING;
		return;

	case 4:
		config->spNumOfImages = valueAsNum;
		break;

	case 5:
		if (valueAsNum < PCADimLowerBound || valueAsNum > PCADimUpperBound) {
			*msg = SP_CONFIG_INVALID_INTEGER;
			return;
		}
		config->spPCADimension = valueAsNum;
		break;

	case 6:
		strcpy(config->spPCAFilename, value);
		break;

	case 7:
		config->spNumOfFeatures = valueAsNum;
		break;

	case 8:
		if (strcmp(value, "true") == 0) {
			config->spExtractionMode = true;
		} else if (strcmp(value, "false") == 0) {
			config->spExtractionMode = false;
		} else {
			*msg = SP_CONFIG_INVALID_BOOLEAN;
			return;
		}
		break;

	case 9:
		config->spNumOfSimilarImages = valueAsNum;
		break;

	case 10:

		for (SplitMethod splitMethod = RANDOM; splitMethod <= INCREMENTAL; splitMethod++) {
			if (strcmp(value, convertMethodToString((splitMethod))) == 0) {
				config->spKDTreeSplitMethod = splitMethod;
				return;
			}
		}
		*msg = SP_CONFIG_INVALID_STRING;
		return;

	case 11:
		config->spKNN = valueAsNum;
		break;

	case 12:
		if (strcmp(value, "true") == 0) {
			config->spMinimalGUI = true;
		} else if (strcmp(value, "false") == 0) {
			config->spMinimalGUI = false;
		} else {
			*msg = SP_CONFIG_INVALID_BOOLEAN;
			return;
		}
		break;

	case 13:
		if (valueAsNum > 4) {
			*msg = SP_CONFIG_INVALID_INTEGER;
			return;
		}
		config->spLoggerLevel = valueAsNum;
		break;

	case 14:
		strcpy(config->spLoggerFilename, value);
		break;

	default:
		*msg = SP_CONFIG_INVALID_LINE;
		return;
	}

	*msg = SP_CONFIG_SUCCESS;
}

void initConfiguration(SPConfig config) {
	strcpy(config->spImagesDirectory, "");
	strcpy(config->spImagesPrefix, "");
	strcpy(config->spImagesSuffix, "");
	config->spPCADimension = spPCADimensionDefault;
	config->spNumOfFeatures = spNumOfFeaturesDefault;
	config->spExtractionMode = spExtractionModeDefault;
	config->spNumOfSimilarImages = spNumOfSimilarImagesDefault;
	config->spKDTreeSplitMethod = spKDTreeSplitMethodDefault;
	config->spKNN = spKNNDefault;
	config->spMinimalGUI = spMinimalGuiDefault;
	config->spLoggerLevel = spLoggerLevelDefault;
	config->spNumOfImages = -1;
	strcpy(config->spPCAFilename, spPCAFilenameDefault);
	strcpy(config->spLoggerFilename, spLoggerFilenameDefault);
}

SP_CONFIG_MSG createFilePath(char* imagePath, const SPConfig config, int index,
		char* suffix, const char* func) {

	int pathLen;
	char errMsg[256];

	if (imagePath == NULL || config == NULL) {
		sprintf(errMsg, "The function %s was called with an invalid argument",
				func);
		spLoggerPrintError(errMsg, __FILE__, __func__, __LINE__);
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	if (index >= config->spNumOfImages || index <= 0) {
		sprintf(errMsg, "The index given to %s is out of range", func);
		spLoggerPrintError(errMsg, __FILE__, __func__, __LINE__);
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}

	pathLen = sprintf(imagePath, "%s%s%d%s", config->spImagesDirectory,
			config->spImagesPrefix, index, suffix);
	if (pathLen < 1) {
		sprintf(errMsg, "sprintf function has failed");
		spLoggerPrintError(errMsg, __FILE__, __func__, __LINE__);
		return SP_CONFIG_UNKNOWN_ERROR;
	}

	return SP_CONFIG_SUCCESS;
}

bool getterAssert(const SPConfig config, SP_CONFIG_MSG* msg, const char* func) {
	char warningMsg[256];

	assert(msg);
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;

		sprintf(warningMsg, "The function %s was called with an invalid argument"
				, func);
		spLoggerPrintWarning(warningMsg, __FILE__, __func__, __LINE__);
		return false;
	}

	*msg = SP_CONFIG_SUCCESS;
	return true;
}
