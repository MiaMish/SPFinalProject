/*
 * SPConfigTests.c
 *
 *  Created on: 11 баев 2016
 *      Author: Ana
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../SPConfig.h"
#include "../SPLogger.h"
#include "unit_test_util.h"

//#define MAX_SIZE 1024

/*
 * the options for the cut method when the kd-tree is build
 */
typedef enum sp_methods {RANDOM, MAX_SPREAD, INCREMENTAL} Method;


/*
 * the options for the image suffix
 */
typedef enum imageTypes {jpg, png, bmp, gif} Types;

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


