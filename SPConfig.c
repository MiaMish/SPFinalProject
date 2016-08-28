#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>


#include "SPConfig.h"
#include "SPLogger.h"
#include "./unit_tests/unit_test_util.h"

/** default values for configuration **/
#define MAX_SIZE 1024
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

/** the options for the cut method when the kd-tree is build **/
typedef enum sp_methods {RANDOM, MAX_SPREAD, INCREMENTAL} Method;

/** the options for the image suffix **/
typedef enum imageTypes {jpg, png, bmp, gif} Types;

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
	Method spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	int spLoggerLevel;
	char* spLoggerFilename;
};

/*
 * @param config - pointer to an uninitialized SPConfig
 * puts default values in config
 */
void initConfuguration(SPConfig config);

/*
 * if str is not a decimal number, returns -1
 * else returns an integer representing str
 */
int convertStringToNum(char str[]);

/*
 * each field name is assigned an integer
 * if field isn't one of the fields of SPConfig than returns -1
 */
int convertFieldToNum(char* field);

/* @param method
 * @returns method as string
 */
char* convertMethodToString(Method method);

/* @param type
 * @returns type as string
 */
char* convertTypeToString(Types type);

/*
 * recieves a  string and tries to split it into field and value
 * @returns SP_CONFIG_SUCCESS if managed to update a field in config
 * @returns SP_CONFIG_INVALID_STRING if line is not in the correct format
 * @returns SP_CONFIG_INVALID_INTEGER if value is supposed to be an integer,
 * but is not one
 */
void parseConfigLine(char* line, SPConfig config, SP_CONFIG_MSG* msg);

/*
 * @assert msg != NULL
 * @returns true if config != NULL
 * and false otherwise
 */
