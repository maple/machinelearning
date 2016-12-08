#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void mutation (int mutation_rate, int length, int individual, int **population);

int main()
{
	int **population;
	int num;
	int length;
	int mutation_rate;
	int i, j;

	num = 10;
	length = 5;
	mutation_rate = 50;

	population = new int*[num];
	for (i=0; i<num; i++){
		population[i] = new int[length];
	}


	srand((unsigned) time (NULL));

	printf("mutation_rate is %d \n", mutation_rate);
	printf("population = \n");
	for( i=0; i<num; i++) {
		for( j=0; j<length; j++){
			population[i][j] = i %2;
			printf("%d", population[i][j]);
		}printf(" ");
	}

	for( i=0; i<num; i++){
		mutation (mutation_rate, length, i, population);
	}

	printf("\npopulation = \n");
	for( i=0; i<num; i++){
		for( j=0; j<length; j++){
			printf("%d", population[i][j]);
		}
		printf(" ");
	}
	printf("\n");

	for( i=0; i<num; i++){
		delete []population[i];
	}
	delete []population;
	return 0;
}

void mutation(int mutation_rate, int length, int individual, int **population){
	int i;

	for( i=0; i<length; i++){
		if(mutation_rate > rand()%100){
			population[individual][i] =! population[individual][i];
		}
	}
}

