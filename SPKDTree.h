/*
 * SPKDTree.h
 *
 *  Created on: 16 ����� 2016
 *      Author: user
 */

#ifndef SPKDTREE_H_
#define SPKDTREE_H_

#include "SPKDArray.h"
#include "SPBPriorityQueue.h"
#include "SPConfigUtils.h"

#define INVALID_DIM -1
#define INVALID_VAL -1

struct SPKDTreeNode;
typedef struct SPKDTreeNode SPKDTreeNode;

SPKDTreeNode* spKDTreeInit(SPKDArray* kdArr, SplitMethod splitMethod);

void spKDTreeDestroy(SPKDTreeNode* root);

SPBPQueue spKDTreeNearestNeighbor(SPKDTreeNode* root, SPPoint testPoint, int neighborsCount, int spKNN);

#endif /* SPKDTREE_H_ */
