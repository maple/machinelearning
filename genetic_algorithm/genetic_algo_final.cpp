#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int calc_fitness(int individual, int length, int **population);
void roulette_selection(int num, int length, int **population, int *fitness);
void one_point_crossover (int crossover_rate, int mutation_rate, int num, int length, int **population, int **pair);
void pairing( int num, int length, int **population, int **pair );
void mutation (int mutation_rate, int length, int individual, int **population);
double average_fitness (int num, int *fitness);

int main()
{
	int **population;
	int **pair;
	int num;
	int length;
	int *fitness;
	int generation;
	int max_generation;
	int crossover_rate;
	int mutation_rate;
	int i, j;

	num = 10;
	length = 5;
	max_generation = 20;
	crossover_rate = 50;
	mutation_rate = 5;

	population = new int *[num];
	for ( i=0; i<num; i++){
		pair[i] = new int [length];
	}

	fitness = new int[num];

	srand((unsigned) time(NULL));

	for(i = 0; i<num; i++) {
		for(j=0; j<length; j++){
			population[i][j] = rand()%2;
			printf("%d", population[i][j]);
		}
	}

	for (generation = 0; generation < max_generation; generation++){
		printf ("generation = %d\n", generation);
		pairing(num, length, population, pair);
		one_point_crossover(crossover_rate, mutation_rate, num, length, population, pair);
		for (i=0; i<num; i++){
			fitness[i] = calc_fitness (i, length, population);
			for(j=0; j<length; j++){
				printf ("%d", population[i][j]);
			}
			printf("fitness = %d\n", fitness[i]);
		}
		if (average_fitness(num, fitness) > 45){
			printf("The program is finished since target fitness value has reached\n");
			break;
		}

		roulette_selection(num, length, population, fitness);
	}

	
