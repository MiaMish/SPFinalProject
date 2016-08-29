#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "SPConfig.h"
#include "SPConfigUtils.h"
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

/**the range of spPCADimension **/
#define PCADimUpperBound 28
#define PCADimLowerBound 10

/**Error massages to be printed in case configuration create failed **/
#define invalidLine "Invalid configuration line"
#define invalidValue "Invalid value - constraint not met"
#define directoryNotSet "Parameter spImagesDirectory is not set"
#define preffixNotSet "Parameter spImagesPrefix is not set"
#define suffixNotSet "Parameter spImagesSuffix is not set"
#define imageNumNotSet "Parameter spNumOfImages is not set"

/** *Error massages reported to logger**/
#define configIsNull "config is NULL"
#define msgIsNull "msg is NULL"
#define argIsNull "one of the arguments is NULL"

struct sp_config_t{
	char* spImagesDirectory;
	char* spImagesPrefix;
	char* spImagesSuffix;
	int spNumOfImages;
	int spPCADimension;
	char* spPCAFilename;
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SplitMethod spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
	char* spLoggerFilename;
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
SP_CONFIG_MSG createFilePath(char* imagePath, const SPConfig config,
		int index, char* suffix);

/*
 * @assert msg != NULL
 * @returns true if config != NULL
 * and false otherwise
 */
bool getterAssert(const SPConfig config, SP_CONFIG_MSG* msg);


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	SPConfig config = NULL;
	FILE* file = NULL;
	SP_LOGGER_MSG logMsg = NULL;
	char line[MAX_SIZE];
	int lineCounter = 0;

	assert(msg != NULL);

	if (filename == NULL) {
		msg = SP_CONFIG_INVALID_ARGUMENT;
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

	if (*msg != SP_CONFIG_SUCCESS) {
		free(config);
		config = NULL;
		return config;
	}

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
	}

	fclose(file);

