#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "map.h"

#define STRESS_TEST_SIZE 100000

bool testKeyAccessPolicy(PtMap map);
void testStress(PtMap map);
int* createSequentialShuffledArray(int size);

int main() {

	PtMap map = mapCreate();

	bool passed = testKeyAccessPolicy(map);

	if(passed) {
		testStress(map);
	}

	mapDestroy(&map);	
	
	return EXIT_SUCCESS;
}

bool testKeyAccessPolicy(PtMap map) {
	if( map == NULL) return false;

	mapClear(map);

	/* We just want to test if mappings are saved/retrived correctly */

	int keys[] = {1,4,7,2,8,5,19};
	int values[] = {0,1,2,3,4,5,6};

	printf("--- Testing key access based policy --- \n");

	/* from an empty map... */
	bool passed = true;
	int ret;
	bool exists;
	for(int i=0; i<sizeof(keys)/sizeof(int); i++) {
		exists = mapContains(map, keys[i]);
		
		if(exists) {
			printf("mapContains failed for an empty map.\n");
			passed = false;
			break;
		}
		else if(mapRemove(map, keys[i], &ret) != MAP_EMPTY) {
			printf("mapRemoved failed for an empty map (expected MAP_EMPTY).\n");
			passed = false;
			break;
		}
	}
	printf("mapContains and mapRemove (empty map)...");
	if(passed) {
		printf("[OK]\n");
	} else {
		printf("[FAILED]\n");
		return false;
	}

	/* insert mappings */
	for(int i=0; i<sizeof(keys)/sizeof(int); i++) {
		if( mapPut(map, keys[i], values[i]) != MAP_OK ) {
			printf("Put key/value pair... [FAILED] \n");
			return false;
		}
	}

	int size;
	mapSize(map, &size);

	printf("mapSize...");
	if( size == sizeof(keys)/sizeof(int)) {
		printf("[OK]\n");
	} else {
		printf("[FAILED]\n");
		return false;
	}

	passed = true;
	for(int i=0; i<size; i++) {
		if( !mapContains(map, keys[i]) ) {
			printf("mapContains for existing key... [FAILED] \n");
			passed = false;
			break;
		}

		if( mapGet(map, keys[i], &ret) != MAP_OK || ret != values[i] ) {
			printf("mapGet for existing key... [FAILED] \n");
			passed = false;
			break;
		}
	}

	printf("mapPut, mapContains and mapRemove...");
	if(passed) {
		printf("[OK]\n");
	} else {
		printf("[FAILED]\n");
		return false;
	}

	/* test put while replacing value */
	values[0] = 100;
	mapPut(map, keys[0], values[0]);
	mapGet(map, keys[0], &ret);

	if(ret != values[0]) {
		printf("mapPut, mapGet (replace value)... [FAILED] \n");
		return false;
	}

	mapPrint(map);

	/* test removal of mappings */
	for(int i=0; i<sizeof(keys)/sizeof(int); i++) {
		mapRemove(map, keys[i], &ret);
		if(ret != values[i]) {
			printf("mapRemove to return correct value... [FAILED] \n");
			return false;
		}

		if(mapContains(map, keys[i])) {
			printf("mapContains while passing removed key... [FAILED] \n");
			return false;
		}
	}

	mapSize(map, &size);

	printf("mapSize...");
	if( size == 0) {
		printf("[OK]\n");
	} else {
		printf("[FAILED]\n");
		return false;
	}

	mapPrint(map);

	printf("ALL tests passed successfully! \n");

	return true;
}

int* createSequentialShuffledArray(int size) {
	int *arr = (int*)malloc(size * sizeof(int));

	for(int i=0; i<size; i++) {
		arr[i] = i;
	}

	/* shuffle */
	for(int i=0; i<size/2; i++) {
		int index1 = rand() % size;
		int index2 = rand() % size;

		int aux = arr[index1];
		arr[index1] = arr[index2];
		arr[index2] = aux;
	}

	return arr;
}

void testStress(PtMap map) {
	if( map == NULL) return;

	srand(123456789); /* same results for implementation comparison*/

	mapClear(map);

	int *testArr = createSequentialShuffledArray(STRESS_TEST_SIZE);

	printf("--- Stress testing with %d mappings --- \n", STRESS_TEST_SIZE);

	/* the mappings don't matter, just the performance */
	clock_t start, end;
    double cpu_time_used;

	/* put mappings */
	start = clock();
	for(int i=0; i<STRESS_TEST_SIZE; i++) {
		mapPut(map, testArr[i], testArr[i]);
	}
	end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("-- Time taken for mapPut: %lf seconds \n", cpu_time_used);

	/* search mappings, via contains, with unordered array */
	start = clock();
	for(int i=0; i<STRESS_TEST_SIZE; i++) {
		mapContains(map, i);
	}
	end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    
    printf("-- Time taken for mapContains (search only): %lf seconds \n", cpu_time_used);

	free(testArr);	
}