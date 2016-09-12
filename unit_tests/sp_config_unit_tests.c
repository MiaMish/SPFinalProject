/*
 * sp_config_unit_tests.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../SPConfig.h"
#include "../SPConfigUtils.h"
#include "../SPLogger.h"
#include "unit_test_util.h"
#include "unit_tests.h"

/*
 * basic tester for spConfigCreate based on example in instruction file
 */
bool spConfigBasicTest1() {
	SPConfig config;
	SP_CONFIG_MSG msg;
	const char* configFilename = "./files_for_unit_tests/configExample1.txt";

	const char* expDir = "./images/";
	const char* expPrefix = "img";
	const char* expSuffix = ".png";
	int expNumOfIm = 17;
	int expPCADim = 20;
	const char* expPCAFilename = "pca.yml";
	int expNumOfFeatures = 100;
	bool expExtrMode = true;
	bool expMinimalGui = false;
	int expNumOfSimIm = 5;
	const char* expLoggerFilename = "stdout";
	int expLoggerLevel = 3;
	SplitMethod expMethod = MAX_SPREAD;

	config = spConfigCreate(configFilename, &msg);
	ASSERT_NOT_NULL(config);

	ASSERT_TRUE(spConfigIsExtractionMode(config, &msg) == expExtrMode);
	ASSERT_TRUE(spConfigMinimalGui(config, &msg) == expMinimalGui);

	ASSERT_TRUE(spConfigGetNumOfImages(config, &msg) == expNumOfIm);
	ASSERT_TRUE(spConfigGetPCADim(config, &msg) == expPCADim);
	ASSERT_TRUE(spConfigGetNumOfFeatures(config, &msg) == expNumOfFeatures);
	ASSERT_TRUE(spConfigGetNumOfSimIms(config, &msg) == expNumOfSimIm);
	ASSERT_TRUE(spConfigGetLogLevel(config, &msg) == expLoggerLevel);

	ASSERT_TRUE(strcmp(spConfigGetDirectory(config, &msg), expDir) == 0);
	ASSERT_TRUE(strcmp(spConfigGetPrefix(config, &msg), expPrefix) == 0);
	ASSERT_TRUE(strcmp(spConfigGetSuffix(config, &msg), expSuffix) == 0);
	ASSERT_TRUE(strcmp(spConfigGetPCAFilename(config, &msg), expPCAFilename) == 0);
	ASSERT_TRUE(strcmp(spConfigGetLogName(config, &msg), expLoggerFilename) == 0);

	ASSERT_TRUE(spConfigGetSplitMethod(config, &msg) == expMethod);

	spConfigDestroy(config);

	return true;
}

/*
 * basic tester for various getters called before config was created
 * @return true if after each call for some getter, *msg == SP_CONFIG_INVALID_ARGUMENT
 * and each function returns value signifying the argument was invalid
 * @return false otherwise
 */
bool spConfigUninitialized() {
	SPConfig config = NULL;
	SP_CONFIG_MSG* msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));

	ASSERT_FALSE(spConfigIsExtractionMode(config, msg));
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_FALSE(spConfigMinimalGui(config, msg));
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_FALSE(spConfigGetNumOfImages(config, msg) >= 0);
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_FALSE(spConfigGetNumOfFeatures(config, msg) >= 0);
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_FALSE(spConfigGetPCADim(config, msg) >= 0);
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_FALSE(spConfigGetNumOfSimIms(config, msg) >= 0);
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_NULL(spConfigGetLogName(config, msg));
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_TRUE(spConfigGetLogLevel(config, msg) == -1);
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_TRUE(spConfigGetSpKNN(config, msg) == -1);
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_NULL(spConfigGetDirectory(config, msg));
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_NULL(spConfigGetPrefix(config, msg));
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_NULL(spConfigGetSuffix(config, msg));
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	*msg = SP_CONFIG_SUCCESS;
	ASSERT_NULL(spConfigGetPCAFilename(config, msg));
	ASSERT_TRUE(*msg == SP_CONFIG_INVALID_ARGUMENT);

	free(msg);
	return true;
}

/*
 * if config != NULL and imagePath == NULL and index in range , then
 * spConfigGetImagePath == spConfigGetImageFeatsPath == spConfigGetPCAPath ==
 * SP_CONFIG_INVALID_ARGUMENT
 * @return true if condition is met
 * @return false otherwise
 */
