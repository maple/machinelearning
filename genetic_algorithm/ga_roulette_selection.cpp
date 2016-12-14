#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int calc_fitness(int individual, int length, int **population);
void roulette_selection (int num, int length, int **population, int *fitness);

int main()
{
	int **population;
	int num;
	int length;
	int *fitness;

	int i, j, k;

	num = 10;
	length = 5;

	population = new int * [num];
	for (i=0; i<num; i++){
		population[i] = new int [length];
	}

	fitness = new int[num];

	srand((unsigned) time (NULL));

	for ( i=0; i<num; i++){
		for( j=0; j<length; j++){
			population[i][j] = rand()%2;
			printf("%d", population[i][j]);
		}
		fitness[i] = calc_fitness(i, length, population);
		printf(" fitness = %d\n", fitness[i]);
	}

	for ( k=0; k<10; k++) {
		roulette_selection(num, length, population, fitness);
		for( i=0; i<num; i++){
			fitness[i] = calc_fitness(i, length, population);
			for(j=0; j<length; j++){
				printf("%d", population[i][j]);
			}
			printf(" %d \n", fitness[i]);
		}
	}

	for( i=0; i<num; i++){
		delete []population[i];
	}
	delete []population;
	delete []fitness;
	return 0;
}

int calc_fitness (int individual, int length, int **population){
	int i;
	int fitness = 0;
	for(i=0; i<length; i++){
		fitness = fitness + population[individual][i];
	}
	return fitness;
}

void roulette_selection (int num, int length, int **population, int *fitness)
{
	int i, j, k;
	int sum_fitness;
	int **new_population;
	double *roulette;
	double *ac_roulette;    // accomulation of roulettes.
	double r;               // random number

	new_population = new int*[num];
	for(i=0; i<num; i++){
		new_population[i] = new int[length];
	}

	roulette = new double[num];
	ac_roulette = new double[num];

	sum_fitness = 0;
	for (i=0; i<num; i++){
		sum_fitness = sum_fitness + fitness[i];
	}

	// to make a roulette
	roulette[0] = (double)fitness[0] / (double) sum_fitness;
	ac_roulette[0] = roulette[0];
	int calc_fitness(int individual, int length, int **population);
	printf ("roulette[%d] = %1f  ac_roulette[%d] = %1f \n", 0, roulette[0], 0, ac_roulette[0]);

	for( i= 1; i<num; i++){
		roulette[i] = (double)fitness[i] / (double)sum_fitness;
		ac_roulette[i] = ac_roulette[i-1] + roulette[i];
		printf("roulette[%d] = %1f  ac_roulette[%d] = %1f \n", i, roulette[i], i, ac_roulette[i]);
	}

	// to select with the roulette.
	for (i = 0; i<num; i++){
		r = (double)rand() / (double)RAND_MAX;
		for(j=0; j<num; j++){
			if(r<=ac_roulette[j]){
				printf("r = %1f ac_roulette[%d] = %1f \n", r, j, ac_roulette[j]);
				for(k=0; k<length; k++){
					new_population[i][k] = population[j][k];
				}
				break;
			}
		}
	}

	// to copy to new_population from populaiton.
	for( i=0; i<num; i++){
		for(k=0; k<length; k++){
			population[i][k] = new_population[i][k];
		}
	}

	delete[]roulette;
	delete[]ac_roulette;

	for(i=0; i<num; i++){
		delete []new_population[i];
	}
	delete []new_population;
}
			
