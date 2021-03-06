/*
 * SPKDArray.c
 *
 *  Created on: 15 áàåâ× 2016
 *      Author: user
 */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include "SPKDArray.h"

#define NULL_CHECK(val,kdArr) if (val == NULL) { spKDArrayDestroy(kdArr); return NULL; }

/*
 * A helper function to initialize the sorted array of indices for the given coordinate
 */
void fillIndices(SPKDArray* kdArr, int coor);

/*
 * A helper function to sort the indices array according to the given coordinate
 */
void sortIndices(SPKDArray* kdArr, int coor);

struct SPKDArray {
	SPPoint* points;
	int pointsCount;
	int dim;
	int** sortedIndices;
};

/*
 * A helper function to initialize a kd-array
 */
SPKDArray* InitBasic(SPPoint* arr, int size, int dim) {
	SPKDArray* kdArr = (SPKDArray*) malloc(sizeof(SPKDArray));
	int i;
	NULL_CHECK(kdArr, kdArr);

	kdArr->pointsCount = size;
	kdArr->points = (SPPoint*) malloc(sizeof(SPPoint) * size);
	NULL_CHECK(kdArr->points, kdArr);

	for (i = 0; i < size; i++) {
		SPPoint curr = spPointCopy(arr[i]);
		NULL_CHECK(curr, kdArr);
		kdArr->points[i] = curr;
	}

	kdArr->dim = dim;

	kdArr->sortedIndices = (int**) calloc(dim, sizeof(int*));
	NULL_CHECK(kdArr->sortedIndices, kdArr);

	for (i = 0; i < dim; i++) {
		kdArr->sortedIndices[i] = (int*) malloc(sizeof(int) * size);
		NULL_CHECK(kdArr->sortedIndices[i], kdArr);
	}

	return kdArr;
}

SPKDArray* spKDArrayInit(SPPoint* arr, int size, int dim) {
	SPKDArray* kdArr = InitBasic(arr, size, dim);
	int i;
	if (kdArr == NULL) {
		return NULL;
	}

	for (i = 0; i < dim; i++) {
		fillIndices(kdArr, i);
		sortIndices(kdArr, i);
	}

	return kdArr;
}

void spKDArrayDestroy(SPKDArray* kdArr) {
	int i;
	if (kdArr == NULL) {
		return;
	}
	if (kdArr->points != NULL) {
		for (i = 0; i < kdArr->pointsCount; i++) {
			spPointDestroy(kdArr->points[i]);
		}

		free(kdArr->points);

	}
	if (kdArr->sortedIndices != NULL) {
		for (i = 0; i < kdArr->dim; i++) {
			free(kdArr->sortedIndices[i]);
		}
		free(kdArr->sortedIndices);
	}
	free(kdArr);
}

void fillIndices(SPKDArray* kdArr, int coor) {
	int i;
	for (i = 0; i < kdArr->pointsCount; i++) {
		kdArr->sortedIndices[coor][i] = i;
	}
}

static SPKDArray* currentKdArr = NULL;

static int currentAxis = -1;

/*
 * A helper function to compare two points in a kd-array
 */
int pointsComparator(const void* ptr1, const void* ptr2) {
	int index1 = *(const int*) ptr1;
	int index2 = *(const int*) ptr2;

	double coor1 = spPointGetAxisCoor(currentKdArr->points[index1],
			currentAxis);
	double coor2 = spPointGetAxisCoor(currentKdArr->points[index2],
			currentAxis);

	if (coor1 == coor2) {
		return index1 - index2;
	}

	return coor1 - coor2;
}

void sortIndices(SPKDArray* kdArr, int axis) {
	currentKdArr = kdArr;
	currentAxis = axis;
	qsort(kdArr->sortedIndices[axis], kdArr->pointsCount, sizeof(int),
			pointsComparator);
}

/*
 * macro to clean up allocations on allocation failure
 */
#define SPLIT_CLEANUP(leftMap, rightMap, leftPoints, rightPoints, leftTree, rightTree)   \
	free(leftMap);   \
	free(rightMap);   \
	free(leftPoints);   \
	free(rightPoints);   \
	spKDArrayDestroy(leftTree);   \
	spKDArrayDestroy(rightTree)

