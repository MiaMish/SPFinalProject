#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>

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

/** the options for the cut method when the kd-tree is build **/
typedef enum sp_methods {RANDOM, MAX_SPREAD, INCREMENTAL} Method;

/** the options for the image suffix **/
typedef enum imageTypes {jpg, png, bmp, gif} Types;

/*
 * bit fields where
 * field == 1 if the field was initialized
 * field == 0 otherwise
 */
typedef struct AllSetFlag {
	unsigned int is_spImagesDirectory_set : 1;
	unsigned int is_spImagesPrefix_set : 1;
	unsigned int is_spImagesSuffix_set : 1;
	unsigned int is_spNumOfImages_set : 1;
} *allSet;

typedef struct sp_config_t{
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

	allSet flag;
} *SPConfig;

/*
 * @param config - pointer to an uninitialized SPConfig
 * @param flags - pointer to an uninitialized allSet
 * puts default values in config and flags and
 * sets config->flag = flags
 */
void initConfuguration(SPConfig* config, SP_CONFIG_MSG* msg);

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
	allSet flags = NULL;
	FILE* file = NULL;
	SP_LOGGER_MSG logMsg = SP_LOGGER_SUCCESS;
	char field[MAX_SIZE];
	char value[MAX_SIZE];
	char line[MAX_SIZE];
	char ch;
	int counter;
	int lineCounter = 0;

	asser(msg != NULL);

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
	flags = (allSet*) malloc(sizeof(flags));
	if(config == NULL) {
		*msg = SP_CONFIG_ALLOC_FAIL;
		free(config);
		config = NULL;
		return config;
	}
	initConfiguration(config, msg);

	if(msg != SP_CONFIG_SUCCESS) {
		free(config);
		config = NULL;
		return config;
	}

	flags = config->flag;

	while(fgets(line, MAX_SIZE, file)) {
		lineCounter++;
		parseConfigLine(line, config, msg);
		if(msg != SP_CONFIG_SUCCESS) {
			fclose(file);
			if(msg == SP_CONFIG_INVALID_LINE) {
				printError(config, lineCounter, invalidLine);
			} else if (msg == SP_CONFIG_INVALID_INTEGER ||
					msg == SP_CONFIG_INVALID_STRING ||
					msg == SP_CONFIG_INVALID_BOOLEAN) {
				printError(config, lineCounter, invalidValue);
			} else {
				printError(config, lineCounter, "Some error I didn't think about");
			}

			spConfigDestroy(config);
			config = NULL;
			return config;
		}
	}

	fclose(file);

	if(flags->is_spImagesDirectory_set == 0) {
		*msg = SP_CONFIG_MISSING_DIR;
		printError(config, lineCounter, directoryNotSet);
	} else if (flags->is_spImagesPrefix_set == 0) {
		*msg = SP_CONFIG_MISSING_PREFIX;
		printError(config, lineCounter, preffixNotSet);
	} else if (flags->is_spImagesSuffix_set == 0) {
		*msg = SP_CONFIG_MISSING_SUFFIX;
		printError(config, lineCounter, suffixNotSet);
	} else if (flags->is_spNumOfImages_set == 0) {
		*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		printError(config, lineCounter, imageNumNotSet);
	} else {
		*msg = SP_CONFIG_SUCCESS;
	}

	if (msg != SP_CONFIG_SUCCESS) {
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
	if (!getterAssert(config, &msg)) {
		return false;
	}
	return (config->spExtractionMode == true);
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, &msg)) {
		return false;
	}
	return (config->spMinimalGUI == true);
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, &msg)) {
		return -1;
	}
	return config->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, &msg)) {
		return -1;
	}
	return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg) {
	if (!getterAssert(config, &msg)) {
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
		if (config->flag != NULL) {
			free(config->flag);
			config->flag = NULL;
		}
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
	allSet flags = config->flag;
	int i = 0;
	int count = 0;
	int fieldId;
	int valueAsNum;

	while(line[i] == ' ') i++;

	/*
	 * comment lines or empty lines are allowed
	 */
	if (line[i] == '#' || line[i] == '\n' ||
			line[i] == '\r\n' || line[i] == EOF) {
		*msg = SP_CONFIG_SUCCESS;
		return;
	}

	/** extracting the first string from line **/
	while(line[i] != '\n' || line[i] != '\r\n' || line[i] != EOF ||
			line[i] != ' ' || line[i] != '=') {
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
	while(line[i] != '\n' || line[i] != '\r\n' ||
			line[i] != EOF || line[i] != ' ') {

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
		flags->is_spImagesDirectory_set = 1;
		break;

	case 2:
		config->spImagesPrefix = value;
		flags->is_spImagesPrefix_set = 1;
		break;

	case 3:
		if(value[0] == '.') {
			for (i = 0; i < (sizeof(Types)/sizeof(Types*)); i++) {
				if (strcmp(value[1], Types[i]) == 0) {
					config->spImagesSuffix = value;
					flags->is_spImagesSuffix_set = 1;
				}
			}
		} else {
			*msg = SP_CONFIG_INVALID_STRING;
			return;
		}
		break;

	case 4:
		config->spNumOfImages = valueAsNum;
		flags->is_spNumOfImages_set = 1;
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
		if (strcmp(value, "true") == 0 || strcmp(value, "false") == 0) {
			config->spExtractionMode = value;
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
			if (strcmp(value, Method[i]) == 0) {
				config->spKDTreeSplitMethod = value;
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
		if (strcmp(value, "true") == 0 || strcmp(value, "false") == 0) {
			config->spMinimalGUI = value;
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

void initConfuguration(SPConfig config, SP_CONFIG_MSG* msg) {
	allSet flags = (allSet*) malloc(sizeof(allSet));
	if (flags == NULL) {
		msg = SP_CONFIG_ALLOC_FAIL;
		return;
	}
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

	flags->is_spImagesDirectory_set = 0;
	flags->is_spImagesPrefix_set = 0;
	flags->is_spImagesSuffix_set = 0;
	flags->is_spNumOfImages_set = 0;

	config->flag = flags;
	*msg = SP_CONFIG_SUCCESS;
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


/**
 *
 * TESTERS FOR INTERNAL FUNCTIONS
 *
 */

bool stringToIntTest() {
	char* str1 = "abd";
	char* str2 = "145";
	char* str3 = "+2";
	char* str4 = "1.0";
	char* str5 = "14d";
	char* str6 = "-156";
	char* str7 = "";
	char* str8 = "\n";
	int num1 = -1;
	int num2 = 145;
	int num3 = 2;
	int num4 = -1;
	int num5 = -156;
	int num6 = -1;
	int num7 = -1;
	int num8 = -1;

	ASSERT_TRUE(convertStringToNum(str1) == num1);
	ASSERT_TRUE(convertStringToNum(str2) == num2);
	ASSERT_TRUE(convertStringToNum(str3) == num3);
	ASSERT_TRUE(convertStringToNum(str4) == num4);
	ASSERT_TRUE(convertStringToNum(str5) == num5);
	ASSERT_TRUE(convertStringToNum(str6) == num6);
	ASSERT_TRUE(convertStringToNum(str7) == num7);
	ASSERT_TRUE(convertStringToNum(str8) == num8);
	return true;
}

bool fieldToNumTest() {
	char* str1 = "abd";
	char* str2 = "";
	char* str3 = "\t";
	char* str4 = "spImagesPrefix ";

	int num1 = -1;
	int num2 = -1;
	int num3 = -1;
	int num4 = 2;

	ASSERT_TRUE(convertFieldToNum(str1) == num1);
	ASSERT_TRUE(convertFieldToNum(str2) == num2);
	ASSERT_TRUE(convertFieldToNum(str3) == num3);
	ASSERT_TRUE(convertFieldToNum(str4) == num4);
	return true;
}

bool initConfigurationTest() {
	SPConfig config = NULL;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	config = (SPConfig*) malloc(sizeof(config*));
	initConfuguration(config, msg);

	ASSERT_TRUE(config->flag->is_spImagesDirectory_set == 0);
	ASSERT_TRUE(config->flag->is_spImagesPrefix_set == 0);
	ASSERT_TRUE(config->flag->is_spImagesSuffix_set == 0);
	ASSERT_TRUE(config->flag->is_spNumOfImages_set == 0);

	ASSERT_TRUE(config->spImagesDirectory == NULL);
	ASSERT_TRUE(config->spImagesPrefix == NULL);
	ASSERT_TRUE(config->spImagesSuffix == NULL);
	ASSERT_TRUE(config->spNumOfImages == NULL);
	ASSERT_TRUE(config->spPCADimension == 20);
	ASSERT_TRUE(config->spPCAFilename == "pca.yml");
	ASSERT_TRUE(config->spNumOfFeatures == 100);
	ASSERT_TRUE(config->spExtractionMode == true);
	ASSERT_TRUE(config->spNumOfSimilarImages == 1);
	ASSERT_TRUE(config->spKDTreeSplitMethod == false);
	ASSERT_TRUE(config->spKNN == 1);
	//ASSERT_TRUE(config->spMinimalGUI == enum methods[1]);
	ASSERT_TRUE(config->spLoggerLevel == 3);
	ASSERT_TRUE(config->spLoggerFilename == stdout);
	return true;
}

bool parseLineTest(int testType, char* fileName, SP_CONFIG_MSG* expectedMsg,
		void* expectedValue) {
	FILE* file = NULL;
	SPConfig config = NULL;
	SP_CONFIG_MSG msg = NULL;
	char expectedSuffix[6] = NULL;
	bool expectedMode[3] = NULL;
	bool expectedGui[4] = NULL;
	Method expectedMethod[5] = NULL;
	char* expectedStr[8] = NULL;
	int* expectedNum[21] = NULL;

	char line[MAX_SIZE];
	int i = 0;

	file = fopen(fileName, 'r');
	config = (SPConfig*) malloc(sizeof(SPConfig));

	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	initConfuguration(config, msg);

	while(fgets(line, MAX_SIZE, file)) {
		parseConfigLine(line, config, msg);
		ASSERT_TRUE(strcmp(msg, expectedMsg[i]) == 0);
		switch (testType) {
		case 2:
			expectedSuffix = *(char*) expectedValue;
			ASSERT_TRUE(strcmp(config->spImagesSuffix, expectedSuffix[i]) == 0);
			break;
		case 3:
			expectedMode = *(bool*) expectedValue;
			ASSERT_TRUE(config->spExtractionMode == expectedMode[i]);
			break;
		case 4:
			expectedGui = *(bool*) expectedValue;
			ASSERT_TRUE(config->spMinimalGUI == expectedMode[i]);
			break;
		case 5:
			expectedMethod = *(Method)  expectedValue;
			ASSERT_TRUE(strcmp(config->spKDTreeSplitMethod, expectedMethod[i]) == 0);
			break;
		case 6:
			expectedStr = *(char*) expectedValue;
			if(i == 0) {
				ASSERT_TRUE(strcmp(config->spImagesDirectory, expectedStr[i]) == 0);
				ASSERT_TRUE(config->flag->is_spImagesDirectory_set == 1);
			} else if (i == 1 || i == 2) {
				ASSERT_TRUE(strcmp(config->spImagesPrefix, expectedStr[i]) == 0);
			} else if (i == 3 || i == 4) {
				ASSERT_TRUE(strcmp(config->spPCAFilename, expectedStr[i]) == 0);
			} else {
				ASSERT_TRUE(strcmp(config->spLoggerFilename, expectedStr[i]) == 0);
			}
			break;
		case 7:
			expectedNum = *(int*) expectedValue;
			if(i == 0 || i == 1 || i == 2) {
				ASSERT_TRUE(config->spNumOfImages == expectedValue[i]);
			} else if (i == 3 || i == 4 || i == 5) {
				ASSERT_TRUE(config->spPCADimension == expectedValue[i]);
			} else if (i == 6 || i == 7 || i == 8 || i == 9) {
				ASSERT_TRUE(config->spNumOfFeatures == expectedValue[i]);
			} else if (i == 6 || i == 7 || i == 8 || i == 9) {
				ASSERT_TRUE(config->spNumOfSimilarImages == expectedValue[i]);
			} else if (i == 6 || i == 7 || i == 8 || i == 9) {
				ASSERT_TRUE(config->spKNN == expectedValue[i]);
			} else {
				ASSERT_TRUE(config->spLoggerLevel == expectedValue[i]);
			}
			break;
		}
		i++;
		return true;
	}


	fclose(file);
	spConfigDestroy(config);
	return true;
}

bool parseLineGeneralTest() {
	char* fileName = "ParseConfigLineGeneralTest.txt";
	SP_CONFIG_MSG expectedMsg[10] = {SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING,SP_CONFIG_INVALID_STRING,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS};

	return parseLineTest(1, fileName, &expectedMsg, NULL);
}

bool parseLineSuffixTest() {
	char* fileName = "ParseConfigLineSuffixTest.txt";
	SP_CONFIG_MSG expectedMsg[6] = {SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING};
	char expectedSuffix[6] = {NULL, ".gif", ".jpg", ".bmp", ".png", ".png"};

	return parseLineTest(2, fileName, &expectedMsg, &expectedSuffix);
}

bool parseLineModeTest() {
	char* fileName = "ParseConfigLineModeTest.txt";
	SP_CONFIG_MSG expectedMsg[3] = {SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING,
			SP_CONFIG_SUCCESS};
	bool expectedMode[3] = {true, true, false};

	return parseLineTest(3, fileName, &expectedMsg, &expectedMode);
}

bool parseLineGuiTest() {
	char* fileName = "ParseConfigLineGuiTest.txt";
	SP_CONFIG_MSG expectedMsg[4] = {SP_CONFIG_INVALID_BOOLEAN, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_BOOLEAN};
	bool expectedGui[4] = {false, true, false, false};

	return parseLineTest(4, fileName, &expectedMsg, &expectedGui);
}

bool parseLineMethodTest() {
	char* fileName = "ParseConfigLineMethodTest.txt";
	SP_CONFIG_MSG expectedMsg[5] = {SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING,
			SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS};
	Method expectedMethod[5] = {RANDOM, RANDOM, INCREMENTAL, MAX_SPREAD,
			MAX_SPREAD};

	return parseLineTest(5, fileName, &expectedMsg, &expectedMethod);
}

bool parseLineStringTest() {
	char* fileName = "ParseConfigLineBoolTest.txt";
	SP_CONFIG_MSG expectedMsg[8] = {SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS , SP_CONFIG_INVALID_STRING};
	char* expectedStr[8] = {"./somefolder", "baloon", "img", "img", "loggerName",
			"stdout", "stdout"};

	return parseLineTest(6, fileName, &expectedMsg, &expectedStr);
}

bool parseLineNumeralTest() {
	char* fileName = "ParseConfigLineBoolTest.txt";
	SP_CONFIG_MSG expectedMsg[21] = {SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER,
			SP_CONFIG_SUCCESS , SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS};
	int* expectedNum[21] = {NULL, 5, 10, "img", "loggerName", 20, 20, 15, 13, 13,
	13, 13, 4, 4, 2, 2, 4, 4, 4, 1};

	return parseLineTest(7, fileName, &expectedMsg, &expectedNum);
}


int main() {
	RUN_TEST(stringToIntTest);
	RUN_TEST(fieldToNumTest);
	RUN_TEST(initConfigurationTest);
	RUN_TEST(parseLineGeneralTest);
	RUN_TEST(parseLineSuffixTest);
	RUN_TEST(parseLineModeTest);
	RUN_TEST(parseLineStringTest);
	RUN_TEST(parseLineNumeralTest);

	return 1;
}
