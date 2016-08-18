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

SPKDArray* SPKDArrayInit(SPPoint* arr, int size, int dim);

void SPKDArrayDestroy(SPKDArray* kdArr);

void SPKDArraySplit(SPKDArray* kdArr, int coor, SPKDArray** kdLeft, SPKDArray** kdRight);

int SPKDArrayGetPointsCount(SPKDArray* kdArr);

SPPoint SPKDArrayGetPointAt(SPKDArray* kdArr, int i);

double SPKDArrayGetPointVal(SPKDArray* kdArr, int dim, int i);

int SPKDArrayGetDimension(SPKDArray* kdArr);

int SPKDArrayFindMaxSpreadDimension(SPKDArray* kdArr);

double SPKDArrayGetMedian(SPKDArray* kdArr, int axis);

#endif /* SPKDARRAY_H_ */
