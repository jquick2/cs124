#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

int min3(int x, int y, int z){
  return x < y ? (x < z ? x : z) : (y < z ? y : z);
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
void stand_mult(int* mata, int a_dim, int a_r_off, int a_c_off, int* matb, int b_dim, int b_r_off, int b_c_off, int* matout, int dim) {
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

	//printf("here\n");
	int it_dim = min3(a_dim,b_dim,out_dim);

	//printf("here2\n");

	for (int i = 0; i < it_dim; i++) {
		for (int j = 0; j < it_dim; j++) {
			matout[(i+out_r_off)*out_dim + (j + out_c_off)] = mata[(a_r_off+i)*a_dim + (a_c_off+j)] + 
			(sign * matb[(b_r_off+i)*b_dim + (b_c_off+j)]);
		}
	}
}

// Multiplication of two matrices using strassen 
void strass_mult(int* mata, int* matb, int* matout, int dim) {

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
	stand_add(mata,0,0,mata,m_dim,m_dim,addend1,0,0,dim,dim,m_dim,false);
	stand_add(matb,0,0,matb,m_dim,m_dim,addend2,0,0,dim,dim,m_dim,false);
	stand_mult(addend1,m_dim,0,0,addend2,m_dim,0,0,p1,m_dim);


	// M2
	stand_add(mata,m_dim,0,mata,m_dim,m_dim,addend1,0,0,dim,dim,m_dim,false);
	stand_mult(addend1,m_dim,0,0,matb,dim,0,0,p2,m_dim);

	// M3
	stand_add(matb,0,m_dim,matb,m_dim,m_dim,addend2,0,0,dim,dim,m_dim,true);
	stand_mult(mata,dim,0,0,addend2,m_dim,0,0,p3,m_dim);

	// M6
	stand_add(mata,m_dim,0,mata,0,0,addend1,0,0,dim,dim,m_dim,true);
	stand_add(matb,0,0,matb,0,m_dim,addend2,0,0,dim,dim,m_dim,false);
	stand_mult(addend1,m_dim,0,0,addend2,m_dim,0,0,p4,m_dim);

	// C22
	stand_add(p1,0,0,p2,0,0,matout,m_dim,m_dim,m_dim,m_dim,dim,true);
	stand_add(matout,m_dim,m_dim,p3,0,0,matout,m_dim,m_dim,dim,m_dim,dim,false);
	stand_add(matout,m_dim,m_dim,p4,0,0,matout,m_dim,m_dim,dim,m_dim,dim,false);

	// M5
	stand_add(mata,0,0,mata,0,m_dim,addend1,0,0,dim,dim,m_dim,false);
	stand_mult(addend1,m_dim,0,0,matb,dim,m_dim,m_dim,p4,m_dim);

	// C12
	stand_add(p3,0,0,p4,0,0,matout,0,m_dim,m_dim,m_dim,dim,false);

	// M4
	stand_add(matb,m_dim,0,matb,0,0,addend2,0,0,dim,dim,m_dim,true);
	stand_mult(mata,dim,m_dim,m_dim,addend2,m_dim,0,0,p3,m_dim);

	// C21
	stand_add(p2,0,0,p3,0,0,matout,m_dim,0,m_dim,m_dim,dim,false);

	// M7
	stand_add(mata,0,m_dim,mata,m_dim,m_dim,addend1,0,0,dim,dim,m_dim,true);
	stand_add(matb,m_dim,0,matb,m_dim,m_dim,addend2,0,0,dim,dim,m_dim,false);
	stand_mult(addend1,m_dim,0,0,addend2,m_dim,0,0,p2,m_dim);

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
    return (float)rand() / (float)RAND_MAX ;
}


// Initialize a dim x dim matrix
void init_matrix(int* mat, int dim) {
	// i rows
	for (int i = 0; i < dim; i++) {
		// j columns
		for (int j = 0; j < dim; j++) {
			mat[dim*i + j] = (int) floor(3*cust_rand());
		}
	}
}

int main(int argc, char* argv[]) {
	clock_t startStrass, endStrass, startNormal, endNormal;
	double timeStrass, timeNormal;
	clock_t differenceStrass, differenceNormal;
	int* mat1;
	int* mat2;
	int* mat3;
	srand(time(NULL));
	cust_rand();

	if (argc != 2) {
		printf("Usage: ./strassen dim\n");
		exit(0);
	}

	int dim = strtol(argv[1],NULL,10);

	int* mat1 = malloc(dim*dim*sizeof(int));
	int* mat2 = malloc(dim*dim*sizeof(int));
	int* mat3 = malloc(dim*dim*sizeof(int));

	init_matrix(mat1,dim);
	init_matrix(mat2,dim);

	//print_matrix(mat1,dim,0,dim,0,dim);
	//print_matrix(mat2,dim,0,dim,0,dim);
	
	strass_mult(mat1,mat2,mat3,dim);

	//print_matrix(mat3,dim,0,dim,0,dim);

	stand_mult(mat1,dim,0,0,mat2,dim,0,0,mat3,dim);

	//print_matrix(mat3,dim,0,dim,0,dim);

	double cpu_time_used;

	// for (var i = 0; i < 10; ) {
	/*start = clock();
	strass_mult(mat1,mat2,mat3,dim);
	end = clock();
	double difference = (double) (end - start);
	cpu_time_used = difference / (double) CLOCKS_PER_SEC;
	printf("Time %f\n", cpu_time_used);

	start = clock();
	strass_mult(mat1,mat2,mat3,dim);
	end = clock();
	difference = (double) (end - start);
	cpu_time_used = difference / (double) CLOCKS_PER_SEC;
	printf("Time %f\n", cpu_time_used);*/



	free(mat1);
	free(mat2);
	free(mat3);

	int count = 0;

	printf("\n\033[0;32mPrinting for wide range\033[0m\n\n");
	for (int i = 1; i < 20000; i = i << 1) {
		// Initialize matrices
		mat1 = malloc(dim*dim*sizeof(int));
		mat2 = malloc(dim*dim*sizeof(int));
		mat3 = malloc(dim*dim*sizeof(int));
		init_matrix(mat1,dim);
		init_matrix(mat2,dim);

		// Print matrixes if necessary
		// print_matrix(mat1,dim,0,dim,0,dim);
		// print_matrix(mat2,dim,0,dim,0,dim);
		count++;
		// Strassen Timing
		printf("Loop %d and i = %d\n", count, i);
		startStrass = clock();
		strass_mult(mat1,mat2,mat3,dim);
		endStrass = clock();
		differenceStrass = (double) (endStrass - startStrass);
		// timeStrass = differenceStrass / (double) CLOCKS_PER_SEC;
		timeStrass = 0;
		printf("Strass Time \033[0;33m%f\n\033[0m", timeStrass);

		// Normal Timing
		startNormal = clock();
		strass_mult(mat1,mat2,mat3,dim);
		endNormal = clock();
		differenceNormal = (double) (endNormal - startNormal);
		// timeNormal = differenceNormal / (double) CLOCKS_PER_SEC;
		timeNormal = 0;
		printf("Normal Time \033[0;33m%f\n\033[0m", timeNormal);
		printf("End loop %d\n\n", count);

		free(mat1);
		free(mat2);
		free(mat3);
		// print_matrix(mat3,dim,0,dim,0,dim);
	}	

	int startRange = 64; // Can switch w/ args if needed
	int endRange = 256; // Not gauranteed to exactly end on this number
	int segments = 4; // Number of points to record from 64 to 256
	count = 0;

	printf("\n\033[0;32mNow printing for %d to %d\033[0m\n\n", startRange, endRange);
	for (int i = startRange; i <= endRange; i = i + (((endRange - startRange) / (segments - 1)))) {
		
		// Initialize matrices
		mat1 = malloc(dim*dim*sizeof(int));
		mat2 = malloc(dim*dim*sizeof(int));
		mat3 = malloc(dim*dim*sizeof(int));
		init_matrix(mat1,dim);
		init_matrix(mat2,dim);

		// Print matrixes if necessary
		// print_matrix(mat1,dim,0,dim,0,dim);
		// print_matrix(mat2,dim,0,dim,0,dim);
		count++;
		// Strassen Timing
		printf("Loop %d and i = %d\n", count, i);
		startStrass = clock();
		strass_mult(mat1,mat2,mat3,dim);
		endStrass = clock();
		differenceStrass = (double) (endStrass - startStrass);
		// timeStrass = differenceStrass / (double) CLOCKS_PER_SEC;
		timeStrass = 0;
		printf("Strass Time \033[0;33m%f\n\033[0m", timeStrass);

		// Normal Timing
		startNormal = clock();
		strass_mult(mat1,mat2,mat3,dim);
		endNormal = clock();
		differenceNormal = (double) (endNormal - startNormal);
		// timeNormal = differenceNormal / (double) CLOCKS_PER_SEC;
		timeNormal = 0;
		printf("Normal Time \033[0;33m%f\n\033[0m", timeNormal);
		printf("End loop %d\n\n", count);

		free(mat1);
		free(mat2);
		free(mat3);
		// print_matrix(mat3,dim,0,dim,0,dim);
	}
}