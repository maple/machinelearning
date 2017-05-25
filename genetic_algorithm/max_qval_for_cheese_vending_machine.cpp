#include <stdio.h>

double max_Qval (int s, int num_a, double** Qtable);

int main()
{
	double **Qtable;
	int num_a, num_s;
	int i, j;
	double max;

	num_a = 5;
	num_s = 10;

	Qtable = new double *[num_s];
	for(i=0; i<num_s; i++){
		Qtable[i] = new double[num_a];
	}

	for(i=0; i<num_s; i++){
		for(j=0; j<num_a; j++){
			Qtable[i][j] = 0;
			printf("Q[%d][%d] = %lf\n", i, j, Qtable[i][j]);
		}
	}

	// set Q value for test.  Max = 9.
	Qtable[3][2] = 9;
	Qtable[3][4] = 6;
	max = max_Qval (3, num_a, Qtable);
	printf("max = %lf\n", max);

	for(i = 0; i < num_s; i++) {
		delete []Qtable[i];
	}
	delete []Qtable;

	return 0;
}

double max_Qval (int s, int num_a, double ** Qtable){
	double max;
	int i=0;

	max = Qtable[s][0];
	for(i=1; i<num_a; i++){
		if(Qtable[s][i] > max){
			max = Qtable[s][i];
		}
	}
	return max;
}

