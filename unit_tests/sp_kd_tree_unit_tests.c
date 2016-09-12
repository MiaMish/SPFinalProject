#include "../SPKDTree.h"
#include "../SPKDArray.h"
#include "unit_test_util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "unit_tests.h"

#define POINTS_SIZE 7
#define POINTS_DIM 4

/*
 * Helper macro to test points equality
 */
#define assertPointsEquals(a,b)   \
	for (int ii = 0; ii < POINTS_DIM; ii++) {   \
		ASSERT_EQUALS(spPointGetAxisCoor(a, ii), spPointGetAxisCoor(b, ii));   \
	}

/*
 * Helper methods to get a points array
 */
SPPoint* fillTreePoints() {
	SPPoint* points = (SPPoint*) malloc(sizeof(SPPoint) * POINTS_SIZE);

	double values0[POINTS_DIM] = { 0, 0, 0, 0 };
	points[0] = spPointCreate(values0, POINTS_DIM, 0);

	double values1[POINTS_DIM] = { 1, 2, 1, 2 };
	points[1] = spPointCreate(values1, POINTS_DIM, 1);

	double values2[POINTS_DIM] = { 9, 1, 4, -2 };
	points[2] = spPointCreate(values2, POINTS_DIM, 2);

	double values3[POINTS_DIM] = { 5, 1, 5, -6 };
	points[3] = spPointCreate(values3, POINTS_DIM, 3);

	double values4[POINTS_DIM] = { -1, -2, 0, 3 };
	points[4] = spPointCreate(values4, POINTS_DIM, 4);

	double values5[POINTS_DIM] = { 10, 8, 2, 1 };
	points[5] = spPointCreate(values5, POINTS_DIM, 5);

	double values6[POINTS_DIM] = { -2, 2, -4, 4 };
	points[6] = spPointCreate(values6, POINTS_DIM, 6);

	return points;
}

/*
 * Helper method to destroy a points array
 */
void killTreePoints(SPPoint* points) {
	for (int i = 0; i < POINTS_SIZE; i++) {
		spPointDestroy(points[i]);
	}
	free(points);
}

/*
 * Test split of kd-array as used by kd-tree
 */
bool KDArrayForKDTreeTest() {
	SPPoint* points = fillTreePoints();
	SPPoint A = points[0];
	SPPoint B = points[1];
	SPPoint C = points[2];
	SPPoint D = points[3];
	SPPoint E = points[4];
	SPPoint F = points[5];
	SPPoint G = points[6];

	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	// dim 0
	ASSERT_EQUALS(spPointGetAxisCoor(G, 0), spKDArrayGetPointVal(kdArr, 0, 0));
	ASSERT_EQUALS(spPointGetAxisCoor(E, 0), spKDArrayGetPointVal(kdArr, 0, 1));
	ASSERT_EQUALS(spPointGetAxisCoor(A, 0), spKDArrayGetPointVal(kdArr, 0, 2));
	ASSERT_EQUALS(spPointGetAxisCoor(B, 0), spKDArrayGetPointVal(kdArr, 0, 3));
	ASSERT_EQUALS(spPointGetAxisCoor(D, 0), spKDArrayGetPointVal(kdArr, 0, 4));
	ASSERT_EQUALS(spPointGetAxisCoor(C, 0), spKDArrayGetPointVal(kdArr, 0, 5));
	ASSERT_EQUALS(spPointGetAxisCoor(F, 0), spKDArrayGetPointVal(kdArr, 0, 6));

	// dim1
	ASSERT_EQUALS(spPointGetAxisCoor(E, 1), spKDArrayGetPointVal(kdArr, 1, 0));
	ASSERT_EQUALS(spPointGetAxisCoor(A, 1), spKDArrayGetPointVal(kdArr, 1, 1));
	ASSERT_EQUALS(spPointGetAxisCoor(C, 1), spKDArrayGetPointVal(kdArr, 1, 2));
	ASSERT_EQUALS(spPointGetAxisCoor(D, 1), spKDArrayGetPointVal(kdArr, 1, 3));
	ASSERT_EQUALS(spPointGetAxisCoor(B, 1), spKDArrayGetPointVal(kdArr, 1, 4));
	ASSERT_EQUALS(spPointGetAxisCoor(G, 1), spKDArrayGetPointVal(kdArr, 1, 5));
	ASSERT_EQUALS(spPointGetAxisCoor(F, 1), spKDArrayGetPointVal(kdArr, 1, 6));

	// dim2
	ASSERT_EQUALS(spPointGetAxisCoor(G, 2), spKDArrayGetPointVal(kdArr, 2, 0));
	ASSERT_EQUALS(spPointGetAxisCoor(A, 2), spKDArrayGetPointVal(kdArr, 2, 1));
	ASSERT_EQUALS(spPointGetAxisCoor(E, 2), spKDArrayGetPointVal(kdArr, 2, 2));
	ASSERT_EQUALS(spPointGetAxisCoor(B, 2), spKDArrayGetPointVal(kdArr, 2, 3));
	ASSERT_EQUALS(spPointGetAxisCoor(F, 2), spKDArrayGetPointVal(kdArr, 2, 4));
	ASSERT_EQUALS(spPointGetAxisCoor(C, 2), spKDArrayGetPointVal(kdArr, 2, 5));
	ASSERT_EQUALS(spPointGetAxisCoor(D, 2), spKDArrayGetPointVal(kdArr, 2, 6));

	// dim3
	ASSERT_EQUALS(spPointGetAxisCoor(D, 3), spKDArrayGetPointVal(kdArr, 3, 0));
	ASSERT_EQUALS(spPointGetAxisCoor(C, 3), spKDArrayGetPointVal(kdArr, 3, 1));
	ASSERT_EQUALS(spPointGetAxisCoor(A, 3), spKDArrayGetPointVal(kdArr, 3, 2));
	ASSERT_EQUALS(spPointGetAxisCoor(F, 3), spKDArrayGetPointVal(kdArr, 3, 3));
	ASSERT_EQUALS(spPointGetAxisCoor(B, 3), spKDArrayGetPointVal(kdArr, 3, 4));
	ASSERT_EQUALS(spPointGetAxisCoor(E, 3), spKDArrayGetPointVal(kdArr, 3, 5));
	ASSERT_EQUALS(spPointGetAxisCoor(G, 3), spKDArrayGetPointVal(kdArr, 3, 6));

	return true;
}

