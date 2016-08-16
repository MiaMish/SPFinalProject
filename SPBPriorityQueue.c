#include <stdlib.h>
#include "SPBPriorityQueue.h"
#include "SPList.h"


struct sp_bp_queue_t{
	int capacity;
	SPList list;
};


SPBPQueue spBPQueueCreate(int maxSize){
	SPBPQueue queue = malloc(sizeof(struct sp_bp_queue_t));
	queue->capacity = maxSize;
	queue->list = spListCreate();
	return queue;
}


SPBPQueue spBPQueueCopy(SPBPQueue source){
	SPBPQueue queue = malloc(sizeof(struct sp_bp_queue_t));
	queue->capacity = source->capacity;
	queue->list = spListCopy(source->list);
	return queue;
}


void spBPQueueDestroy(SPBPQueue source){
	spListDestroy(source->list);
	free(source);
}

void spBPQueueClear(SPBPQueue source){
	spListClear(source->list);
}

int spBPQueueSize(SPBPQueue source){
	return spListGetSize(source->list);
}


int spBPQueueGetMaxSize(SPBPQueue source){
	return source->capacity;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element){
	if (element == NULL || source == NULL || source->list == NULL){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	SPListElement new = spListElementCopy(element);
	if(new == NULL){
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}
	if (source->capacity <= 0){
		return SP_BPQUEUE_FULL;
	}
	SPListElement node = spListGetFirst(source->list);

	while(1){
		if (node == NULL){
			spListInsertLast(source->list, new);
			break;
		}
		if (spListElementCompare(node, new) > 0){
			spListInsertBeforeCurrent(source->list, new);
			break;
		}
		else{
			node = spListGetNext(source->list);
		}
	}

	if (spListGetSize(source->list) > source->capacity){
		int cnt = 0;
		node = spListGetFirst(source->list);
		while(cnt < spListGetSize(source->list) - 1){
			node = spListGetNext(source->list);
			cnt++;
		}
		spListRemoveCurrent(source->list);
		return SP_BPQUEUE_FULL;
	}
	else{
		return SP_BPQUEUE_SUCCESS;
	}


}


SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source){

	if (spListGetSize(source->list) == 0){
		return SP_BPQUEUE_EMPTY;
	}

	int cnt = 0;
	SPListElement node = spListGetFirst(source->list);
	while(cnt < spListGetSize(source->list) - 1){
		node = spListGetNext(source->list);
		if (node == NULL) {
			break;
		}
		cnt++;
	}
	spListRemoveCurrent(source->list);
	return SP_BPQUEUE_SUCCESS;
}


SPListElement spBPQueuePeek(SPBPQueue source){
	SPListElement new = spListElementCopy(spListGetFirst(source->list));
	return new;
}


SPListElement spBPQueuePeekLast(SPBPQueue source){
	int cnt = 0;
	SPListElement node = spListGetFirst(source->list);
	while(cnt < spListGetSize(source->list) - 1){
		node = spListGetNext(source->list);
		cnt++;
	}
	SPListElement new = spListElementCopy(node);
	return new;
}


double spBPQueueMinValue(SPBPQueue source){
	return (spListElementGetValue(spListGetFirst(source->list)));
}


double spBPQueueMaxValue(SPBPQueue source){
	int cnt = 0;
	SPListElement node = spListGetFirst(source->list);
	while(cnt < spListGetSize(source->list) - 1){
		node = spListGetNext(source->list);
		cnt++;
	}
	return spListElementGetValue(node);
}


bool spBPQueueIsEmpty(SPBPQueue source){
	return (spListGetSize(source->list) == 0);
}


bool spBPQueueIsFull(SPBPQueue source){
	return (spListGetSize(source->list) == source->capacity);
}
