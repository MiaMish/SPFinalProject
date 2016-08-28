/*
 * mainaux.c
 */

#include "SPPoint.h"
#include "SPImageProc.h"

void extractFeaturesFromIm(const SPConfig config, SP_CONFIG_MSG* msg, int index) {
	char imagePath[1024];
	SPPoint* imFeatures;
	int* numOfFeats = (int*)malloc(sizeof(int));

	msg = spConfigGetImagePath(imagePath, config, index);
	imFeatures = getImageFeatures(imagePath, index, numOfFeats);
}


