/*
 * sp_config_utils_unit_tests.c
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

bool methodToStringTest() {
	char* str1 = "RANDOM";
	char* str2 = "MAX_SPREAD";
	char* str3 = "INCREMENTAL";

	const char* res1 = convertMethodToString(RANDOM);
	const char* res2 = convertMethodToString(MAX_SPREAD);
	const char* res3 = convertMethodToString(INCREMENTAL);

	ASSERT_TRUE(strcmp(str1, res1) == 0);
	ASSERT_TRUE(strcmp(str2, res2) == 0);
	ASSERT_TRUE(strcmp(str3, res3) == 0);

	return true;
}

bool typeToStringTest() {
	char* str1 = ".jpg";
	char* str2 = ".png";
	char* str3 = ".bmp";
	char* str4 = ".gif";

	const char* res1 = convertTypeToString(jpg);
	const char* res2 = convertTypeToString(png);
	const char* res3 = convertTypeToString(bmp);
	const char* res4 = convertTypeToString(gif);

	ASSERT_TRUE(strcmp(str1, res1) == 0);
	ASSERT_TRUE(strcmp(str2, res2) == 0);
	ASSERT_TRUE(strcmp(str3, res3) == 0);
	ASSERT_TRUE(strcmp(str4, res4) == 0);

	return true;
}

bool extractFieldAndValueTest() {
	char* filename = "./files_for_unit_tests/extractFieldAndValueTest.txt";
	FILE* file = fopen(filename, "r");
	char* line = (char*) malloc(sizeof(char) * 1024);
	char* value = (char*) malloc(sizeof(char) * 1024);
	int fieldId;
	int i;

	ASSERT_FALSE(file);
	ASSERT_FALSE(line);
	ASSERT_FALSE(value);

	int fieldIdRes[25] = {0, -1, -1, -1, -1, 0, 1, 3, 11, 0, 0, 12, 12, 2,
			3, 4, 5, 6, 7, 8, 9, 10, 13, 14};

	for(i = 0; i < 25; i++) {
		fgets(line, 1024, file);
		fieldId = extractFieldAndValue(line, value);
		ASSERT_TRUE(fieldId == fieldIdRes[i]);
	}

	free(line);
	free(value);
	fclose(file);
	return true;
}


int main_test() {
	RUN_TEST(stringToIntTest);
	RUN_TEST(fieldToNumTest);
	RUN_TEST(methodToStringTest);
	RUN_TEST(typeToStringTest);
	RUN_TEST(extractFieldAndValueTest);

	return 1;
}

