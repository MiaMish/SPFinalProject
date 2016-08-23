/*
 * SPUtils.h
 *
 *  Created on: 23 баев„ 2016
 *      Author: user
 */

#ifndef SPCONFIGUTILS_H_
#define SPCONFIGUTILS_H_

#include "SPConfig.h"

/** default values for configuration **/
#define MAX_SIZE 1024
#define spPCADimensionDefault 20
#define spPCAFilenameDefault "pca.yml"
#define spNumOfFeaturesDefault 100
#define spNumOfSimilarImagesDefault 1
#define spKNNDefault 1
#define spLoggerLevelDefault 3
#define spLoggerFilenameDefault "stdout"

/** the options for the cut method when the kd-tree is build **/
typedef enum sp_methods {
	RANDOM = 0, MAX_SPREAD = 1, INCREMENTAL = 2
} SplitMethod;

/** the options for the image suffix **/
typedef enum imageTypes {
	jpg, png, bmp, gif
} ImageType;

struct sp_config_t {
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
const char* convertMethodToString(SplitMethod method);

/* @param type
 * @returns type as string
 */
const char* convertTypeToString(ImageType type);

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
void printError(const char* filename, int lineNumber, char* msg);

#endif /* SPCONFIGUTILS_H_ */