bool getterAssert(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * @param filename - the name of the configuration file
 * @param lineNumber >= 0 - the line in the configuration file where error occured
 * @param msg - string with the massage to be printed
 */
void printError(char* filename, int lineNumber, char* msg);


SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg) {
	SPConfig config = NULL;
	FILE* file = NULL;
	SP_LOGGER_MSG logMsg = SP_LOGGER_SUCCESS;
	char line[MAX_SIZE];
	int lineCounter = 0;

	assert(msg != NULL);

	if (filename == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return config;
	}
	file = fopen(filename, "r");
	if (file == NULL) {
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return config;
	}

	config = (SPConfig*) malloc(sizeof(SPConfig));
	if(config == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		free(config);
		config = NULL;
		return config;
	}
	initConfiguration(config);

	if(*msg != SP_CONFIG_SUCCESS) {
		free(config);
		config = NULL;
		return config;
	}

	while(fgets(line, MAX_SIZE, file)) {
		lineCounter++;
		parseConfigLine(line, config, msg);
		if(*msg != SP_CONFIG_SUCCESS) {
			fclose(file);
			if(*msg == SP_CONFIG_INVALID_LINE) {
				printError(filename, lineCounter, invalidLine);
			} else if (*msg == SP_CONFIG_INVALID_INTEGER ||
					*msg == SP_CONFIG_INVALID_STRING ||
					*msg == SP_CONFIG_INVALID_BOOLEAN) {
				printError(filename, lineCounter, invalidValue);
			} else {
				printError(filename, lineCounter, "Some error I didn't think about");
			}

			spConfigDestroy(config);
			config = NULL;
			return config;
		}
	}

	fclose(file);

	if(config->spImagesDirectory == NULL) {
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
	if(logMsg == SP_LOGGER_OUT_OF_MEMORY) {
		spConfigDestroy(config);
		config = NULL;
		*msg = SP_CONFIG_ALLOC_FAIL;
	}
	if(logMsg == SP_LOGGER_CANNOT_OPEN_FILE) {
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
	if (!getterAssert(config, msg)) {
		return false;
	}
	return (config->spMinimalGUI == true);
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg)) {
		return -1;
	}
	return config->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg)) {
		return -1;
	}
	return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, msg)) {
		return -1;
	}
	return config->spPCADimension;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index) {

	int pathLength;
	if(imagePath == NULL || config == NULL) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	if(index >= config->spNumOfImages || index < 1) {
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}
	pathLength = sprintf(imagePath, "%s%s%d%s\n", config->spImagesDirectory,
			config->spImagesPrefix, index, config->spImagesSuffix);
	/*if (pathLength < 1) {
		return SOME_ERROR_MASSAGE
	} */
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
	int pathLength;
	if(pcaPath == NULL || config == NULL) {
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	pathLength = sprintf(pcaPath, "%s%s\n", config->spImagesDirectory,
			config->spPCAFilename);
	/*if (pathLength < 1) {
		return SOME_ERROR_MASSAGE
	} */
	return SP_CONFIG_SUCCESS;
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

void parseConfigLine(char* line, SPConfig config, SP_CONFIG_MSG* msg) {
	char field[MAX_SIZE];
	char value[MAX_SIZE];
	int i = 0;
	int count = 0;
	int fieldId;
	int valueAsNum;

	while(line[i] == ' ') i++;

	/*
	 * comment lines or empty lines are allowed
	 */
	if (line[i] == '#' || line[i] == '\n' || line[i] == EOF) {
		*msg = SP_CONFIG_SUCCESS;
		return;
	}

	/** extracting the first string from line **/
	while(line[i] != '\n' || line[i] != EOF || line[i] != ' ' || line[i] != '=') {
		field[count] = line[i];
		count++;
	}
	value[count] = '\0';

	fieldId = convertFieldToNum(field);
	if(line[i] != '=' || fieldId == -1) {
		*msg = SP_CONFIG_INVALID_LINE;
		return;
	}

	/** extracting the second string from line **/
	count = 0;
	i++; //the previous character '=' isn't part of the value
	while(line[i] == ' ') i++;
	while(line[i] != '\n' || line[i] != EOF || line[i] != ' ') {
		value[count] = line[i];
		count++;
	}
	value[count] = '\0';

	while(line[i] != '\n' || line[i] != EOF) {
		if(line[i] != ' ') {
			*msg = SP_CONFIG_INVALID_STRING;
			return;
		}
		value[count] = line[i];
		count++;
	}

	/*
	 * if field is of type int, first we'll make sure the value is positive
	 */
	valueAsNum = convertStringToNum(value);
	if (fieldId == 4 || fieldId == 5 || fieldId == 7 || fieldId == 9 ||
			fieldId == 11 || fieldId == 13) {
		if(valueAsNum < 0) {
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
		if(value[0] == '.') {
			for (i = 0; i < (sizeof(Types)/sizeof(Types*)); i++) {
				if (strcmp(value[1], convertTypeToString(Types[i])) == 0) {
					config->spImagesSuffix = Types[i];
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
		} else if(strcmp(value, "false") == 0) {
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
		for (i = 0; i < (sizeof(Method)/sizeof(Method*)); i++) {
			if (strcmp(value, convertMethodToString((Method[i]))) == 0) {
				config->spKDTreeSplitMethod = Method[i];
			}
		}
		if (strcmp(config->spKDTreeSplitMethod, value) == 0) {
			*msg = SP_CONFIG_INVALID_STRING;
			return;
		}
		break;

	case 11:
			config->spKNN = valueAsNum;
			break;

	case 12:
		if (strcmp(value, "true") == 0) {
			config->spMinimalGUI = true;
		} else if ( strcmp(value, "false") == 0) {
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

int convertFieldToNum(char* field) {
	if(strcmp(field, "spImagesDirectory") == 0) return 1;
	if(strcmp(field, "spImagesPrefix") == 0) return 2;
	if(strcmp(field, "spImagesSuffix") == 0) return 3;
	if(strcmp(field, "spNumOfImages") == 0) return 4;
	if(strcmp(field, "spPCADimension") == 0) return 5;
	if(strcmp(field, "spPCAFilename") == 0) return 6;
	if(strcmp(field, "spNumOfFeatures") == 0) return 7;
	if(strcmp(field, "spExtractionMode") == 0) return 8;
	if(strcmp(field, "spNumOfSimilarImages") == 0) return 9;
	if(strcmp(field, "spKDTreeSplitMethod") == 0) return 10;
	if(strcmp(field, "spKNN") == 0) return 11;
	if(strcmp(field, "spMinimalGUI") == 0) return 12;
	if(strcmp(field, "spLoggerLevel") == 0) return 13;
	if(strcmp(field, "spLoggerFilename") == 0) return 14;
	return -1;
}

int convertStringToNum(char str[]) {
	int i = 0;
	while(str[i] != '\n' || str[i] != EOF) {
		if (!isdigit(str[i])) {
			if (i != 0 || str[i] != '+') { //+int is allowed
				return -1;
			}
		}
		i++;
	}
	if(i == 0) {
		return -1;
	}
	return atoi(str);
}

char* convertMethodToString(Method method) {
	switch(method) {
	case 0:
		return "RANDOM";
	case 1:
		return "MAX_SPREAD";
	case 2:
		return "INCREMENTAL";
	}

	/*shouldn't get to this line */
	return NULL;
}

char* convertTypeToString(Types type) {
	switch(type) {
	case 0:
		return "jpg";
	case 1:
		return "png";
	case 2:
		return "bmp";
	case 3:
		return "gif";
	}

	/*shouldn't reach this line */
	return NULL;
}

bool getterAssert(const SPConfig config, SP_CONFIG_MSG* msg) {
	assert(msg != NULL);

	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}

	*msg = SP_CONFIG_SUCCESS;
	return true;
}

void printError(char* filename, int lineNumber, char* msg) {
	sprintf("File: %s\nLine: %d\nMessage: %s\n", filename, lineNumber, msg);
}

