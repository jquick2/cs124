#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>

// RNG function used
float cust_rand(int max_val)
{
	float unscaled = RAND_MAX;
	while (unscaled == RAND_MAX) {
		unscaled = rand();
	}
    return max_val * unscaled / (float) RAND_MAX ;
}

void get_max(int64_t array[], int size_of_array, int64_t* max, int64_t* nextmax, int* maxpos, int *nextmaxpos) {
	int	i;
	
	*maxpos = *nextmaxpos = -1;
	*max = *nextmax = 0;
	for (i = 0; i < size_of_array; i++) {
		if (array[i] > *max) {
			*nextmax = *max;
			*nextmaxpos = *maxpos;
			*max = array[i];
			*maxpos = i;
		} else if (array[i] > *nextmax) {
			*nextmax = array[i];
			*nextmaxpos = i;
		}
	}
}

// Standard KK Implementation
int karmarkarKarp(int64_t array[], int size_int) {
	int64_t max, nextmax;
	int maxpos, nextmaxpos;
	while (true) {
		get_max(array, size_int, &max, &nextmax, &maxpos, &nextmaxpos);

		// Only one non-zero elt left, thus done
		if (nextmax == 0) {
			break;
		}

		array[nextmaxpos] = 0;
		array[maxpos] = llabs(max - nextmax);;
	}

	return max;
}

// Non-prepartition
void randomize_S(int* S) {
	// generate random S array
	for (int i = 0; i < 100; i++) {
		if ((rand() % 2) == 0) {
			S[i] = -1;
		} 
		else {
			S[i] = 1;
		}
	}
}

// Prepartition
void randomize_P(int* P) {
	// generate random S array -- if there's any trouble 
	// later on, this hasn't been fully checked but assumed 
	// to give [1,100] integral range
	for (int i = 0; i < 100; i++) {
		P[i] = (int) floor(cust_rand(100));
	}
}

// Abstracts out for each prepart case
void randomize_array(int* arr, bool prepart) {
	if (prepart == true) {
		randomize_P(arr);
	}

	else {
		randomize_S(arr);
	}
}

// Abstracts out for each prepart case
int64_t calc_residue(int64_t* arr, int* soln, bool prepart) {
	int64_t residue = 0;
	if (prepart == true) {
		int64_t aPrime[100];
		for (int j = 0; j < 100; j++) {
			aPrime[j] = 0;
		}

		for (int j = 0; j < 100; j++) {
			aPrime[soln[j]] += arr[j];
		}

		residue = karmarkarKarp(aPrime,100);
	}

	else {
		for (int j = 0; j < 100; j++) {
			residue += soln[j]*arr[j];
		}
		residue = llabs(residue);
	}
	return residue;
}

// Abstracts out for each prepart case
void random_move(int* rand_soln, int* new_soln, bool prepart) {
	int i, j;
	i = (rand() % 100);
	// j must not be i
	while (true) {
		j = (rand() % 100);
		if (j != i) {
			break;
		}
	}
	if (prepart == true) {
		// Prepart random move simpler, just set i to j in new
		new_soln[i] = j;
	}

	else {
		// random move; looks a little different here
		// but this is just a way to do it in place
		new_soln[i] = rand_soln[i] * -1;
		if ((rand() % 2) == 0) {
			new_soln[j] = rand_soln[j] * -1;
		}
	}
}

// Standard Repeated Random
int64_t repeated_random(int64_t* arr, int num_iters, bool prepart) {
	int* rand_soln = malloc(sizeof(int) * 100);
	int64_t	residue, new_residue;

	randomize_array(rand_soln, prepart);
	residue = calc_residue(arr, rand_soln, prepart);

	for (int it = 0; it < num_iters; it++) {
		randomize_array(rand_soln, prepart);

		new_residue = calc_residue(arr, rand_soln, prepart);
		if (new_residue < residue) {
			residue = new_residue;
		}
	}

	free(rand_soln);
	return residue;
}

int64_t hill_climbing(int64_t*arr, int num_iters, bool prepart) {
	int* rand_soln = malloc(sizeof(int) * 100);
	int* temp_soln = malloc(sizeof(int) * 100);
	int64_t residue, new_residue;

	randomize_array(rand_soln, prepart);
	memcpy(temp_soln, rand_soln, sizeof(int)*100);
	residue = calc_residue(arr, rand_soln, prepart);

	for (int it = 0; it < num_iters; it++) {
		random_move(rand_soln,temp_soln,prepart);

		new_residue = calc_residue(arr, temp_soln, prepart);
		if (new_residue < residue) {
			residue = new_residue;
			memcpy(rand_soln, temp_soln, sizeof(int)*100);
		}
	}

	free(rand_soln);
	free(temp_soln);
	return residue;
}

