#include "../SPKDArray.h"
#include "unit_test_util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "unit_tests.h"

#define POINTS_SIZE 5
#define POINTS_DIM 3

/*
 * Helper function to create an array of points
 */
SPPoint* fillPoints() {
	SPPoint* points = (SPPoint*) malloc(sizeof(SPPoint) * POINTS_SIZE);

	double values0[POINTS_DIM] = { 1, 2, 3 };
	points[0] = spPointCreate(values0, POINTS_DIM, 0);

	double values1[POINTS_DIM] = { 123, 70, 321 };
	points[1] = spPointCreate(values1, POINTS_DIM, 1);

	double values2[POINTS_DIM] = { 2, 7, -17 };
	points[2] = spPointCreate(values2, POINTS_DIM, 2);

	double values3[POINTS_DIM] = { 9, 11, 5 };
	points[3] = spPointCreate(values3, POINTS_DIM, 3);

	double values4[POINTS_DIM] = { 3, 4, 911 };
	points[4] = spPointCreate(values4, POINTS_DIM, 4);

	return points;
}

/*
 * Helper function to destroy an array of points
 */
void killPoints(SPPoint* points) {
	for (int i = 0; i < POINTS_SIZE; i++) {
		spPointDestroy(points[i]);
	}
	free(points);
}

/*
 * Helper macro to check points equality
 */
#define assertPointsEquals(a,b)   \
	for (int ii = 0; ii < POINTS_DIM; ii++) {   \
		ASSERT_EQUALS(spPointGetAxisCoor(a, ii), spPointGetAxisCoor(b, ii));   \
	}


/*
 * Check init and destroy of kd-array
 */
bool InitAndDestroyArray() {
	SPPoint* points = fillPoints();
	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	ASSERT_NOT_NULL(kdArr);

	killPoints(points);
	spKDArrayDestroy(kdArr);
	return true;
}

/*
 * Check points count getter
 */
bool GetPointsCount() {
	SPPoint* points = fillPoints();
	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	ASSERT_EQUALS(spKDArrayGetPointsCount(kdArr), POINTS_SIZE);

	killPoints(points);
	spKDArrayDestroy(kdArr);
	return true;
}

/*
 * Check point at index getter
 */
bool GetPointAt() {
	SPPoint* points = fillPoints();
	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	for (int i = 0; i < POINTS_SIZE; i++) {
		SPPoint point = spKDArrayGetPointAt(kdArr, i);
		assertPointsEquals(point, points[i]);
	}

	killPoints(points);
	spKDArrayDestroy(kdArr);
	return true;
}

/*
 * Check point by dimension and position in dimension value getter
 */
bool GetPointVal() {
	SPPoint* points = fillPoints();
	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 0, 0), 1);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 0, 1), 2);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 0, 2), 3);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 0, 3), 9);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 0, 4), 123);

	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 1, 0), 2);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 1, 1), 4);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 1, 2), 7);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 1, 3), 11);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 1, 4), 70);

	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 2, 0), -17);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 2, 1), 3);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 2, 2), 5);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 2, 3), 321);
	ASSERT_EQUALS(spKDArrayGetPointVal(kdArr, 2, 4), 911);

	killPoints(points);
	spKDArrayDestroy(kdArr);
	return true;
}

/*
 * Check kd-array dimension getter
 */
bool GetArrayDimension() {
	SPPoint* points = fillPoints();
	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	ASSERT_EQUALS(spKDArrayGetDimension(kdArr), POINTS_DIM);

	killPoints(points);
	spKDArrayDestroy(kdArr);
	return true;
}

/*
 * Check median getter per dimension
 */
bool GetAxisMedian() {
	SPPoint* points = fillPoints();
	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	ASSERT_EQUALS(spKDArrayGetMedian(kdArr, 0), 3);
	ASSERT_EQUALS(spKDArrayGetMedian(kdArr, 1), 7);
	ASSERT_EQUALS(spKDArrayGetMedian(kdArr, 2), 5);

	killPoints(points);
	spKDArrayDestroy(kdArr);
	return true;
}

/*
 * Check max spread getter per dimension
 */
bool FindMaxSpreadDimension() {
	SPPoint* points = fillPoints();
	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	ASSERT_EQUALS(spKDArrayFindMaxSpreadDimension(kdArr), 2);

	killPoints(points);
	spKDArrayDestroy(kdArr);
	return true;
}

/*
 * Check array splitting
 */
bool SplitArray() {
	SPPoint* points = fillPoints();
	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	SPKDArray* left = NULL;
	SPKDArray* right = NULL;

	spKDArraySplit(kdArr, 0, &left, &right);
	ASSERT_NOT_NULL(left);
	ASSERT_NOT_NULL(right);

	assertPointsEquals(points[0], spKDArrayGetPointAt(left, 0));
	assertPointsEquals(points[2], spKDArrayGetPointAt(left, 1));
	assertPointsEquals(points[4], spKDArrayGetPointAt(left, 2));
	assertPointsEquals(points[3], spKDArrayGetPointAt(right, 0));
	assertPointsEquals(points[1], spKDArrayGetPointAt(right, 1));

	spKDArrayDestroy(left);
	spKDArrayDestroy(right);

	spKDArraySplit(kdArr, 1, &left, &right);
	ASSERT_NOT_NULL(left);
	ASSERT_NOT_NULL(right);

	assertPointsEquals(points[0], spKDArrayGetPointAt(left, 0));
	assertPointsEquals(points[4], spKDArrayGetPointAt(left, 1));
	assertPointsEquals(points[2], spKDArrayGetPointAt(left, 2));
	assertPointsEquals(points[3], spKDArrayGetPointAt(right, 0));
	assertPointsEquals(points[1], spKDArrayGetPointAt(right, 1));

	spKDArrayDestroy(left);
	spKDArrayDestroy(right);

	spKDArraySplit(kdArr, 2, &left, &right);
	ASSERT_NOT_NULL(left);
	ASSERT_NOT_NULL(right);

	assertPointsEquals(points[2], spKDArrayGetPointAt(left, 0));
	assertPointsEquals(points[0], spKDArrayGetPointAt(left, 1));
	assertPointsEquals(points[3], spKDArrayGetPointAt(left, 2));
	assertPointsEquals(points[1], spKDArrayGetPointAt(right, 0));
	assertPointsEquals(points[4], spKDArrayGetPointAt(right, 1));

	killPoints(points);
	spKDArrayDestroy(kdArr);
	return true;
}

/*
 * main tests runner
 */
int sp_kd_array_unit_tests() {
	RUN_TEST(InitAndDestroyArray);
	RUN_TEST(GetPointsCount);
	RUN_TEST(GetPointAt);
	RUN_TEST(GetPointVal);
	RUN_TEST(GetArrayDimension);
	RUN_TEST(GetAxisMedian);
	RUN_TEST(FindMaxSpreadDimension);
	RUN_TEST(SplitArray);
	return 0;
}

