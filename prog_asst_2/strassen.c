#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "strassen.h"

int min3(int x, int y, int z){
  return x < y ? (x < z ? x : z) : (y < z ? y : z);
}

double log2(double x) {
	return log(x)/log(2);
}

// Print neatly a dim x dim matrix mat, for rows start_r to end_r and start_c to end_c
void print_matrix(int* mat, int dim, int start_r, int end_r, int start_c, int end_c) {
	for (int i = start_r; i < end_r; i++) {
		for (int j = start_c; j < end_c; j++) {
			printf("%d ",mat[i*dim + j]);
		}
		printf("\n");
	}
	printf("\n\n");
}

// Standard multiplication of matrices each of dim x dim size, producing matout
// This version is just a triple for loop, no submatrices
// I wanted to see if there was a major difference with a submatrix approach
// That would reuse some addresses in memory in rapid succession with this 
// Localization. Doesn't seem to make a big difference in the break point however
void old_stand_mult(int* mata, int a_dim, int a_r_off, int a_c_off, int* matb, int b_dim, int b_r_off, int b_c_off, int* matout, int dim) {
	int sum;
	// i row matrix a 
	for (int i = 0; i < dim; i++) {
		// k column matrix b
		for (int k = 0; k < dim; k++) {
			// j combs
			sum = 0;
			for (int j = 0; j < dim; j++) {
				sum += mata[(i+a_r_off)*a_dim + (j+a_c_off)] * matb[(j+b_r_off)*b_dim + (k+b_c_off)];
			}
			matout[i*dim + k] = sum;
		}
	}
}

// Addition of matrices, giving matout, with specified starting offsets for rows and columns
// of a, b, and out matrices, as well as specified dimensions of the input matrices and a 
// subtract boolean
void stand_add(int* mata, int a_r_off, int a_c_off, int* matb, int b_r_off, int b_c_off,
	int* matout, int out_r_off, int out_c_off, int a_dim, int b_dim, int out_dim, bool subtract) {

	int sign = -1*(2*(int)subtract - 1);

	int it_dim = min3(a_dim,b_dim,out_dim);

	for (int i = 0; i < it_dim; i++) {
		for (int j = 0; j < it_dim; j++) {
			matout[(i+out_r_off)*out_dim + (j + out_c_off)] = mata[(a_r_off+i)*a_dim + (a_c_off+j)] + 
			(sign * matb[(b_r_off+i)*b_dim + (b_c_off+j)]);
		}
	}
}

// Multiplication with splitting into submatrices but naive 8 mults,
// I found this method to be worse than the naive method in speed.
// Still included the code for it though.
void stand_mult(int* mata, int a_dim, int a_r_off, int a_c_off, int* matb, int b_dim, int b_r_off, int b_c_off, int* matout, int dim) {
	// Sub-dimension for split
	if (dim == 1) {
		matout[0] = mata[a_r_off*a_dim + a_c_off] * matb[b_r_off*b_dim + b_c_off];
	}

	else {
		int m_dim = dim / 2;
		int* addend1 = malloc((m_dim)*(m_dim)*sizeof(int));
		int* addend2 = malloc((m_dim)*(m_dim)*sizeof(int));
		
		// C11
		stand_mult(mata,a_dim,a_r_off,a_c_off,matb,b_dim,b_r_off,b_c_off,addend1,m_dim);
		stand_mult(mata,a_dim,a_r_off,m_dim+a_c_off,matb,b_dim,m_dim+b_r_off,b_c_off,addend2,m_dim);
		stand_add(addend1,0,0,addend2,0,0,matout,0,0,m_dim,m_dim,dim,false);

		// C12
		stand_mult(mata,a_dim,a_r_off,a_c_off,matb,b_dim,b_r_off,m_dim+b_c_off,addend1,m_dim);
		stand_mult(mata,a_dim,a_r_off,m_dim+a_c_off,matb,b_dim,m_dim+b_r_off,m_dim+b_c_off,addend2,m_dim);
		stand_add(addend1,0,0,addend2,0,0,matout,0,m_dim,m_dim,m_dim,dim,false);

		// C21
		stand_mult(mata,a_dim,m_dim+a_r_off,a_c_off,matb,b_dim,b_r_off,b_c_off,addend1,m_dim);
		stand_mult(mata,a_dim,m_dim+a_r_off,m_dim+a_c_off,matb,b_dim,m_dim+b_r_off,b_c_off,addend2,m_dim);
		stand_add(addend1,0,0,addend2,0,0,matout,m_dim,0,m_dim,m_dim,dim,false);

		// C22
		stand_mult(mata,a_dim,m_dim+a_r_off,a_c_off,matb,b_dim,b_r_off,m_dim+b_c_off,addend1,m_dim);
		stand_mult(mata,a_dim,m_dim+a_r_off,m_dim+a_c_off,matb,b_dim,m_dim+b_r_off,m_dim+b_c_off,addend2,m_dim);
		stand_add(addend1,0,0,addend2,0,0,matout,m_dim,m_dim,m_dim,m_dim,dim,false);

		free(addend1);
		free(addend2);
	}

}

