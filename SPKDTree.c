/*
 * SPKDTree.c
 *
 *  Created on: 16 баев„ 2016
 *      Author: user
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "SPKDTree.h"
#include <math.h>
#include <assert.h>

#define NULL_CHECK(val, tree) if (val == NULL) { spKDTreeDestroy(tree); return NULL; }

struct SPKDTreeNode {
	int dim;
	double medianValue;
	SPKDTreeNode* left;
	SPKDTreeNode* right;
	SPPoint leaf;
};

int randomDimension(int max) {
	double scaled = (double) rand() / RAND_MAX;
	return max * scaled;
}

SPKDTreeNode* Init(SPKDArray* kdArr, SplitMethod splitMethod,
		int parentSplittingDimension) {
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

	int splittingDimension = INVALID_DIM;
	int arrayDimension = spKDArrayGetDimension(kdArr);

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
	case UNDEFINED:
		//TODO: such case should not exist. needs to announce error
		break;
	}

	SPKDArray* leftArr = NULL;
	SPKDArray* rightArr = NULL;
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

void neighborSearch(SPKDTreeNode* root, SPBPQueue bpq, SPPoint point) {
	if (root == NULL) {
		return;
	}

	if (root->leaf != NULL) {
		int index = spPointGetIndex(root->leaf);
		double dist = spPointL2SquaredDistance(root->leaf, point);
		SPListElement elem = spListElementCreate(index, dist);
		if (elem == NULL) {
			return;
		}
		spBPQueueEnqueue(bpq, elem);
		return;
	}

	double pointValue = spPointGetAxisCoor(point, root->dim);
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

	double maxVal = spBPQueueMaxValue(bpq);
	double diff = (pointValue - root->medianValue) * (pointValue - root->medianValue);
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

