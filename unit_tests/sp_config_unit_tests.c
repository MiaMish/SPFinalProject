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

/*
 * basic tester for spConfigCreate based on example in instruction file
 */
bool spConfigBasicTest1() {
	SPConfig config;
	SP_CONFIG_MSG* msg;
	char* configFilename = "./files_for_unit_tests/configExample1.txt";

	char* expDir = "./images/";
	char* expPrefix = "img";
	char* expSuffix = ".png";
	int expNumOfIm = 17;
	int expPCADim = 20;
	char* expPCAFilename = "pca.yml";
	int expNumOfFeatures = 100;
	bool expExtrMode = true;
	bool expMinimalGui = false;
	int expNumOfSimIm = 5;
	char* expLoggerFilename = "stdout";
	int expLoggerLevel = 2;
	SplitMethod expMethod = MAX_SPREAD;

	msg = (SP_CONFIG_MSG*) malloc(sizeof(SP_CONFIG_MSG));
	ASSERT_FALSE(msg);
	config = spConfigCreate(configFilename, msg);
	ASSERT_FALSE(config);

	ASSERT_TRUE(spConfigIsExtractionMode(config, msg) == expExtrMode);
	ASSERT_TRUE(spConfigMinimalGui(config, msg) == expMinimalGui);

	ASSERT_TRUE(spConfigGetNumOfImages(config, msg) == expNumOfIm);
	ASSERT_TRUE(spConfigGetPCADim(config, msg) == expPCADim);
	ASSERT_TRUE(spConfigGetNumOfFeatures(config, msg) == expNumOfFeatures);
	ASSERT_TRUE(spConfigGetNumOfSimIms(config, msg) == expNumOfSimIm);
	ASSERT_TRUE(spConfigGetLogLevel(config, msg) == expLoggerLevel);

	ASSERT_TRUE(strcmp(spConfigGetDirectory(config, msg), expDir) == 0);
	ASSERT_TRUE(strcmp(spConfigGetPrefix(config, msg), expPrefix) == 0);
	ASSERT_TRUE(strcmp(spConfigGetSuffix(config, msg), expSuffix) == 0);
	ASSERT_TRUE(strcmp(spConfigGetPCAFilename(config, msg), expPCAFilename) == 0);
	ASSERT_TRUE(strcmp(spConfigGetLogName(config, msg), expLoggerFilename) == 0);

	ASSERT_TRUE(spConfigGetSplitMethod(config, msg) == expMethod);

	return true;
}