int* resize_matrix(int* matrix, int old_dim, int new_dim) {
	int* tmp = malloc((old_dim)*(old_dim)*sizeof(int));
	memcpy(tmp,matrix,(old_dim)*(old_dim)*sizeof(int));
	matrix = realloc(matrix,(new_dim)*(new_dim)*sizeof(int));

	for (int i = 0; i < new_dim; i ++) {
		for (int j = 0; j < new_dim; j ++) {
			if (i < old_dim && j < old_dim) {
				matrix[i*new_dim + j] = tmp[i*old_dim + j];
			}
			else {
				matrix[i*new_dim + j] = 0;
			}
		}
	}
	free(tmp);
	return matrix;
}

int* resize_matrix_down(int* matrix, int old_dim, int new_dim) {
	int* tmp = malloc((old_dim)*(old_dim)*sizeof(int));
	memcpy(tmp,matrix,(old_dim)*(old_dim)*sizeof(int));
	matrix = realloc(matrix,(new_dim)*(new_dim)*sizeof(int));

	for (int i = 0; i < new_dim; i++) {
		for (int j = 0; j < new_dim; j++) {
			matrix[i*new_dim + j] = tmp[i*old_dim + j];
		}
	}

	free(tmp);
	return matrix;
}

void choose_mult(int* mata, int a_dim, int a_r_off, int a_c_off,
	int* matb, int b_dim, int b_r_off, int b_c_off, 
	int* matout, int dim, int cutoff) {

	if (dim <= cutoff || dim <= 1) {
		old_stand_mult(mata,a_dim,a_r_off,a_c_off,matb,b_dim,b_r_off,b_c_off,matout,dim);
	}
	else {
		strass_mult(mata,a_dim,a_r_off,a_c_off,matb,b_dim,b_r_off,b_c_off,matout,dim,cutoff);
	}

}

