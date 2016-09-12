/*
 * KDArray.h
 *
 *  Created on: 15 баев„ 2016
 *      Author: user
 */

#ifndef SPKDARRAY_H_
#define SPKDARRAY_H_

#include "SPPoint.h"

/*
 * struct for kd-array data structure
 */
struct SPKDArray;
typedef struct SPKDArray SPKDArray;

/*
 * @param arr - an array of points
 * @param size - the size of the points array
 * @param dim - the dimension of the points
 *
 * The function is building a new kd-array composed of the given points
 *
 * @return NULL on any allocation or other initialization error
 * @return a newly constructed kd-array otherwise
 */
SPKDArray* spKDArrayInit(SPPoint* arr, int size, int dim);

/*
 * @param kdArr - a kd-array
 *
 * The function destroys a given kd-array
 *
 */
void spKDArrayDestroy(SPKDArray* kdArr);

/*
 * @param kdArr - a kd-array
 * @param coor - the dimension on which to split
 * @param kdLeft - an output parameter for the left splitted array
 * @param kdRight - and output parameter for the right splitted array
 *
 * The function gets a kd-array and splits it to two arrays according to a given split dimension
 *
 */
void spKDArraySplit(SPKDArray* kdArr, int coor, SPKDArray** kdLeft, SPKDArray** kdRight);

/*
 * @param kdArr - a kd-array
 *
 * The function returns the number of points stored in the array
 *
 */
int spKDArrayGetPointsCount(SPKDArray* kdArr);

/*
 * @param kdArr - a kd-array
 * @param i - an index
 *
 * The function returns the i-th point stored in the array
 *
 */
SPPoint spKDArrayGetPointAt(SPKDArray* kdArr, int i);

/*
 * @param kdArr - a kd-array
 * @param dim - a dimension
 * @param i - an index
 *
 * The function returns the i-th ordered point in the given dimension
 *
 */
double spKDArrayGetPointVal(SPKDArray* kdArr, int dim, int i);

/*
 * @param kdArr - a kd-array
 *
 * The function returns the dimension of the array
 *
 */
int spKDArrayGetDimension(SPKDArray* kdArr);

/*
 * @param kdArr - a kd-array
 *
 * The function returns the dimension which has the max spread
 *
 */
int spKDArrayFindMaxSpreadDimension(SPKDArray* kdArr);

/*
 * @param kdArr - a kd-array
 * @param axis - a dimension
 *
 * The function returns the median value according to the given dimension
 *
 */
double spKDArrayGetMedian(SPKDArray* kdArr, int axis);

#endif /* SPKDARRAY_H_ */
