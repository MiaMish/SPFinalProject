/*
 * SPKDArray.c
 *
 *  Created on: 15 баев„ 2016
 *      Author: user
 */

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include "SPKDArray.h"

#define NULL_CHECK(val,kdArr) if (val == NULL) { spKDArrayDestroy(kdArr); return NULL; }

void fillIndices(SPKDArray* kdArr, int coor);
void sortIndices(SPKDArray* kdArr, int coor);

struct SPKDArray {
	SPPoint* points;
	int pointsCount;
	int dim;
	int** sortedIndices;
};

SPKDArray* InitBasic(SPPoint* arr, int size, int dim) {
	SPKDArray* kdArr = (SPKDArray*) malloc(sizeof(SPKDArray));
	NULL_CHECK(kdArr, kdArr);

	kdArr->pointsCount = size;
	kdArr->points = (SPPoint*) malloc(sizeof(SPPoint) * size);
	NULL_CHECK(kdArr->points, kdArr);

	for (int i = 0; i < size; i++) {
		SPPoint curr = spPointCopy(arr[i]);
		NULL_CHECK(curr, kdArr);
		kdArr->points[i] = curr;
	}

	kdArr->dim = dim;

	kdArr->sortedIndices = (int**) calloc(dim, sizeof(int*));
	NULL_CHECK(kdArr->sortedIndices, kdArr);

	for (int i = 0; i < dim; i++) {
		kdArr->sortedIndices[i] = (int*) malloc(sizeof(int) * size);
		NULL_CHECK(kdArr->sortedIndices[i], kdArr);
	}

	return kdArr;
}

SPKDArray* spKDArrayInit(SPPoint* arr, int size, int dim) {
	SPKDArray* kdArr = InitBasic(arr, size, dim);
	if (kdArr == NULL) {
		return NULL;
	}

	for (int i = 0; i < dim; i++) {
		fillIndices(kdArr, i);
		sortIndices(kdArr, i);
	}

	return kdArr;
}

void spKDArrayDestroy(SPKDArray* kdArr) {
	if (kdArr == NULL) {
		return;
	}
	if (kdArr->points != NULL) {
		for (int i = 0; i < kdArr->pointsCount; i++) {
			spPointDestroy(kdArr->points[i]);
		}

		free(kdArr->points);

	}
	if (kdArr->sortedIndices != NULL) {
		for (int i = 0; i < kdArr->dim; i++) {
			free(kdArr->sortedIndices[i]);
		}
		free(kdArr->sortedIndices);
	}
	free(kdArr);
}

void fillIndices(SPKDArray* kdArr, int coor) {
	for (int i = 0; i < kdArr->pointsCount; i++) {
		kdArr->sortedIndices[coor][i] = i;
	}
}

static SPKDArray* currentKdArr = NULL;

static int currentAxis = -1;

int pointsComparator(const void* ptr1, const void* ptr2) {
	int index1 = *(const int*) ptr1;
	int index2 = *(const int*) ptr2;

	double coor1 = spPointGetAxisCoor(currentKdArr->points[index1],
			currentAxis);
	double coor2 = spPointGetAxisCoor(currentKdArr->points[index2],
			currentAxis);

	return coor1 - coor2;
}

void sortIndices(SPKDArray* kdArr, int axis) {
	currentKdArr = kdArr;
	currentAxis = axis;
	qsort(kdArr->sortedIndices[axis], kdArr->pointsCount, sizeof(int),
			pointsComparator);
}

#define SPLIT_CLEANUP(leftMap, rightMap, leftPoints, rightPoints, leftTree, rightTree)   \
	free(leftMap);   \
	free(rightMap);   \
	free(leftPoints);   \
	free(rightPoints);   \
	spKDArrayDestroy(leftTree);   \
	spKDArrayDestroy(rightTree)

void spKDArraySplit(SPKDArray* kdArr, int coor, SPKDArray** kdLeft,
		SPKDArray** kdRight) {
	*kdLeft = NULL;
	*kdRight = NULL;

	int* leftMap = (int*) calloc(kdArr->pointsCount, sizeof(int));
	int* rightMap = (int*) calloc(kdArr->pointsCount, sizeof(int));

	int leftSize = (kdArr->pointsCount + 1) / 2;
	int rightSize = kdArr->pointsCount / 2;
	SPPoint* leftPoints = (SPPoint*) calloc(leftSize, sizeof(SPPoint));
	SPPoint* rightPoints = (SPPoint*) calloc(rightSize, sizeof(SPPoint));

	if (leftMap == NULL || rightMap == NULL) {
		SPLIT_CLEANUP(leftMap, rightMap, leftPoints, rightPoints, NULL, NULL);
		return;
	}

	int i;
	for (i = 0; i < leftSize; i++) {
		int currIndex = kdArr->sortedIndices[coor][i];
		leftMap[currIndex] = i + 1; // mark lefts
		leftPoints[i] = kdArr->points[currIndex];
	}
	for (int j = 0; i < kdArr->pointsCount; i++, j++) {
		int currIndex = kdArr->sortedIndices[coor][i];
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
		int leftSpot = 0;
		int rightSpot = 0;
		for (int j = 0; j < kdArr->pointsCount; j++) {
			int currIndex = kdArr->sortedIndices[i][j];
			if (leftMap[currIndex] > 0) {
				(*kdLeft)->sortedIndices[i][leftSpot] = leftMap[currIndex] - 1;
				leftSpot++;
			} else if (rightMap[currIndex] > 0) {
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

	for (int i = 0; i < kdArr->dim; i++) {
		double minPointVal = INT_MAX;
		double maxPointVal = INT_MIN;
		for (int j = 0; j < kdArr->pointsCount; j++) {
			int index = kdArr->sortedIndices[i][j];
			double currVal = spPointGetAxisCoor(kdArr->points[index], i);
			if (currVal < minPointVal) {
				minPointVal = currVal;
			}
			if (currVal > maxPointVal) {
				maxPointVal = currVal;
			}
		}
		double spread = maxPointVal - minPointVal;
		if (spread > maxSpread) {
			maxSpread = spread;
			maxSpreadDim = i;
		}
	}

	return maxSpreadDim;
}

double spKDArrayGetMedian(SPKDArray* kdArr, int axis) {
	int meanIndex = kdArr->sortedIndices[axis][(kdArr->pointsCount - 1) / 2];
	SPPoint meanPoint = kdArr->points[meanIndex];
	return spPointGetAxisCoor(meanPoint, axis);
}