// Muiplication of two matrices using strassen 
void strass_mult(int* mata, int a_dim, int a_r_off, int a_c_off,
	int* matb, int b_dim, int b_r_off, int b_c_off, 
	int* matout, int dim, int cutoff) {

	// Dimension of an M matrix
	int m_dim = dim/2;

	int* p1 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* p2 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* p3 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* p4 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* addend1 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* addend2 = malloc((m_dim)*(m_dim)*sizeof(int));

	// Calculating in order 1, 2, 3, 6, 5, 4, 7 allows us to only use 4
	// mallocs for placeholders.

	// M1
<<<<<<< HEAD
	stand_add(mata,a_r_off,a_c_off,mata,m_dim+a_r_off,m_dim+a_c_off,addend1,0,0,a_dim,a_dim,m_dim,false);
	stand_add(matb,b_r_off,b_c_off,matb,m_dim+b_r_off,m_dim+b_c_off,addend2,0,0,b_dim,b_dim,m_dim,false);
	choose_mult(addend1,m_dim,0,0,addend2,m_dim,0,0,p1,m_dim,cutoff);
=======
	stand_add(mata,0,0,mata,m_dim,m_dim,addend1,0,0,dim,dim,m_dim,false);
	stand_add(matb,0,0,matb,m_dim,m_dim,addend2,0,0,dim,dim,m_dim,false);
	stand_mult(addend1,m_dim,0,0,addend2,m_dim,0,0,p1,m_dim);
>>>>>>> 2bc900626fd873dd8b8f7b73688d668251f30498

	// M2
	stand_add(mata,m_dim+a_r_off,a_c_off,mata,m_dim+a_r_off,m_dim+a_c_off,addend1,0,0,a_dim,a_dim,m_dim,false);
	choose_mult(addend1,m_dim,0,0,matb,b_dim,b_r_off,b_c_off,p2,m_dim,cutoff);

	// M3
	stand_add(matb,b_r_off,m_dim+b_c_off,matb,m_dim+b_r_off,m_dim+b_c_off,addend2,0,0,b_dim,b_dim,m_dim,true);
	choose_mult(mata,a_dim,a_r_off,a_c_off,addend2,m_dim,0,0,p3,m_dim,cutoff);

	// M6
	stand_add(mata,m_dim+a_r_off,a_c_off,mata,a_r_off,a_c_off,addend1,0,0,a_dim,a_dim,m_dim,true);
	stand_add(matb,b_r_off,b_c_off,matb,b_r_off,m_dim+b_c_off,addend2,0,0,b_dim,b_dim,m_dim,false);
	choose_mult(addend1,m_dim,0,0,addend2,m_dim,0,0,p4,m_dim,cutoff);

	// C22
	stand_add(p1,0,0,p2,0,0,matout,m_dim,m_dim,m_dim,m_dim,dim,true);

	stand_add(matout,m_dim,m_dim,p3,0,0,matout,m_dim,m_dim,dim,m_dim,dim,false);
	stand_add(matout,m_dim,m_dim,p4,0,0,matout,m_dim,m_dim,dim,m_dim,dim,false);

	// M5
	
	stand_add(mata,a_r_off,a_c_off,mata,a_r_off,m_dim+a_c_off,addend1,0,0,a_dim,a_dim,m_dim,false);
	choose_mult(addend1,m_dim,0,0,matb,b_dim,m_dim+b_r_off,m_dim+b_c_off,p4,m_dim,cutoff);

	// C12

	stand_add(p3,0,0,p4,0,0,matout,0,m_dim,m_dim,m_dim,dim,false);

	// M4
	stand_add(matb,m_dim+b_r_off,b_c_off,matb,b_r_off,b_c_off,addend2,0,0,b_dim,b_dim,m_dim,true);
	choose_mult(mata,a_dim,m_dim+a_r_off,m_dim+a_c_off,addend2,m_dim,0,0,p3,m_dim,cutoff);

	// C21
	stand_add(p2,0,0,p3,0,0,matout,m_dim,0,m_dim,m_dim,dim,false);

	// M7
	stand_add(mata,a_r_off,m_dim+a_c_off,mata,m_dim+a_r_off,m_dim+a_c_off,addend1,0,0,a_dim,a_dim,m_dim,true);
	stand_add(matb,m_dim+b_r_off,b_c_off,matb,m_dim+b_r_off,m_dim+b_c_off,addend2,0,0,b_dim,b_dim,m_dim,false);
	choose_mult(addend1,m_dim,0,0,addend2,m_dim,0,0,p2,m_dim,cutoff);

	// C11
	stand_add(p1,0,0,p3,0,0,matout,0,0,m_dim,m_dim,dim,false);
	stand_add(matout,0,0,p4,0,0,matout,0,0,dim,m_dim,dim,true);
	stand_add(matout,0,0,p2,0,0,matout,0,0,dim,m_dim,dim,false);

	free(p1);
	free(p2);
	free(p3);
	free(p4);
	free(addend1);
	free(addend2);
}

