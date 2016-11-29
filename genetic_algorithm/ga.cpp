#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
  reference : a book : Robot Intelligence
  how to compile: $ g++ -Wall filename.cpp -o executablename
 */

int main()
{
	int **population;    // 個体集合
	int num;             // 個体数
	int length;          // 遺伝子長
	int i, j;

	num = 10;
	length = 5;

	population = new int*[num];
	for(i=0 ; i < num ; i ++){
		population[i] = new int[length];
	}

	// initiate random function
	srand ((unsigned) time (NULL));

	for ( i=0; i<num; i++ ){
		for ( j = 0; j < length; j++ ){
			population[i][j] = rand() % 2;
			printf ("%d", population[i][j]);
		}
		printf("\n");
	}
	for ( i = 0; i < num ; i++ ){
		delete []population[i];
	}
	delete []population;

	return 0;
}
