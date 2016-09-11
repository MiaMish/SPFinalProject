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

int sp_config_unit_tests() {
	RUN_TEST(spConfigBasicTest1);

	return 0;
}