// RNG function used
float cust_rand()
{
    return (float)rand() / (float) RAND_MAX ;
}


// Initialize a dim x dim matrix
void init_matrix(int* mat, int dim, int new_dim) {
	// i rows
	for (int i = 0; i < new_dim; i++) {
		// j columns
		for (int j = 0; j < new_dim; j++) {
			// Add padding if necessary
			if (i < dim && j < dim) {
				mat[new_dim*i + j] = (int) floor(3*cust_rand());
			}
			else {
				mat[new_dim*i + j] = 0;
			}
		}
	}
}

bool check_power_of_2(int x) {
	while (true) {
		if (x % 2 != 0) {
			return false;
		}
		x /= 2;
		if (x == 1) {
			return true;
		}
	}
}

<<<<<<< HEAD

void rand_gen(int dim, int cutoff) {
	time_t start, end;
	int new_dim;
	// Allocate to the next power of 2, that way we don't have to realloc
	if (check_power_of_2(dim) == true) {
		new_dim = dim;
	}
	else {
		new_dim = pow(2,floor(log2(dim))+1);
	}

	int* mat1 = malloc(new_dim*new_dim*sizeof(int));
	int* mat2 = malloc(new_dim*new_dim*sizeof(int));
	int* mat3 = malloc(new_dim*new_dim*sizeof(int));
=======
	// strass_mult(mat1,mat2,mat3,dim);
	// strass_mult(mat1,mat2,mat3,dim);
>>>>>>> 2bc900626fd873dd8b8f7b73688d668251f30498

	init_matrix(mat1,dim,new_dim);
	init_matrix(mat2,dim,new_dim);

	start = clock();
	strass_mult(mat1,new_dim,0,0,mat2,new_dim,0,0,mat3,new_dim,cutoff);
	end = clock();
	double strass_time = ((double)(end-start))/CLOCKS_PER_SEC;
	
<<<<<<< HEAD
	// When we print, just print first dim rows and columns
	// Rest is padding
	//print_matrix(mat3,new_dim,0,dim,0,dim);
=======
	// strass_mult(mat1,mat2,mat3,dim);
>>>>>>> 2bc900626fd873dd8b8f7b73688d668251f30498

	printf("%d, %d: %f\n",cutoff, dim, strass_time);

<<<<<<< HEAD
	free(mat1);
	free(mat2);
	free(mat3);
}
=======
	// stand_mult(mat1,dim,0,0,mat2,dim,0,0,mat3,dim);
>>>>>>> 2bc900626fd873dd8b8f7b73688d668251f30498


<<<<<<< HEAD
void file_gen(int dim, int cutoff, char* filename) {

	int new_dim;
	// Allocate to the next power of 2, that way we don't have to realloc
	if (check_power_of_2(dim) == true) {
		new_dim = dim;
	}
	else {
		new_dim = pow(2,floor(log2(dim))+1);
	}
	// Initialize input/output matrices
	int* mat1 = malloc(new_dim*new_dim*sizeof(int));
	int* mat2 = malloc(new_dim*new_dim*sizeof(int));
	int* mat3 = malloc(new_dim*new_dim*sizeof(int));

	// Initialize the reading in of the file
	FILE* f;
	char* line = NULL;
	size_t len = 64;
	f = fopen(filename,"r");
	if (f == NULL) {
		printf("Invalid file. Exiting.\n");
		exit(0);
	}

	// Get Matrix A
	for (int i = 0; i < new_dim; i++) {
		for (int j = 0; j < new_dim; j++) {
			if (i < dim && j < dim) {
				getline(&line, &len, f);
				mat1[i*new_dim + j] = strtol(line,NULL,10);	
			}
			else {
				mat1[i*new_dim +j] = 0;
			}

		}
	}

	// Get Matrix B
	for (int i = 0; i < new_dim; i++) {
		for (int j = 0; j < new_dim; j++) {
			if (i < dim && j < dim) {
				getline(&line, &len, f);
				mat2[i*new_dim + j] = strtol(line,NULL,10);	
			}
			else {
				mat2[i*new_dim +j] = 0;
			}
		}
	}

	strass_mult(mat1,new_dim,0,0,mat2,new_dim,0,0,mat3,new_dim,cutoff);

	// Print output for the diagonals
	for (int i = 0; i < dim; i ++) {
		printf("%d\n",mat3[i*new_dim + i]);
	}

	// Print extra line
	printf("\n");

	free(mat1);
	free(mat2);
	free(mat3);
	fclose(f);
}