// Simulated annealing algorithm
int64_t simulated_annealing(int64_t*arr, int num_iters, bool prepart) {
	int* rand_soln = malloc(sizeof(int) * 100);
	int* temp_soln = malloc(sizeof(int) * 100);
	int64_t residue, new_residue;
	double prob;
	int min_res = INT_MAX;

	randomize_array(rand_soln, prepart);
	memcpy(temp_soln, rand_soln, sizeof(int)*100);
	residue = calc_residue(arr, rand_soln, prepart);
	for (int it = 0; it < num_iters; it++) {
		random_move(rand_soln,temp_soln,prepart);

		new_residue = calc_residue(arr, temp_soln, prepart);
		if (new_residue < residue) {
			residue = new_residue;
			memcpy(rand_soln, temp_soln, sizeof(int)*100);
		} 

		else {
			// Cooling schedule
			prob = exp(((-1) * (new_residue - residue))/(pow(10, 10)*pow(0.8, (it/300))));
			// If lower, then same outcome as above.
			if (cust_rand(1) < prob) {
				residue = new_residue;
				memcpy(rand_soln, temp_soln, sizeof(int)*100);
			}
		}

		// Lastly, check if global maximum so far.
		if (residue < min_res) {
			min_res = residue;
		}
	}

	free(rand_soln);
	free(temp_soln);
	return min_res;
}

void generate_from_input(char* filename, int64_t* A) {
	// Initialize the reading in of the file
	FILE* f;
	char* line = NULL;
	size_t len = 64;
	f = fopen(filename,"r");
	if (f == NULL) {
		printf("Invalid file. Exiting.\n");
		exit(0);
	}

	// Get S; assuming size here always 100
	for (int i = 0; i < 100; i++) {
		getline(&line, &len, f);
		A[i] = strtoll(line, NULL, 10); // string to long long 
	}
}

void generate_randomly(int64_t* A) {
	for (int i = 0; i < 100; i++) {
		A[i] = rand(); // may need to generate long long random number here
	}
}

int main(int argc, char* argv[]) {
	// Set seed
	srand(time(NULL));
	cust_rand(1);

	if (argc != 4) {
		printf("Usage: ./partition flag algorithm inputfile \n");
		exit(0);
	}

	// Allocate for input Array
	int64_t* A = malloc(sizeof(int64_t)*100);
	int flag = strtol(argv[1],NULL,10);

	// Input mode 
	if (flag == 0) {
		
		int64_t res = -1;
		int algorithm = strtol(argv[2],NULL,10);
		char* inputfile = argv[3];
		generate_from_input(inputfile,A);

		if (algorithm == 0) {
			res = karmarkarKarp(A,100);
		}
		else if (algorithm == 1) {
			res = repeated_random(A,1,false);
		}
		else if (algorithm == 2) {
			res = hill_climbing(A,1,false);
		}
		else if (algorithm == 3) {
			res = simulated_annealing(A,1,false);
		}
		else if (algorithm == 11) {
			res = repeated_random(A,1,true);
		}
		else if (algorithm == 12) {
			res = hill_climbing(A,1,true);
		}
		else if (algorithm == 13) {
			res = simulated_annealing(A,1,true);
		}
		else {
			printf("Invalid algorithm code: %d\n", algorithm);
			exit(0);
		}

		printf("%llu\n",res);
	}

	// Randomized mode > 0
	else {
		int numgens = 50;
		int numits = 25000;

		int64_t results[7];
		for (int i = 0; i < 7; i++) {
			results[i] = 0;
		}

		for (int it = 0; it < numgens; it++) {
			generate_randomly(A);

			results[1] += repeated_random(A,numits,false);
			results[2] += hill_climbing(A,numits,false);
			results[3] += simulated_annealing(A,numits,false);
			results[4] += repeated_random(A,numits,true);
			results[5] += hill_climbing(A,numits,true);
			results[6] += simulated_annealing(A,numits,true);
			results[0] += karmarkarKarp(A,100);
		}
		for (int i = 0; i < 7; i++) {
			printf("%llu\n",results[i]);
		}
	}

	free(A);
}
