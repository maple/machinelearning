#include "stdio.h"
#include "stdlib.h"
#include "time.h"

// 適合性 : 首の長さ、1リンクあたり10cm

int calc_fitness (int indivisual, int length, int **population);

int main()
{
	int **population;
	int num;
	int length;
	int *fitness;

	int i,j;

	num = 10;
	length = 5;

	population = new int*[num];
	for ( i=0; i<num; i++){
		population[i] = new int [length];
	}

	fitness = new int [num];
	srand((unsigned)time(NULL));

	for (i = 0; i<num; i++) {
		for (j = 0; j<length; j++){
			population[i][j] = rand()%2;
			printf("%d", population[i][j]);
		}

		fitness[i] = calc_fitness(i, length, population);
		printf( " fitness = %d\n", fitness[i]);
	}

	for( i=0; i<num; i++ ){
		delete [] population [i];
	}
	delete []population;
	delete []fitness;

	return 0;
}

int calc_fitness(int individual, int length, int **population) {
	int i;
	int fitness = 0;
	for( i=0; i<length; i++){
		fitness = fitness + population[individual][i];
	}
	return 10 * fitness;
}
