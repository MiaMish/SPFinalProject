/*
 * SPFeaturesSerializer.h
 *
 *  Created on: 3 בספט׳ 2016
 *      Author: user
 */

#ifndef SPFEATURESSERIALIZER_H_
#define SPFEATURESSERIALIZER_H_

#include "SPConfig.h"
#include "SPPoint.h"

#define VERIFY_ALLOC(ptr)   \
	if ((ptr) == NULL) {	\
		msg = SP_CONFIG_ALLOC_FAIL; \
		spLoggerPrintError(allocFail, __FILE__, __func__, __LINE__); \
		return msg; \
	}

/*
 * @param imFeatures - the features extracted
 * @param numOfFeats - the number of features
 * @param config - the configs provider
 * @param imageIndex - index of the image to which the feats belong to
 *
 * Creates features file for the index-th image in directory
 *
 * @return SP_CONFIG_UNKNOWN_ERROR if fopens fails
 * @return SP_CONFIG_SUCCESS if successful
 */
SP_CONFIG_MSG writeImageFeaturesToFile(SPPoint* imFeatures, int numOfFeats, SPConfig config, int imageIndex);

/*
 * @param imFeatures - an output parameter for the imported features
 * @param numOfFeats - an output parameter for the number of imported features
 * @param config - the configs provider
 * @param imageIndex - index of the image to which the feats belong to
 *
 * Creates features file for the index-th image in directory
 *
 * @return SP_CONFIG_UNKNOWN_ERROR if fopens fails
 * @return SP_CONFIG_SUCCESS if successful
 */
SP_CONFIG_MSG readImageFeaturesFromFile(SPPoint** imFeatures, int* numOfFeats, SPConfig config, int imageIndex);


#endif /* SPFEATURESSERIALIZER_H_ */
