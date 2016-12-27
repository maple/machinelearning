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
	pair = new int*[num];
	for ( i=0; i<num; i++){
		population[i] = new int [length];
		pair[i] = new int [length];
	}

	fitness = new int[num];

	srand((unsigned) time(NULL));

	for(i = 0; i<num; i++) {
		for(j=0; j<length; j++){
			population[i][j] = rand()%2;
			printf("%d", population[i][j]);
		}
		printf("\n");
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
			printf("  fitness = %d\n", fitness[i]);
		}
		if (average_fitness(num, fitness) > 45){
			printf("The program is finished since target fitness value has reached\n");
			break;
		}

		roulette_selection(num, length, population, fitness);
	}


	for ( i=0; i<num; i++){
		delete []population[i];
		delete []pair[i];
	}
	delete []population;
	delete []pair;
	delete []fitness;

	return 0;
}

int calc_fitness ( int individual, int length, int **population){
	int i;
	int fitness = 0;
	for( i=0; i<length; i++){
		fitness = fitness + population [individual][i];
	}
	return 10 * fitness;
}

void roulette_selection(int num, int length, int **population, int *fitness){
	int i, j, k;
	int sum_fitness;
	int **new_population;
	double *roulette;
	double *ac_roulette;
	double r;

	new_population = new int * [num];
	for( i=0; i<num; i++){
		new_population[i] = new int [length];
	}
	roulette = new double[num];
	ac_roulette = new double[num];

	sum_fitness = 0;
	for ( i=0; i< num; i++){
		sum_fitness = sum_fitness + fitness [i];
	}
	roulette[0] = (double)fitness[0] / (double)sum_fitness;
	ac_roulette[0] = roulette[0];

	for( i=1; i<num; i++){
		roulette[i] = (double)fitness[i] / (double) sum_fitness;
		ac_roulette[i] = ac_roulette[i-1] + roulette[i];
	}

	for( i=0; i<num; i++){
		r = (double)rand() / (double) RAND_MAX;
		for( j=0; j<num; j++){
			if( r<=ac_roulette[j]){
				for (k=0; k<length; k++){
					new_population[i][k] = population[j][k];
				}
				break;
			}
		}
	}

	for( i=0; i<num; i++) {
		for ( k=0; k<length; k++){
			population[i][k] = new_population[i][k];
		}
	}

	delete []roulette;
	delete []ac_roulette;

	for( i=0; i<num; i++){
		delete []new_population[i];
	}
	delete []new_population;

}

void one_point_crossover(int crossover_rate, int mutation_rate, int num, int length, int **population, int **pair){
	int i, j;
	int cross_point;

	for ( i=0; i<(num-1) ; i=i+2 ){
		if(rand() % 100<crossover_rate){
			cross_point = rand()%(length-1) + 1;
			// printf("cross_point = %d \n", cross_point);
			for(j=0; j<cross_point; j++){
				population[i][j] = pair[i+1][j];
				population[i+1][j] = pair[i][j];
			}
			for( j=cross_point; j<length; j++){
				population[i][j] = pair[i][j];
				population[i+1][j] = pair[i+1][j];
			}

			mutation(mutation_rate, length, i, population);
			mutation(mutation_rate, length, i+1, population);
		}else {
			// do NOT apply cross_point
			for( j=0; j<length; j++){
				population[i][j] = pair[i][j];
				population[i+1][j] = pair[i+1][j];
			}
		}
	}
}

void mutation (int mutation_rate, int length, int individual, int **population){
	int i;

	for( i=0; i<length; i++){
		if(mutation_rate > rand() % 100){
			population[individual][i] = !population[individual][i];
		}
	}
}

void pairing (int num, int length, int **population, int **pair){
	int *shuffle;
	int r;
	int i, j;
	int temp;

	shuffle = new int[num];
	for( i=0; i<num; i++){
		shuffle[i] = i;
	}
	for( i=0; i<num; i++){
		r = rand()%num;
		temp=shuffle[r];
		shuffle[r] = shuffle[i];
		shuffle[i] = temp;
	}

	// // check the value after shuffle
	// for (i=0; i<num; i++){
	// 	printf("shuffle[%d]=%d\n", i, shuffle[i]);
	// }

	for( i=0; i<num; i++){
		for(j=0; j<length; j++){
			pair[i][j] = population[shuffle[i]][j];
		}
	}

	// // to check values for debug.
	// for(i = 0; i<num; i++){
	// 	for( j=0; j<length; j++){
	// 		printf("%d", population[i][j]):
	// 	}
	// 	printf(">>>");
	// 	for( j=0; j<length; j++){
	// 		printf("%d", pair[i][j]);
	// 	}
	// 	printf("\n");
	// }
	
}

	
double average_fitness(int num, int * fitness){
	int i;
	int sum = 0;

	for( i=0; i<num; i++){
		sum = sum + fitness[i];
	}
	return (double)sum /(double)num;
}