	if (config->spImagesDirectory == NULL) {
		*msg = SP_CONFIG_MISSING_DIR;
		printError(filename, lineCounter, directoryNotSet);
	} else if (config->spImagesPrefix == NULL) {
		*msg = SP_CONFIG_MISSING_PREFIX;
		printError(filename, lineCounter, preffixNotSet);
	} else if (config->spImagesSuffix == NULL) {
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

	logMsg = spLoggerCreate(config->spLoggerFilename, config->spLoggerLevel);
	if (logMsg == SP_LOGGER_OUT_OF_MEMORY) {
		spConfigDestroy(config);
		config = NULL;
		*msg = SP_CONFIG_ALLOC_FAIL;
	}
	if (logMsg == SP_LOGGER_CANNOT_OPEN_FILE) {
		spConfigDestroy(config);
		config = NULL;
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
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

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index) {

	return createFilePath(imagePath, config, index,
			config->spImagesSuffix, __func__);
}

SP_CONFIG_MSG spConfigGetImageFeatsPath(char* imagePath, const SPConfig config,
		int index) {

	return createFilePath(imagePath, config, index, spFeatsSuffix, __func__);
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
	int pathLength;
	char* errMsg;

	if (pcaPath == NULL || config == NULL) {
		errMsg = sprintf("The function was called with an invalid argument");
		spLoggerPrintError(errMsg, __FILE__, __func__, __LINE__);
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	realloc(pcaPath, sizeof(*config->spImagesDirectory)
			+ sizeof(*config->spPCAFilename));

	pathLength = sprintf(pcaPath, "%s%s\n", config->spImagesDirectory,
			config->spPCAFilename);
	if (pathLength < 1) {
		errMsg = sprintf("sprintf function has failed");
		spLoggerPrintError(errMsg, __FILE__, __func__, __LINE__);
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

void spConfigDestroy(SPConfig config) {
	if (config != NULL) {
		if (logger != NULL) {
			spLoggerDestroy();
		}
		free(config);
		config = NULL;
	}
}


/*
 * Helper functions
 */

void parseConfigLine(char* line, SPConfig config, SP_CONFIG_MSG* msg) {
	char field[MAX_SIZE];
	char value[MAX_SIZE];
	int i = 0;
	int count = 0;
	int fieldId;
	int valueAsNum;

	fieldId = findFieldAndValue(line, config, msg, &field, &value);
	if (fieldId <= 0) {
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
		config->spImagesDirectory = value;
		break;

	case 2:
		config->spImagesPrefix = value;
		break;

	case 3:
		if (value[0] == '.') {
			for (i = 0; i < (sizeof(ImageType) / sizeof(*ImageType)); i++) {
				const char* typeString = convertTypeToString((ImageType) i);
				if (strcmp(value, typeString) == 0) {
					config->spImagesSuffix = (char*) typeString;
				}
			}
		} else {
			*msg = SP_CONFIG_INVALID_STRING;
			return;
		}
		break;

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
		config->spPCAFilename = value;
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
		for (i = 0; i < (sizeof(SplitMethod) / sizeof(SplitMethod*)); i++) {
			if (strcmp(value, convertMethodToString(((SplitMethod) i))) == 0) {
				config->spKDTreeSplitMethod = (SplitMethod) i;
			} else {
				*msg = SP_CONFIG_INVALID_STRING;
				return;
			}
		}
		break;

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
		config->spLoggerFilename = value;
		break;

	default:
		*msg = SP_CONFIG_INVALID_LINE;
		return;
	}

	*msg = SP_CONFIG_SUCCESS;
}


void initConfuguration(SPConfig config) {
	config->spPCADimension = spPCADimensionDefault;
	config->spPCAFilename = spPCAFilenameDefault;
	config->spNumOfFeatures = spNumOfFeaturesDefault;
	config->spExtractionMode = true;
	config->spNumOfSimilarImages = spNumOfSimilarImagesDefault;
	config->spKDTreeSplitMethod = spKDTreeSplitMethodDefault;
	config->spKNN = spKNNDefault;
	config->spMinimalGUI = false;
	config->spLoggerLevel = spLoggerLevelDefault;
	config->spLoggerFilename = spLoggerFilenameDefault;

	config->spImagesDirectory = NULL;
	config->spImagesPrefix = NULL;
	config->spImagesSuffix = NULL;
	config->spNumOfImages = -1;
}

SP_CONFIG_MSG createFilePath(char* imagePath, const SPConfig config,
		int index, char* suffix, char* func) {

	int pathLen;
	char* errMsg;

	if (imagePath == NULL || config == NULL) {
		errMsg = sprintf("The function %s was called with an invalid argument",
			func);
		spLoggerPrintError(errMsg, __FILE__, __func__, __LINE__);
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	if (index >= config->spNumOfImages || index < 1) {
		errMsg = sprintf("The index given to %s is out of range",
			func);
		spLoggerPrintError(errMsg, __FILE__, __func__, __LINE__);
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}

	realloc(imagePath, sizeof(*config->spImagesDirectory)
			+ sizeof(*config->spImagesPrefix) + sizeof(*index) + sizeof(suffix));

	pathLen = sprintf(imagePath, "%s%s%d%s\n", config->spImagesDirectory,
			config->spImagesPrefix, index, suffix);
	if (pathLen < 1) {
		errMsg = sprintf("sprintf function has failed");
		spLoggerPrintError(errMsg, __FILE__, __func__, __LINE__);
		return SP_CONFIG_UNKNOWN_ERROR;
	}

	return SP_CONFIG_SUCCESS;
}

bool getterAssert(const SPConfig config, SP_CONFIG_MSG* msg, char* func) {
	char* errMsg = sprintf("The function %s was called with an invalid argument",
			func);

	assert(msg != NULL);

	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		spLoggerPrintError(errMsg, __FILE__, __func__, __LINE__);
		return false;
	}

	*msg = SP_CONFIG_SUCCESS;
	return true;
}
