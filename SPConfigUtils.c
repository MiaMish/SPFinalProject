/*
 * SPUtils.c
 */


#include "SPConfigUtils.h"
#include "SPLogger.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ctype.h>


int convertFieldToNum(char* field) {
	if (strcmp(field, "spImagesDirectory") == 0)
		return 1;
	if (strcmp(field, "spImagesPrefix") == 0)
		return 2;
	if (strcmp(field, "spImagesSuffix") == 0)
		return 3;
	if (strcmp(field, "spNumOfImages") == 0)
		return 4;
	if (strcmp(field, "spPCADimension") == 0)
		return 5;
	if (strcmp(field, "spPCAFilename") == 0)
		return 6;
	if (strcmp(field, "spNumOfFeatures") == 0)
		return 7;
	if (strcmp(field, "spExtractionMode") == 0)
		return 8;
	if (strcmp(field, "spNumOfSimilarImages") == 0)
		return 9;
	if (strcmp(field, "spKDTreeSplitMethod") == 0)
		return 10;
	if (strcmp(field, "spKNN") == 0)
		return 11;
	if (strcmp(field, "spMinimalGUI") == 0)
		return 12;
	if (strcmp(field, "spLoggerLevel") == 0)
		return 13;
	if (strcmp(field, "spLoggerFilename") == 0)
		return 14;
	return -1;
}

int convertStringToNum(char str[]) {
	int i = 0;
	if (str[0] == '+') {
		i = 1;
	}
	while (str[i] != '\n' && str[i] != EOF && str[i] != '\0') {
		if (!isdigit(str[i])) {
			return -1;
		}
		i++;
	}
	if (i == 0) {
		return -1;
	}
	if (str[0] ==  '+') {
		// +num is allowed
		return atoi(&str[1]);
	}
	return atoi(str);
}

const char* convertMethodToString(SplitMethod method) {
	switch (method) {
	case 0:
		return "RANDOM";
	case 1:
		return "MAX_SPREAD";
	case 2:
		return "INCREMENTAL";
	case 3:
		return "UNDEFINED";
	}

	/*shouldn't get to this line */
	spLoggerPrintError("SplitMethod was altered, but convertMethodToString wasn't",
		__FILE__, __func__, __LINE__);
	return NULL;
}

const char* convertTypeToString(ImageType type) {
	switch (type) {
	case 0:
		return ".jpg";
	case 1:
		return ".png";
	case 2:
		return ".bmp";
	case 3:
		return ".gif";
	}

	/*shouldn't reach this line */
	/*shouldn't get to this line */
	spLoggerPrintError("ImageType was altered, but convertTypeToString wasn't",
		__FILE__, __func__, __LINE__);
	return NULL;
}

/*
 * return 0 - line is empty or a comment
 * return -1 if line format is not field = value
 * return convertFieldToNum(field) if line is in correct format
 */
int extractFieldAndValue(const char* line, char* value) {
	char field[MAX_SIZE] = {'\0'};
	int i = 0;
	int count = 0;
	int fieldId;

	while (line[i] == ' ') {
		i++;
	}

	//comment lines or empty lines are allowed
	if (line[i] == '#' || line[i] == '\n' || line[i] == EOF) {
		return 0;
	}

	//extracting the first string (field) from line
	while (line[i] != '\n' && line[i] != '\0' && line[i] != EOF &&
			line[i] != ' ' && line[i] != '=' && i < MAX_SIZE) {
		field[count] = line[i];
		count++;
		i++;
	}
	value[count] = '\0';

	fieldId = convertFieldToNum(field);
	if (fieldId == -1) {
		return -1;
	}

	//making sure the next character not ' ' is '='
	while (line[i] == ' ' && i < MAX_SIZE) {
		i++;
	}

	if (line[i] != '=') {
		return -1;
	}

	//extracting the second string from line
	count = 0;
	i++; //the previous character '=' isn't part of the value
	while (line[i] == ' ' && i < MAX_SIZE) {
		i++;
	}

	while (line[i] != '\n' && line[i] != EOF && line[i] != ' ' && line[i] != '=') {
		value[count] = line[i];
		count++;
		i++;
	}
	if(count == 0) {
		fieldId = -1;
	}
	value[count] = '\0';

	while (line[i] != '\n' && line[i] != '\0' && line[i] != EOF && i < MAX_SIZE) {
		if (line[i] != ' ') {
			return -1;
		}
		i++;
	}

	return fieldId;
}


void printError(const char* filename, int lineNumber, char* msg) {
	printf("File: %s\nLine: %d\nMessage: %s\n", filename, lineNumber, msg);
}
