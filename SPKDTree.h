/*
 * SPKDTree.h
 *
 *  Created on: 16 баев„ 2016
 *      Author: user
 */

#ifndef SPKDTREE_H_
#define SPKDTREE_H_

#include "SPKDArray.h"
#include "SPBPriorityQueue.h"
#include "SPConfigUtils.h"

#define INVALID_DIM -1
#define INVALID_VAL -1

/*
 * A struct to represent a kd-tree data structure
 */
struct SPKDTreeNode;
typedef struct SPKDTreeNode SPKDTreeNode;

/*
 * @param kdArr - a kd-array
 * @param splitMethod - a method by which to split kd-arrays
 *
 * The function is creating a new kd-tree by splitting the given kd-array
 *
 * @return NULL on any failure
 * @return a new kd-tree otherwise
 */
SPKDTreeNode* spKDTreeInit(SPKDArray* kdArr, SplitMethod splitMethod);

/*
 * @param root - the root of a kd-tree
 *
 * The function destroys a given kd-tree
 *
 */
void spKDTreeDestroy(SPKDTreeNode* root);

/*
 * @param root - the root of a kd-tree
 * @param testPoint - a point for which to search neighbors
 * @param neighborsCount - the number of neighbors to search for
 *
 * The function is performing a nearest-neighbor search on the given tree
 *
 * @return a priority queue stuffed with the nearest points found, represented by index and distance
 *
 */
SPBPQueue spKDTreeNearestNeighbor(SPKDTreeNode* root, SPPoint testPoint, int neighborsCount);

#endif /* SPKDTREE_H_ */