/*
 * Test search in tree, when split incrementally
 */
bool KDTreeSplitIncremental() {

	SPPoint* points = fillTreePoints();
	SPPoint A = points[0];
	SPPoint B = points[1];
	SPPoint C = points[2];
	SPPoint D = points[3];
	SPPoint E = points[4];
	SPPoint F = points[5];
	SPPoint G = points[6];

	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	SPKDTreeNode* root = spKDTreeInit(kdArr, INCREMENTAL);
	ASSERT_NOT_NULL(root);

	double values[] = { 2, 3, 1, -1 };
	SPPoint H = spPointCreate(values, POINTS_DIM, 7);

	SPBPQueue queue = spKDTreeNearestNeighbor(root, H, 4);
	ASSERT_NOT_NULL(queue);

	ASSERT_TRUE(spBPQueueIsFull(queue));
	SPListElement element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(D), spListElementGetIndex(element));
	spListElementDestroy(element);
	spBPQueueDequeue(queue);
	element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(E), spListElementGetIndex(element));
	spListElementDestroy(element);
	spBPQueueDequeue(queue);
	element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(A), spListElementGetIndex(element));
	spListElementDestroy(element);
	spBPQueueDequeue(queue);
	element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(B), spListElementGetIndex(element));
	spListElementDestroy(element);

	spBPQueueDestroy(queue);
	spKDTreeDestroy(root);
	spPointDestroy(H);

	spKDArrayDestroy(kdArr);
	killTreePoints(points);

	return true;
}

/*
 * Test search in tree, when split by max spread dimension
 */
bool KDTreeSplitMaxSpread() {

	SPPoint* points = fillTreePoints();
	SPPoint A = points[0];
	SPPoint B = points[1];
	SPPoint C = points[2];
	SPPoint D = points[3];
	SPPoint E = points[4];
	SPPoint F = points[5];
	SPPoint G = points[6];

	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	SPKDTreeNode* root = spKDTreeInit(kdArr, MAX_SPREAD);
	ASSERT_NOT_NULL(root);

	double values[] = { 2, 3, 1, -1 };
	SPPoint H = spPointCreate(values, POINTS_DIM, 7);

	SPBPQueue queue = spKDTreeNearestNeighbor(root, H, 4);
	ASSERT_NOT_NULL(queue);

	ASSERT_TRUE(spBPQueueIsFull(queue));
	SPListElement element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(D), spListElementGetIndex(element));
	spListElementDestroy(element);
	spBPQueueDequeue(queue);
	element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(E), spListElementGetIndex(element));
	spListElementDestroy(element);
	spBPQueueDequeue(queue);
	element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(A), spListElementGetIndex(element));
	spListElementDestroy(element);
	spBPQueueDequeue(queue);
	element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(B), spListElementGetIndex(element));
	spListElementDestroy(element);

	spBPQueueDestroy(queue);
	spKDTreeDestroy(root);
	spPointDestroy(H);

	spKDArrayDestroy(kdArr);
	killTreePoints(points);

	return true;
}

/*
 * Test search in tree, when split randomly
 */
bool KDTreeSplitRandom() {

	SPPoint* points = fillTreePoints();
	SPPoint A = points[0];
	SPPoint B = points[1];
	SPPoint C = points[2];
	SPPoint D = points[3];
	SPPoint E = points[4];
	SPPoint F = points[5];
	SPPoint G = points[6];

	SPKDArray* kdArr = spKDArrayInit(points, POINTS_SIZE, POINTS_DIM);

	SPKDTreeNode* root = spKDTreeInit(kdArr, RANDOM);
	ASSERT_NOT_NULL(root);

	double values[] = { 2, 3, 1, -1 };
	SPPoint H = spPointCreate(values, POINTS_DIM, 7);

	SPBPQueue queue = spKDTreeNearestNeighbor(root, H, 4);
	ASSERT_NOT_NULL(queue);

	ASSERT_TRUE(spBPQueueIsFull(queue));
	SPListElement element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(D), spListElementGetIndex(element));
	spListElementDestroy(element);
	spBPQueueDequeue(queue);
	element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(E), spListElementGetIndex(element));
	spListElementDestroy(element);
	spBPQueueDequeue(queue);
	element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(A), spListElementGetIndex(element));
	spListElementDestroy(element);
	spBPQueueDequeue(queue);
	element = spBPQueuePeekLast(queue);
	ASSERT_EQUALS(spPointGetIndex(B), spListElementGetIndex(element));
	spListElementDestroy(element);

	spBPQueueDestroy(queue);
	spKDTreeDestroy(root);
	spPointDestroy(H);

	spKDArrayDestroy(kdArr);
	killTreePoints(points);

	return true;
}

/*
 * main caller to tests of this module
 */
int sp_kd_tree_unit_tests() {
	RUN_TEST(KDArrayForKDTreeTest);
	RUN_TEST(KDTreeSplitIncremental);
	RUN_TEST(KDTreeSplitMaxSpread);
	RUN_TEST(KDTreeSplitRandom);

	return 0;
}

