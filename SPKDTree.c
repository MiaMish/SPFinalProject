/*
 * SPKDTree.c
 *
 *  Created on: 16 áàåâ× 2016
 *      Author: user
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "SPKDTree.h"
#include <math.h>
#include <assert.h>

/*
 * macro to check allocation failures
 */
#define NULL_CHECK(val, tree) if (val == NULL) { spKDTreeDestroy(tree); return NULL; }

struct SPKDTreeNode {
	int dim;
	double medianValue;
	SPKDTreeNode* left;
	SPKDTreeNode* right;
	SPPoint leaf;
};

/*
 * Helper function to get a random dimension
 */
int randomDimension(int max) {
	double scaled = (double) rand() / RAND_MAX;
	return max * scaled;
}

/*
 * Helper function to initialize a kd-tree
 */
SPKDTreeNode* Init(SPKDArray* kdArr, SplitMethod splitMethod,
		int parentSplittingDimension) {
	int splittingDimension, arrayDimension;
	SPKDArray* leftArr = NULL;
	SPKDArray* rightArr = NULL;
	
	SPKDTreeNode* root = (SPKDTreeNode*) malloc(sizeof(SPKDTreeNode));
	NULL_CHECK(root, root);

	if (spKDArrayGetPointsCount(kdArr) == 1) {
		root->dim = INVALID_DIM;
		root->medianValue = INVALID_VAL;
		root->left = NULL;
		root->right = NULL;
		root->leaf = spPointCopy(spKDArrayGetPointAt(kdArr, 0));
		NULL_CHECK(root->leaf, root);
		return root;
	}

	splittingDimension = INVALID_DIM;
	arrayDimension = spKDArrayGetDimension(kdArr);

	switch (splitMethod) {
	case MAX_SPREAD:
		splittingDimension = spKDArrayFindMaxSpreadDimension(kdArr);
		assert(splittingDimension < arrayDimension);
		break;

	case RANDOM:
		splittingDimension = randomDimension(arrayDimension);
		assert(splittingDimension < arrayDimension);
		break;

	case INCREMENTAL:
		splittingDimension = (parentSplittingDimension + 1) % arrayDimension;
		break;
	}

	spKDArraySplit(kdArr, splittingDimension, &leftArr, &rightArr);
	NULL_CHECK(leftArr, root);
	NULL_CHECK(rightArr, root);

	root->dim = splittingDimension;
	root->medianValue = spKDArrayGetMedian(kdArr, splittingDimension);
	root->left = Init(leftArr, splitMethod, splittingDimension);
	NULL_CHECK(root->left, root);
	root->right = Init(rightArr, splitMethod, splittingDimension);
	NULL_CHECK(root->right, root);
	root->leaf = NULL;

	spKDArrayDestroy(leftArr);
	spKDArrayDestroy(rightArr);

	return root;
}

SPKDTreeNode* spKDTreeInit(SPKDArray* kdArr, SplitMethod splitMethod) {
	return Init(kdArr, splitMethod, -1);
}

void spKDTreeDestroy(SPKDTreeNode* root) {
	if (root == NULL) {
		return;
	}
	spPointDestroy(root->leaf);
	spKDTreeDestroy(root->left);
	spKDTreeDestroy(root->right);
	free(root);
}

/*
 * Helper function to perform neighbor search
 */
void neighborSearch(SPKDTreeNode* root, SPBPQueue bpq, SPPoint point) {
	int index;
	double dist, pointValue, maxVal, diff;
	
	if (root == NULL) {
		return;
	}

	if (root->leaf != NULL) {
		index = spPointGetIndex(root->leaf);
		dist = spPointL2SquaredDistance(root->leaf, point);
		SPListElement elem = spListElementCreate(index, dist);
		if (elem == NULL) {
			return;
		}
		spBPQueueEnqueue(bpq, elem);
		spListElementDestroy(elem);
		return;
	}

	pointValue = spPointGetAxisCoor(point, root->dim);
	SPKDTreeNode* firstToSearch = NULL;
	SPKDTreeNode* secondToSearch = NULL;
	if (pointValue <= root->medianValue) {
		firstToSearch = root->left;
		secondToSearch = root->right;
	} else {
		firstToSearch = root->right;
		secondToSearch = root->left;
	}

	neighborSearch(firstToSearch, bpq, point);

	maxVal = spBPQueueMaxValue(bpq);
	diff = (pointValue - root->medianValue) * (pointValue - root->medianValue);
	if (!spBPQueueIsFull(bpq) || diff < maxVal) {
		neighborSearch(secondToSearch, bpq, point);
	}
}

SPBPQueue spKDTreeNearestNeighbor(SPKDTreeNode* root, SPPoint testPoint,
		int neighborsCount) {
	SPBPQueue bpq = spBPQueueCreate(neighborsCount);
	if (bpq == NULL) {
		return NULL;
	}

	neighborSearch(root, bpq, testPoint);
	return bpq;
}

