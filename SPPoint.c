#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "SPPoint.h"

struct sp_point_t {
	double* coordinates;
	int dimension;
	int index;
};

SPPoint spPointCreate(double* data, int dim, int index) {
	struct sp_point_t *point;
	int i;
	if (dim <= 0 || data == NULL || index < 0) {
		return NULL;
	}

	point = (struct sp_point_t*) malloc(sizeof (struct sp_point_t));
	if (point == NULL) {
		return NULL;
	}
	point->coordinates = (double*)malloc(sizeof(double)*dim);
	if (point->coordinates == NULL) {
		free(point);
	}
	for(i = 0; i < dim; i++) {
		point->coordinates[i] = data[i];
	}
	point->dimension = dim;
	point->index = index;

	return point;
}

SPPoint spPointCopy(SPPoint source) {
	double* data;
	int dim;
	int index;

	//ASSERT_NULL(source, "ASSERTION FAILED :: source is NULL");
	assert(source != NULL);

	data = source->coordinates;
	dim = source->dimension;
	index = source->index;

	return spPointCreate(data, dim, index);
}

void spPointDestroy(SPPoint point) {
	if (point != NULL) {
		free(point->coordinates);
		free(point);
	}
}

/*
 * c is stupid
 */
int spPointGetDimension(SPPoint point) {
	//ASSERT_NULL(point, "ASSERTION FAILED :: point is NULL");
	assert(point != NULL);
	return point->dimension;
}


int spPointGetIndex(SPPoint point) {
	assert(point != NULL);
	return point->index;
}

double spPointGetAxisCoor(SPPoint point, int axis) {
	assert(point != NULL);
	assert(axis < point->dimension);

	return point->coordinates[axis];
}

double spPointL2SquaredDistance(SPPoint p, SPPoint q) {
	int distance = 0;
	double* pData;
	double* qData;
	int diff;
	int i;

	assert(p != NULL);
	assert(q != NULL);
	assert(p->dimension == q->dimension);

	pData = p->coordinates;
	qData = q->coordinates;
	for (i = 0; i < p->dimension; i++) {
		diff = pData[i] - qData[i];
		distance += diff*diff;
		diff = 0;
	}
	return distance;
}

