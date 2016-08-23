/*
 * SPConfigTests.c
 *
 *  Created on: 11 баев 2016
 *      Author: Ana
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../SPConfig.h"
#include "../SPConfigUtils.h"
#include "../SPLogger.h"
#include "unit_test_util.h"

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
	config = (SPConfig) malloc(sizeof(struct sp_config_t));
	initConfiguration(config);

	ASSERT_TRUE(config->spImagesDirectory == NULL);
	ASSERT_TRUE(config->spImagesPrefix == NULL);
	ASSERT_TRUE(config->spImagesSuffix == NULL);
	ASSERT_TRUE(config->spNumOfImages == -1);
	ASSERT_TRUE(config->spPCADimension == 20);
	ASSERT_TRUE(strcmp(config->spPCAFilename, "pca.yml") == 0);
	ASSERT_TRUE(config->spNumOfFeatures == 100);
	ASSERT_TRUE(config->spExtractionMode == true);
	ASSERT_TRUE(config->spNumOfSimilarImages == 1);
	ASSERT_TRUE(config->spKDTreeSplitMethod == false);
	ASSERT_TRUE(config->spKNN == 1);
	ASSERT_TRUE(config->spMinimalGUI == MAX_SPREAD);
	ASSERT_TRUE(config->spLoggerLevel == 3);
	ASSERT_TRUE(strcmp(config->spLoggerFilename, spLoggerFilenameDefault) == 0);
	return true;
}

bool parseLineTest(int testType, char* fileName, SP_CONFIG_MSG* expectedMsg, void* expectedValue) {
	FILE* file = NULL;
	SPConfig config = NULL;

	char** expectedSuffix;
	bool* expectedMode;
	bool* expectedGui;
	SplitMethod* expectedMethod;
	char** expectedStr;
	int* expectedNum;

	char line[MAX_SIZE];
	int i = 0;

	file = fopen(fileName, "r");
	config = (SPConfig) malloc(sizeof(struct sp_config_t));

	initConfiguration(config);

	while (fgets(line, MAX_SIZE, file)) {
		SP_CONFIG_MSG msg;
		parseConfigLine(line, config, &msg);
		ASSERT_TRUE(msg == expectedMsg[i]);

		switch (testType) {
		case 2:
			expectedSuffix = (char**) expectedValue;
			ASSERT_TRUE(strcmp(config->spImagesSuffix, expectedSuffix[i]) == 0);
			break;
		case 3:
			expectedMode = (bool*) expectedValue;
			ASSERT_TRUE(config->spExtractionMode == expectedMode[i]);
			break;
		case 4:
			expectedGui = (bool*) expectedValue;
			ASSERT_TRUE(config->spMinimalGUI == expectedGui[i]);
			break;
		case 5:
			expectedMethod = (SplitMethod*) expectedValue;
			ASSERT_TRUE(config->spKDTreeSplitMethod == expectedMethod[i]);
			break;
		case 6:
			expectedStr = (char**) expectedValue;
			if (i == 0) {
				ASSERT_TRUE(
						strcmp(config->spImagesDirectory, expectedStr[i]) == 0);
			} else if (i == 1 || i == 2) {
				ASSERT_TRUE(
						strcmp(config->spImagesPrefix, expectedStr[i]) == 0);
			} else if (i == 3 || i == 4) {
				ASSERT_TRUE(strcmp(config->spPCAFilename, expectedStr[i]) == 0);
			} else {
				ASSERT_TRUE(
						strcmp(config->spLoggerFilename, expectedStr[i]) == 0);
			}
			break;
		case 7:
			expectedNum = (int*) expectedValue;
			if (i == 0 || i == 1 || i == 2) {
				ASSERT_TRUE(config->spNumOfImages == expectedNum[i]);
			} else if (i == 3 || i == 4 || i == 5) {
				ASSERT_TRUE(config->spPCADimension == expectedNum[i]);
			} else if (i == 6 || i == 7 || i == 8 || i == 9) {
				ASSERT_TRUE(config->spNumOfFeatures == expectedNum[i]);
			} else if (i == 6 || i == 7 || i == 8 || i == 9) {
				ASSERT_TRUE(config->spNumOfSimilarImages == expectedNum[i]);
			} else if (i == 6 || i == 7 || i == 8 || i == 9) {
				ASSERT_TRUE(config->spKNN == expectedNum[i]);
			} else {
				ASSERT_TRUE(config->spLoggerLevel == expectedNum[i]);
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
	SP_CONFIG_MSG expectedMsg[] = { SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_INVALID_STRING,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_INVALID_STRING,
			SP_CONFIG_SUCCESS };

	return parseLineTest(1, fileName, expectedMsg, NULL);
}

bool parseLineSuffixTest() {
	char* fileName = "ParseConfigLineSuffixTest.txt";
	SP_CONFIG_MSG expectedMsg[] = { SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING };
	char* expectedSuffix[] = { NULL, ".gif", ".jpg", ".bmp", ".png", ".png" };

	return parseLineTest(2, fileName, expectedMsg, expectedSuffix);
}

bool parseLineModeTest() {
	char* fileName = "ParseConfigLineModeTest.txt";
	SP_CONFIG_MSG expectedMsg[] = { SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING,
			SP_CONFIG_SUCCESS };
	bool expectedMode[] = { true, true, false };

	return parseLineTest(3, fileName, expectedMsg, expectedMode);
}

bool parseLineGuiTest() {
	char* fileName = "ParseConfigLineGuiTest.txt";
	SP_CONFIG_MSG expectedMsg[] = { SP_CONFIG_INVALID_BOOLEAN, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_BOOLEAN };
	bool expectedGui[] = { false, true, false, false };

	return parseLineTest(4, fileName, expectedMsg, expectedGui);
}

bool parseLineMethodTest() {
	char* fileName = "ParseConfigLineMethodTest.txt";
	SP_CONFIG_MSG expectedMsg[] = { SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING,
			SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS };
	SplitMethod expectedMethod[] = { RANDOM, RANDOM, INCREMENTAL, MAX_SPREAD,
			MAX_SPREAD };

	return parseLineTest(5, fileName, expectedMsg, expectedMethod);
}

bool parseLineStringTest() {
	char* fileName = "ParseConfigLineBoolTest.txt";
	SP_CONFIG_MSG expectedMsg[] = { SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING };
	char* expectedStr[] = { "./somefolder", "baloon", "img", "img", "loggerName",
			"stdout", "stdout" };

	return parseLineTest(6, fileName, expectedMsg, expectedStr);
}

bool parseLineNumeralTest() {
	char* fileName = "ParseConfigLineNumeralTest.txt";
	SP_CONFIG_MSG expectedMsg[] = { SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_INTEGER,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER,
			SP_CONFIG_SUCCESS };
	int expectedNum[] = { 0, 5, 10, /*"img", "loggerName",*/ 20, 20, 15, 13, 13,
			13, 13, 4, 4, 2, 2, 4, 4, 4, 1 };

	return parseLineTest(7, fileName, expectedMsg, expectedNum);
}

int main_test() {
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