bool getPathNullImagepathTest() {
	SPConfig config;
	SP_CONFIG_MSG msg;
	char* imagePath = NULL;
	const char* configFilename = "./files_for_unit_tests/configExample1.txt";
	config = spConfigCreate(configFilename, &msg);
	ASSERT_NOT_NULL(config);

	ASSERT_TRUE(spConfigGetImagePath(imagePath, config, 1) ==
			SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(spConfigGetImageFeatsPath(imagePath, config, 1) ==
			SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(spConfigGetPCAPath(imagePath, config) ==
			SP_CONFIG_INVALID_ARGUMENT);

	spConfigDestroy(config);
	return true;
}

/*
 * if config == NULL and imagePath != NULL and index in range , then
 * spConfigGetImagePath == spConfigGetImageFeatsPath == spConfigGetPCAPath ==
 * SP_CONFIG_INVALID_ARGUMENT
 * @return true if condition is met
 * @return false otherwise
 */
bool getPathNullConfigTest() {
	SPConfig config = NULL;
	char* imagePath = (char*) malloc(sizeof(*imagePath));

	ASSERT_TRUE(spConfigGetImagePath(imagePath, config, 1) ==
			SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(spConfigGetImageFeatsPath(imagePath, config, 1) ==
			SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(spConfigGetPCAPath(imagePath, config) ==
			SP_CONFIG_INVALID_ARGUMENT);

	free(imagePath);
	return true;
}

/*
 * if config != NULL and imagePath != NULL and
 * index not in range of [1, config->numOfImages], then
 * @return true
 * @return false otherwise
 */
bool getPathWithOutboundsIndexTest() {
	SPConfig config;
	SP_CONFIG_MSG msg;
	const char* configFilename = "./files_for_unit_tests/configExample1.txt";
	config = spConfigCreate(configFilename, &msg);
	ASSERT_NOT_NULL(config);
	char* imagePath = (char*) malloc(sizeof(*imagePath));
	int numOfImages = spConfigGetNumOfImages(config, &msg);

	ASSERT_TRUE(spConfigGetImagePath(imagePath, config, 0) ==
			SP_CONFIG_INDEX_OUT_OF_RANGE);
	ASSERT_TRUE(spConfigGetImagePath(imagePath, config, -3) ==
			SP_CONFIG_INDEX_OUT_OF_RANGE);
	ASSERT_TRUE(spConfigGetImagePath(imagePath, config, numOfImages) ==
			SP_CONFIG_INDEX_OUT_OF_RANGE);
	ASSERT_TRUE(spConfigGetImagePath(imagePath, config, numOfImages + 1) ==
			SP_CONFIG_INDEX_OUT_OF_RANGE);

	ASSERT_TRUE(spConfigGetImageFeatsPath(imagePath, config, 0) ==
			SP_CONFIG_INDEX_OUT_OF_RANGE);
	ASSERT_TRUE(spConfigGetImageFeatsPath(imagePath, config, -3) ==
			SP_CONFIG_INDEX_OUT_OF_RANGE);
	ASSERT_TRUE(spConfigGetImageFeatsPath(imagePath, config, numOfImages) ==
			SP_CONFIG_INDEX_OUT_OF_RANGE);
	ASSERT_TRUE(spConfigGetImageFeatsPath(imagePath, config, numOfImages + 1) ==
			SP_CONFIG_INDEX_OUT_OF_RANGE);

	free(imagePath);
	spConfigDestroy(config);
	return true;
}

/*
 * if config != NULL and imagePath != NULL and
 * index in range of [1, config->numOfImages], and
 * if expected paths equal to the values returned from the functions the
 * @return true
 * @return false otherwise
 */
bool getPathWithValidArgumentsTest() {
	SPConfig config;
	SP_CONFIG_MSG msg;
	const char* configFilename = "./files_for_unit_tests/configExample1.txt";
	config = spConfigCreate(configFilename, &msg);
	ASSERT_NOT_NULL(config);
	char* imagePath = (char*) malloc(sizeof(char) * MAX_SIZE * 4);
	const char* expImagePath = "./images/img1.png";
	const char* expFeatsPath = "./images/img1.feats";
	const char* expPCAPath = "./images/pca.yml";


	ASSERT_TRUE(strcmp(spConfigGetImagePath(imagePath, config, 1),
			expImagePath) == 0);
	ASSERT_TRUE(strcmp(spConfigGetImageFeatsPath(imagePath, config, 1),
			expFeatsPath) == 0);
	ASSERT_TRUE(strcmp(spConfigGetPCAPath(imagePath, config),
			expPCAPath) == 0);

	free(imagePath);
	spConfigDestroy(config);
	return true;
}

int sp_config_unit_tests() {
	RUN_TEST(spConfigBasicTest1);
	RUN_TEST(spConfigUninitialized);
	RUN_TEST(getPathNullImagepathTest);
	RUN_TEST(getPathNullConfigTest);
	RUN_TEST(getPathWithOutboundsIndexTest);
	RUN_TEST(getPathWithValidArgumentsTest);

	return 0;
}

