#include "SPFeaturesSerializer.h"
#include "SPLogger.h"
#include "SPConfig.h"
#include "SPPoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

SP_CONFIG_MSG getFeatsFile(SPConfig config, int imageIndex, FILE** file, const char* mode) {
	char featsPath[260];

	SP_CONFIG_MSG msg = spConfigGetImageFeatsPath(featsPath, config,
			imageIndex);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(featsPathErr, __FILE__, __func__, __LINE__);
		return msg;
	}

	*file = fopen(featsPath, mode);
	if (*file == NULL) {
		msg = SP_CONFIG_UNKNOWN_ERROR;
		spLoggerPrintError(featsFileErr, __FILE__, __func__, __LINE__);
		return msg;
	}

	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG writeImageFeaturesToFile(SPPoint* imFeatures, int numOfFeats,
		SPConfig config, int imageIndex) {
	FILE* featsFile;
	SP_CONFIG_MSG msg = getFeatsFile(config, imageIndex, &featsFile, "w+");
	if (msg != SP_CONFIG_SUCCESS) {
		return msg;
	}

	fprintf(featsFile, "%d\n", numOfFeats);
	for (int i = 0; i < numOfFeats; i++) {
		SPPoint point = imFeatures[i];
		fprintf(featsFile, "%d,%d\n", spPointGetIndex(point),
				spPointGetDimension(point));
		for (int j = 0; j < spPointGetDimension(point); j++) {
			fprintf(featsFile, "%lf\n", spPointGetAxisCoor(point, j));
		}
	}

	fflush(featsFile);
	fclose(featsFile);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG readImageFeaturesFromFile(SPPoint** imFeatures, int* numOfFeats,
		SPConfig config, int imageIndex) {
	FILE* featsFile;
	SP_CONFIG_MSG msg = getFeatsFile(config, imageIndex, &featsFile, "r+");
	if (msg != SP_CONFIG_SUCCESS) {
		return msg;
	}

	fscanf(featsFile, "%d", numOfFeats);

	*imFeatures = (SPPoint*) malloc(sizeof(SPPoint) * *numOfFeats);
	VERIFY_ALLOC(*imFeatures);

	for (int i = 0; i < *numOfFeats; i++) {
		int index;
		int dimension;
		fscanf(featsFile, "%d,%d", &index, &dimension);

		double values[dimension];

		for (int j = 0; j < dimension; j++) {
			fscanf(featsFile, "%lf", &(values[j]));
		}

		SPPoint point = spPointCreate(values, dimension, index);
		VERIFY_ALLOC(point);
		(*imFeatures)[i] = point;
	}

	fclose(featsFile);
	return SP_CONFIG_SUCCESS;
}
