/*
 * SPConfigTests.c
 *
 *  Created on: 11 баев 2016
 *      Author: Ana
 */

#include "SPConfig.h"

#define MAX_SIZE 1024

void stringToIntTest() {
	char* str1 = "abd";
	char* str2 = "145";
	char* str3 = "+2";
	char* str4 = "1.0";
	char* str5 = "14d";
	char* str5 = "-156";
	char* str6 = "";
	char* str7 = "\n";
	int num1 = -1;
	int num2 = 145;
	int num3 = 2;
	int num4 = -1;
	int num5 = -156;
	int num6 = -1;
	int num7 = -1;

	ASSERT_TRUE(convertStringToNum(str1) == num1);
	ASSERT_TRUE(convertStringToNum(str1) == num2);
	ASSERT_TRUE(convertStringToNum(str1) == num3);
	ASSERT_TRUE(convertStringToNum(str1) == num4);
	ASSERT_TRUE(convertStringToNum(str1) == num5);
	ASSERT_TRUE(convertStringToNum(str1) == num6);
	ASSERT_TRUE(convertStringToNum(str1) == num7);
}

void fieldToNumTest() {
	char* str1 = "abd";
	char* str2 = "";
	char* str3 = "\t";
	char* str4 = "spImagesPrefix ";

	int num1 = -1;
	int num2 = -1;
	int num3 = -1;
	int num4 = 2;

	ASSERT_TRUE(convertFieldToNum(str1) == num1);
	ASSERT_TRUE(convertFieldToNum(str1) == num2);
	ASSERT_TRUE(convertFieldToNum(str1) == num3);
	ASSERT_TRUE(convertFieldToNum(str1) == num4);
}

void initConfigurationTest() {
	SPConfig config = NULL;
	struct AllSetFlag flags = NULL;
	config = (SPConfig*) malloc(sizeof(config*));
	flags = (struct AllSetFlag*) malloc (sizeof(flags*));
	initConfuguration(config, flags);

	ASSERT_TRUE(config->flag == flags);
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
}

void parseLineGeneralTest() {
	char* fileName = "ParseConfigLineGeneralTest.txt";
	SP_CONFIG_MSG expectedMsg[10] = {SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING,SP_CONFIG_INVALID_STRING,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS};

	parseLineTest(1, fileName, &expectedMsg, NULL);

}


void parseLineSuffixTest() {
	char* fileName = "ParseConfigLineSuffixTest.txt";
	SP_CONFIG_MSG expectedMsg[6] = {SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING};
	char expectedSuffix[6] = {NULL, ".gif", ".jpg", ".bmp", ".png", ".png"};

	parseLineTest(2, fileName, &expectedMsg, &expectedSuffix);
}

void parseLineModeTest() {
	char* fileName = "ParseConfigLineModeTest.txt";
	SP_CONFIG_MSG expectedMsg[3] = {SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING,
			SP_CONFIG_SUCCESS};
	bool expectedMode[3] = {true, true, false};

	parseLineTest(3, fileName, &expectedMsg, &expectedMode);
}

void parseLineGuiTest() {
	char* fileName = "ParseConfigLineGuiTest.txt";
	SP_CONFIG_MSG expectedMsg[4] = {SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING};
	bool expectedGui[4] = {false, true, false, false};

	parseLineTest(4, fileName, &expectedMsg, &expectedGui);
}

void parseLineMethodTest() {
	char* fileName = "ParseConfigLineMethodTest.txt";
	SP_CONFIG_MSG expectedMsg[5] = {SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING,
			SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS};
	Method expectedMethod[5] = {RANDOM, RANDOM, INCREMENTAL, MAX_SPREAD,
			MAX_SPREAD};

	parseLineTest(5, fileName, &expectedMsg, &expectedMethod);
}

void parseLineStringTest() {
	char* fileName = "ParseConfigLineBoolTest.txt";
	SP_CONFIG_MSG expectedMsg[8] = {SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS , SP_CONFIG_INVALID_STRING};
	char* expectedStr[8] = {"./somefolder", "baloon", "img", "img", "loggerName",
			"stdout", "stdout"};

	parseLineTest(6, fileName, &expectedMsg, &expectedStr);
}

void parseLineNumeralTest() {
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

	parseLineTest(7, fileName, &expectedMsg, &expectedNum);
}

void parseLineTest(int testType, char* fileName, SP_CONFIG_MSG* expectedMsg,
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
	}

	fclose(file);
	spConfigDestroy(config);
}

int main() {
	stringToIntTest();
	fieldToNumTest();
	return 1;
}


