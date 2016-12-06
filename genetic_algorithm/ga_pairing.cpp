#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void pairing (int num, int length, int **population, int **pair);

int main()
{
	int **population;
	int **pair;
	int num;
	int length;
	int i, j;

	num = 10;
	length = 5;

	population = new int * [num];
	pair = new int*[num];
	for (i = 0; i<num ; i++){
		population[i] = new int [length];
		pair[i] = new int [length];
	}

	srand((unsigned) time(NULL));

	for( i=0; i<num; i++){
		for(j=0; j<length; j++){
			population[i][j] = rand() %2;
			printf("%d", population[i][j]);
		}
		printf("\n");
	}

	pairing (num, length, population, pair);

	for (i=0; i<num; i++){
		delete []population[i];
		delete []pair [i];
	}
	delete []population;
	delete []pair;
	return 0;
}


void pairing(int num, int length, int **population, int **pair){
	int *shuffle;
	int r;
	int i, j;
	int temp;

	shuffle = new int[num];

	for( i=0; i<num; i++){
		shuffle[i] = i;
	}

	for( i=0; i<num; i++){
		r = rand() % num;
		temp = shuffle[r];
		shuffle[r] = shuffle[i];
		shuffle[i] = temp;
	}

	for ( i=0; i<num; i++){
		printf ("shuffle[%d] = %d\n", i, shuffle[i]);
	}

	for( i=0; i<num; i++){
		for( j=0; j<length; j++){
			pair[i][j] = population[shuffle[i]][j];
		}
	}

	for(i = 0; i<num; i++){
		for(j=0; j<length; j++){
			printf("%d", population[i][j]);
		}
		printf(">>>");
		for(j = 0; j<length; j++){
			printf("%d", pair[i][j]);
		}
		printf("\n");
	}
}

		
	
