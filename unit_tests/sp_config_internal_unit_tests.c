/*
 * sp_config_internal_unit_tests.c
 *
 * TESTERS FOR INTERNAL FUNCTIONS IN SPConfig.c
 * THEY WERE RUN ON A SEPARATE PROJECT
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "SPConfig.h"
#include "SPConfigUtils.h"
#include "SPLogger.h"
#include "unit_test_util.h"

bool initConfigurationTest() {
	SPConfig config;
	SP_CONFIG_MSG* msg;

	config = (SPConfig) malloc(sizeof(enum sp_config_msg_t));
	if (config == NULL) {
		printf("alloc error on line %d\n", __LINE__);
		return false;
	}
	msg = (SP_CONFIG_MSG*) malloc(sizeof(*msg));
	if (msg == NULL) {
		printf("alloc error on line %d\n", __LINE__);
		free(config);
		return false;
	}
	initConfiguration(config);
	ASSERT_FALSE(config == NULL);
	ASSERT_FALSE(msg == NULL);
	ASSERT_TRUE(strcmp(spConfigGetDirectory(config, msg), "") == 0);
	ASSERT_TRUE(strcmp(spConfigGetPrefix(config, msg), "") == 0);
	ASSERT_TRUE(strcmp(spConfigGetSuffix(config, msg), "") == 0);
	ASSERT_TRUE(spConfigGetNumOfImages(config, msg) == -1);
	ASSERT_TRUE(spConfigGetPCADim(config, msg) == 20);
	ASSERT_TRUE(strcmp(spConfigGetPCAFilename(config, msg), "pca.yml") == 0);
	ASSERT_TRUE(spConfigGetNumOfFeatures(config, msg) == 100);
	ASSERT_TRUE(spConfigIsExtractionMode(config, msg) == true);
	ASSERT_TRUE(spConfigGetNumOfSimIms(config, msg) == 1);
	ASSERT_TRUE(spConfigMinimalGui(config, msg) == false);
	ASSERT_TRUE(spConfigGetSpKNN(config, msg) == 1);
	ASSERT_TRUE(spConfigGetSplitMethod(config, msg) == MAX_SPREAD);
	ASSERT_TRUE(spConfigGetLogLevel(config, msg) == 3);
	ASSERT_TRUE(strcmp(spConfigGetLogName(config, msg), "stdout") == 0);
	return true;
}

bool parseLineGeneralTest() {
	FILE* file = NULL;
	SPConfig config = NULL;
	SP_CONFIG_MSG* msg = NULL;
	char* fileName = (char*) "./files_for_unit_tests.txt/ParseConfigLineGeneralTest.txt";
	SP_CONFIG_MSG expectedMsg[10] = {SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING,SP_CONFIG_INVALID_STRING,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS};

	char line[1024];
	int i = 0;
	file = fopen(fileName, "r");
	config = (SPConfig) malloc(sizeof(enum sp_config_msg_t));
	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	initConfiguration(config);
	while(fgets(line, 1024, file)) {
		parseConfigLine(line, config, msg);
		ASSERT_TRUE(*msg == expectedMsg[i]);
		i++;
	}
	fclose(file);
	spConfigDestroy(config);
	return true;
}

bool parseLineSuffixTest() {
	char* fileName = (char*) "./files_for_unit_tests.txt/ParseConfigLineSuffixTest.txt";
	SP_CONFIG_MSG expectedMsg[6] = {SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_STRING};
	char* expectedSuffix[6] = {NULL, ".gif", ".jpg", ".bmp", ".png", ".png"};

	FILE* file = NULL;
	SPConfig config = NULL;
	SP_CONFIG_MSG* msg = NULL;
	char line[1024];
	int i = 0;

	file = fopen(fileName, "r");
	config = (SPConfig) malloc(sizeof(enum sp_config_msg_t));
	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	initConfiguration(config);

	while(fgets(line, 1024, file)) {
		parseConfigLine(line, config, msg);
		ASSERT_TRUE(*msg == expectedMsg[i]);
		ASSERT_TRUE(strcmp(spConfigGetSuffix(config, msg), expectedSuffix[i]) == 0);
		i++;
	}

	fclose(file);
	spConfigDestroy(config);
	return true;
}

bool parseLineModeTest() {
	char* fileName = (char*) "./files_for_unit_tests.txt/ParseConfigLineModeTest.txt";
	SP_CONFIG_MSG expectedMsg[3] = {SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING,
			SP_CONFIG_SUCCESS};
	bool expectedMode[3] = {true, true, false};

	FILE* file = NULL;
	SPConfig config = NULL;
	SP_CONFIG_MSG* msg = NULL;
	char line[1024];
	int i = 0;

	file = fopen(fileName, "r");
	config = (SPConfig) malloc(sizeof(enum sp_config_msg_t));
	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	initConfiguration(config);

	while(fgets(line, 1024, file)) {
		parseConfigLine(line, config, msg);
		ASSERT_TRUE(*msg == expectedMsg[i]);
		ASSERT_TRUE(spConfigIsExtractionMode(config, msg) == expectedMode[i]);
		i++;
	}

	fclose(file);
	spConfigDestroy(config);
	return true;
}

bool parseLineGuiTest() {
	char* fileName = (char*) "./files_for_unit_tests.txt/ParseConfigLineGuiTest.txt";
	SP_CONFIG_MSG expectedMsg[4] = {SP_CONFIG_INVALID_BOOLEAN, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_BOOLEAN};
	bool expectedGui[4] = {false, true, false, false};

	FILE* file = NULL;
	SPConfig config = NULL;
	SP_CONFIG_MSG* msg = NULL;
	char line[1024];
	int i = 0;

	file = fopen(fileName, "r");
	config = (SPConfig) malloc(sizeof(enum sp_config_msg_t));
	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	initConfiguration(config);

	while(fgets(line, 1024, file)) {
		parseConfigLine(line, config, msg);
		ASSERT_TRUE(*msg == expectedMsg[i]);
		ASSERT_TRUE(spConfigMinimalGui(config, msg) == expectedGui[i]);
		i++;
	}


	fclose(file);
	spConfigDestroy(config);
	return true;
}

bool parseLineMethodTest() {
	char* fileName = (char*) "./files_for_unit_tests.txt/ParseConfigLineMethodTest.txt";
	SP_CONFIG_MSG expectedMsg[5] = {SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING,
			SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS};
	SplitMethod expectedMethod[5] = {RANDOM, RANDOM, INCREMENTAL, MAX_SPREAD,
			MAX_SPREAD};

	FILE* file = NULL;
	SPConfig config = NULL;
	SP_CONFIG_MSG* msg = NULL;
	char line[1024];
	int i = 0;

	file = fopen(fileName, "r");
	config = (SPConfig) malloc(sizeof(enum sp_config_msg_t));
	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	initConfiguration(config);

	while(fgets(line, 1024, file)) {
		parseConfigLine(line, config, msg);
		ASSERT_TRUE(*msg == expectedMsg[i]);
		ASSERT_TRUE(spConfigGetSplitMethod(config, msg) == expectedMethod[i]);
		i++;
	}

	fclose(file);
	spConfigDestroy(config);
	return true;
}

bool parseLineStringTest() {
	char* fileName = (char*) "./files_for_unit_tests.txt/ParseConfigLineBoolTest.txt";
	SP_CONFIG_MSG expectedMsg[7] = {SP_CONFIG_SUCCESS, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_STRING, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS , SP_CONFIG_INVALID_STRING};
	char* expectedStr[7] = {"./somefolder", "baloon", "img", "img", "loggerName",
			"stdout", "stdout"};

	FILE* file = NULL;
	SPConfig config = NULL;
	SP_CONFIG_MSG* msg = NULL;
	char line[1024];
	int i = 0;

	file = fopen(fileName, "r");
	config = (SPConfig) malloc(sizeof(enum sp_config_msg_t));
	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	initConfiguration(config);

	while(fgets(line, 1024, file)) {
		parseConfigLine(line, config, msg);
		ASSERT_TRUE(*msg == expectedMsg[i]);
		if(i == 0) {
			ASSERT_TRUE(strcmp(spConfigGetDirectory(config, msg), expectedStr[i]) == 0);
		} else if (i == 1 || i == 2) {
			ASSERT_TRUE(strcmp(spConfigGetPrefix(config, msg), expectedStr[i]) == 0);
		} else if (i == 3 || i == 4) {
			ASSERT_TRUE(strcmp(spConfigGetPCAFilename(config, msg), expectedStr[i]) == 0);
		} else {
			ASSERT_TRUE(strcmp(spConfigGetLogName(config, msg), expectedStr[i]) == 0);
		}

		i++;
	}

	fclose(file);
	spConfigDestroy(config);
	return true;
}

bool parseLineNumeralTest() {
	char* fileName = (char*) "./files_for_unit_tests.txt/ParseConfigLineNumeralTest.txt";
	SP_CONFIG_MSG expectedMsg[21] = {SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_SUCCESS,

			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,

			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_INTEGER,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER,

			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_INTEGER,

			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER,

			SP_CONFIG_SUCCESS, SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS,
			SP_CONFIG_INVALID_INTEGER, SP_CONFIG_INVALID_INTEGER, SP_CONFIG_SUCCESS};

	int expectedNum[21] = {-1, 5, 10,
			20, 20, 15,
			13, 13, 13, 13,
			4, 4,
			5, 5, 5,
			2, 2, 4, 4, 4, 1};

	FILE* file = NULL;
	SPConfig config = NULL;
	SP_CONFIG_MSG* msg = NULL;
	char line[1024];
	int i = 0;

	file = fopen(fileName, "r");
	config = (SPConfig) malloc(sizeof(enum sp_config_msg_t));
	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	initConfiguration(config);

	while(fgets(line, 1024, file)) {
		parseConfigLine(line, config, msg);
		ASSERT_TRUE(*msg == expectedMsg[i]);
		if(i == 0 || i == 1 || i == 2) {
			ASSERT_TRUE(spConfigGetNumOfImages(config, msg) == expectedNum[i]);
		} else if (i == 3 || i == 4 || i == 5) {
			ASSERT_TRUE(spConfigGetPCADim(config, msg) == expectedNum[i]);
		} else if (i == 6 || i == 7 || i == 8 || i == 9) {
			ASSERT_TRUE(spConfigGetNumOfFeatures(config, msg) == expectedNum[i]);
		} else if (i == 6 || i == 7 || i == 8 || i == 9) {
			ASSERT_TRUE(spConfigGetNumOfSimIms(config, msg) == expectedNum[i]);
		} else if (i == 6 || i == 7 || i == 8 || i == 9) {
			ASSERT_TRUE(spConfigGetSpKNN(config, msg) == expectedNum[i]);
		} else {
			ASSERT_TRUE(spConfigGetLogLevel(config, msg) == expectedNum[i]);
		}

		i++;
	}

	fclose(file);
	spConfigDestroy(config);
	return true;
}