void spKDArraySplit(SPKDArray* kdArr, int coor, SPKDArray** kdLeft,
		SPKDArray** kdRight) {
	int i, j, currIndex, leftSpot, rightSpot;
	
	int* leftMap = (int*) calloc(kdArr->pointsCount, sizeof(int));
	int* rightMap = (int*) calloc(kdArr->pointsCount, sizeof(int));

	int leftSize = (kdArr->pointsCount + 1) / 2;
	int rightSize = kdArr->pointsCount / 2;
	SPPoint* leftPoints = (SPPoint*) calloc(leftSize, sizeof(SPPoint));
	SPPoint* rightPoints = (SPPoint*) calloc(rightSize, sizeof(SPPoint));
	
	*kdLeft = NULL;
	*kdRight = NULL;

	if (leftMap == NULL || rightMap == NULL || leftPoints == NULL || rightPoints == NULL) {
		SPLIT_CLEANUP(leftMap, rightMap, leftPoints, rightPoints, NULL, NULL);
		return;
	}
	
	for (i = 0; i < leftSize; i++) {
		currIndex = kdArr->sortedIndices[coor][i];
		leftMap[currIndex] = i + 1; // mark lefts
		leftPoints[i] = kdArr->points[currIndex];
	}
	for (j = 0; i < kdArr->pointsCount; i++, j++) {
		currIndex = kdArr->sortedIndices[coor][i];
		rightMap[currIndex] = j + 1; // mark rights
		rightPoints[j] = kdArr->points[currIndex];
	}

	*kdLeft = InitBasic(leftPoints, leftSize, kdArr->dim);
	*kdRight = InitBasic(rightPoints, rightSize, kdArr->dim);

	if (*kdLeft == NULL || *kdRight == NULL) {
		SPLIT_CLEANUP(leftMap, rightMap, leftPoints, rightPoints, *kdLeft,
				*kdRight);
		return;
	}

	for (i = 0; i < kdArr->dim; i++) {
		leftSpot = 0;
		rightSpot = 0;
		for (j = 0; j < kdArr->pointsCount; j++) {
			currIndex = kdArr->sortedIndices[i][j];
			if (leftMap[currIndex] > 0) {
				assert(leftMap[currIndex] <= leftSize);
				(*kdLeft)->sortedIndices[i][leftSpot] = leftMap[currIndex] - 1;
				leftSpot++;
			} else if (rightMap[currIndex] > 0) {
				assert(rightMap[currIndex] <= rightSize);
				(*kdRight)->sortedIndices[i][rightSpot] = rightMap[currIndex] - 1;
				rightSpot++;
			} else {
				assert(false);
			}
		}
		if (leftSpot != leftSize || rightSpot != rightSize) {
			assert(leftSpot == leftSize);
			assert(rightSpot == rightSize);
		}
	}

	SPLIT_CLEANUP(leftMap, rightMap, leftPoints, rightPoints, NULL, NULL);
}

int spKDArrayGetPointsCount(SPKDArray* kdArr) {
	return kdArr->pointsCount;
}

SPPoint spKDArrayGetPointAt(SPKDArray* kdArr, int i) {
	return kdArr->points[i];
}

double spKDArrayGetPointVal(SPKDArray* kdArr, int dim, int i) {
	int index = kdArr->sortedIndices[dim][i];
	return spPointGetAxisCoor(kdArr->points[index], dim);
}

int spKDArrayGetDimension(SPKDArray* kdArr) {
	return kdArr->dim;
}

int spKDArrayFindMaxSpreadDimension(SPKDArray* kdArr) {
	int maxSpread = INT_MIN;
	int maxSpreadDim = -1;
	int i, j;
	double minPointVal, maxPointVal, currVal, spread;

	for (i = 0; i < kdArr->dim; i++) {
		minPointVal = INT_MAX;
		maxPointVal = INT_MIN;
		for (j = 0; j < kdArr->pointsCount; j++) {
			currVal = spPointGetAxisCoor(kdArr->points[j], i);
			if (currVal < minPointVal) {
				minPointVal = currVal;
			}
			if (currVal > maxPointVal) {
				maxPointVal = currVal;
			}
		}
		spread = maxPointVal - minPointVal;
		if (spread > maxSpread) {
			maxSpread = spread;
			maxSpreadDim = i;
		}
	}

	return maxSpreadDim;
}

double spKDArrayGetMedian(SPKDArray* kdArr, int axis) {
	int meanIndex;
	SPPoint meanPoint;
	assert(kdArr->pointsCount >= 2);
	meanIndex = kdArr->sortedIndices[axis][(kdArr->pointsCount - 1) / 2];
	meanPoint = kdArr->points[meanIndex];
	return spPointGetAxisCoor(meanPoint, axis);
}