void init_graph(int* mat, float prob) {
	for (int i = 0; i < 1024; i++) {
		for (int j = 0; j < 1024; j ++) {
			if (j == i) {
				mat[i*1024 +j] =0;
			}
			else {
				mat[i*1024 +j] = cust_rand() < prob ? 1 : 0;
			}
		}
	}

}

void triangle_count(int iterations) {
	int cutoff = 32;
	
	int* mat = malloc(1024*1024*sizeof(int));
	int* mat2 = malloc(1024*1024*sizeof(int));
	int* mat3 = malloc(1024*1024*sizeof(int));

	int* results = malloc(iterations*sizeof(int));

	for (float p = 0.01; p <= 0.055; p += 0.01) {
		printf("Probability %f\n",p);
		int total_triangles = 0;
		int min = INT_MAX;
		int max = 0;
		float var_sum = 0;
		float variance;
		for (int it = 0; it < iterations; it ++) {

			init_graph(mat,p);
			strass_mult(mat,1024,0,0,mat,1024,0,0,mat2,1024,cutoff);
			strass_mult(mat,1024,0,0,mat2,1024,0,0,mat3,1024,cutoff);

			int sum = 0;
			int triangles = 0;
			for (int i = 0; i < 1024; i++) {
				sum += mat3[i*1024 + i];
			}
		
			triangles = sum / 6;
			results[it] = triangles;

		}	
		for (int it = 0; it <iterations; it++) {
			total_triangles += results[it];
			if (results[it] < min) {
				min = results[it];
			}
			if (results[it] > max) {
				max = results[it];
			}
		}
		float average = total_triangles/iterations;

		printf("Average: %f triangles \n", average);
		printf("Min: %d triangles \n", min);
		printf("Max: %d triangles \n", max);

		for (int it = 0; it < iterations; it ++) {
			var_sum += pow(results[it] - average,2);
		}
		variance = var_sum / (iterations - 1);
		printf("Std Deviation: %f \n", sqrt(variance));
	}

	free(mat);
	free(mat2);
	free(mat3);
	free(results);
}

