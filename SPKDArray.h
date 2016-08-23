/*
 * KDArray.h
 *
 *  Created on: 15 баев„ 2016
 *      Author: user
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include "SPPoint.h"

struct SPKDArray;
typedef struct SPKDArray SPKDArray;

SPKDArray* spKDArrayInit(SPPoint* arr, int size, int dim);

void spKDArrayDestroy(SPKDArray* kdArr);

void spKDArraySplit(SPKDArray* kdArr, int coor, SPKDArray** kdLeft, SPKDArray** kdRight);

int spKDArrayGetPointsCount(SPKDArray* kdArr);

SPPoint spKDArrayGetPointAt(SPKDArray* kdArr, int i);

double spKDArrayGetPointVal(SPKDArray* kdArr, int dim, int i);

int spKDArrayGetDimension(SPKDArray* kdArr);

int spKDArrayFindMaxSpreadDimension(SPKDArray* kdArr);

double spKDArrayGetMedian(SPKDArray* kdArr, int axis);

#endif /* SPKDARRAY_H_ */
