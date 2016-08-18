/*
 * SPKDTree.c
 *
 *  Created on: 16 баев„ 2016
 *      Author: user
 */

#include "stdio.h"
#include "stdlib.h"
#include "limits.h"
#include "SPKDTree.h"

#define NULL_CHECK(val, tree) if (val == NULL) { SPKDTreeDestroy(tree); return NULL; }

struct SPKDTreeNode {
	int dim;
	double medianValue;
	SPKDTreeNode* left;
	SPKDTreeNode* right;
	SPPoint leaf;
};

inline int randomDimension(int max)
{
       double scaled = (double)rand()/RAND_MAX;
       return max*scaled + 1;
}

SPKDTreeNode* Init(SPKDArray* kdArr, spKDTreeSplitMethodEnum splitMethod, int parentSplittingDimension) {
	SPKDTreeNode* root = (SPKDTreeNode*) malloc(sizeof(SPKDTreeNode));
	NULL_CHECK(root, root);

	if (SPKDArrayGetPointsCount(kdArr) == 1) {
		root->dim = INVALID_DIM;
		root->medianValue = INVALID_VAL;
		root->left = NULL;
		root->right = NULL;
		root->leaf = spPointCopy(SPKDArrayGetPointAt(kdArr, 0));
		NULL_CHECK(root->leaf, root);
		return root;
	}

	int splittingDimension = INVALID_DIM;
	int arrayDimension = SPKDArrayGetDimension(kdArr);

	switch (splitMethod) {
		case MAX_SPREAD:
			splittingDimension = SPKDArrayFindMaxSpreadDimension(kdArr);
			break;
		case RANDOM:
			splittingDimension = randomDimension(arrayDimension);
			break;

		case INCREMENTAL:
			splittingDimension = (parentSplittingDimension + 1) % arrayDimension;
			break;
	}

	SPKDArray* leftArr = NULL;
	SPKDArray* rightArr = NULL;
	SPKDArraySplit(kdArr, splittingDimension, &leftArr, &rightArr);
	NULL_CHECK(leftArr, root);
	NULL_CHECK(rightArr, root);

	root->dim = splittingDimension;
	root->medianValue = SPKDArrayGetMedian(kdArr, splittingDimension);
	root->left = Init(leftArr, splitMethod, splittingDimension);
	NULL_CHECK(root->left, root);
	root->right = Init(rightArr, splitMethod, splittingDimension);
	NULL_CHECK(root->right, root);
	root->leaf = NULL;

	return root;
}

SPKDTreeNode* SPKDTreeInit(SPKDArray* kdArr, spKDTreeSplitMethodEnum splitMethod) {
	return Init(kdArr, splitMethod, 0);
}

void SPKDTreeDestroy(SPKDTreeNode* root) {
	if (root == NULL) {
		return;
	}
	spPointDestroy(root->leaf);
	SPKDTreeDestroy(root->left);
	SPKDTreeDestroy(root->right);
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
	}
	else {
		firstToSearch = root->right;
		secondToSearch = root->left;
	}

	neighborSearch(firstToSearch, bpq, point);

	if (spBPQueueIsFull(bpq)) {
		return;
	}

	double maxVal = spBPQueueMaxValue(bpq);
	double diff = abs(pointValue - root->medianValue);
	if (diff < maxVal) {
		neighborSearch(secondToSearch, bpq, point);
	}
}

SPBPQueue SPKDTreeNearestNeighbor(SPKDTreeNode* root, SPPoint testPoint, int neighborsCount, int spKNN) {
	SPBPQueue bpq = spBPQueueCreate(spKNN);
	if (bpq == NULL) {
		return NULL;
	}

	neighborSearch(root, bpq, testPoint);
	return bpq;
}