int main(int argc, char* argv[]) {

	// Set seed
	srand(time(NULL));
	cust_rand();

	if (argc < 3 || argc > 4) {
		printf("Usage: ./strassen cutoff dim/iterations filename\n");
		exit(0);
	}

	else {
		int cutoff = strtol(argv[1],NULL,10);
		if (cutoff < 0) {
			int iterations = strtol(argv[2],NULL,10);
			triangle_count(iterations);

		}
		else {
			int dim = strtol(argv[2],NULL,10);
			if (argc == 3) {
				rand_gen(dim,cutoff);
			}
			else {
				char* infile_name = argv[3];
				file_gen(dim,cutoff,infile_name);
			}	
		}
=======
	int count = 0;
	int dim;
	double totalStrass;
	double totalNormal;
	int reps;

	printf("\n\033[0;32mPrinting for wide range\033[0m\n\n");
	// Wide range printing function
	for (int i = 1; i <= 2048; i = i << 1) {
		dim = i;
		count++;
		// Strassen Timing
		totalStrass = 0;
		totalNormal = 0;
		reps = 3;
		for (int j = 0; j < 5; j++) {
			// Initialize matrices
			mat1 = malloc(dim*dim*sizeof(int));
			mat2 = malloc(dim*dim*sizeof(int));
			mat3 = malloc(dim*dim*sizeof(int));
			init_matrix(mat1,dim);
			init_matrix(mat2,dim);

			startStrass = clock();
			strass_mult(mat1,mat2,mat3,dim);
			endStrass = clock();
			differenceStrass = (double) (endStrass - startStrass);
			timeStrass = differenceStrass / (double) CLOCKS_PER_SEC;
			totalStrass += timeStrass;

			// Normal Timing
			startNormal = clock();
			stand_mult(mat1,dim,0,0,mat2,dim,0,0,mat3,dim);
			endNormal = clock();
			differenceNormal = (double) (endNormal - startNormal);
			timeNormal = differenceNormal / (double) CLOCKS_PER_SEC;
			totalNormal += timeNormal;

			free(mat1);
			free(mat2);
			free(mat3);
		}

		printf("Loop %d and i = %d\n", count, i);
		printf("Strass Time \033[0;33m%f\n\033[0m", totalStrass / (double) reps);
		printf("Normal Time \033[0;33m%f\n\033[0m", totalNormal / (double) reps);
		printf("End loop %d\n\n", count);
		// print_matrix(mat3,dim,0,dim,0,dim);
	}	

	// Use the below for viewer smaller ranges

	int startRange = 64; // Can switch w/ args if needed
	int endRange = 256; // Not gauranteed to exactly end on this number
	int segments = 5; // Number of points to record from 64 to 256
	count = 0;

	printf("\n\033[0;32mNow printing for %d to %d\033[0m\n\n", startRange, endRange);
	for (int i = startRange; i <= endRange; i = i + (((endRange - startRange) / (segments - 1)))) {
		dim = i;
		// Print matrixes if necessary
		// print_matrix(mat1,dim,0,dim,0,dim);
		// print_matrix(mat2,dim,0,dim,0,dim);
		totalStrass = 0;
		totalNormal = 0;
		reps = 3;
		count++;
		
		for (int j = 0; j < 5; j++) {
			// Initialize matrices
			mat1 = malloc(dim*dim*sizeof(int));
			mat2 = malloc(dim*dim*sizeof(int));
			mat3 = malloc(dim*dim*sizeof(int));
			init_matrix(mat1,dim);
			init_matrix(mat2,dim);
		
			// Strassen Timing
			startStrass = clock();
			strass_mult(mat1,mat2,mat3,dim);
			endStrass = clock();
			differenceStrass = (double) (endStrass - startStrass);
			timeStrass = differenceStrass / (double) CLOCKS_PER_SEC;
			totalStrass += timeStrass;

			// Normal Timing
			startNormal = clock();
			stand_mult(mat1,dim,0,0,mat2,dim,0,0,mat3,dim);
			endNormal = clock();
			differenceNormal = (double) (endNormal - startNormal);
			timeNormal = differenceNormal / (double) CLOCKS_PER_SEC;
			totalNormal += timeNormal;

			free(mat1);
			free(mat2);
			free(mat3);
		}
		printf("Loop %d and i = %d\n", count, i);
		printf("Strass Time \033[0;33m%f\n\033[0m", totalStrass / (double) reps);
		printf("Normal Time \033[0;33m%f\n\033[0m", totalNormal / (double) reps);
		printf("End loop %d\n\n", count);
		// print_matrix(mat3,dim,0,dim,0,dim);
>>>>>>> 2bc900626fd873dd8b8f7b73688d668251f30498
	}
	printf("\n\033[0;37m------FINISHED------\n");
}