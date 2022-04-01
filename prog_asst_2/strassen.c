#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

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
void stand_mult(int* mata, int* matb, int* matout, int dim) {
	int sum;
	// i row matrix a 
	for (int i = 0; i < dim; i++) {
		// j row matrix b
		for (int k = 0; k < dim; k++) {
			// k combs for entry
			sum = 0;
			for (int j = 0; j < dim; j++) {
				sum += mata[i*dim + j] * matb[j*dim + k];
			}
			matout[i*dim + k] = sum;
		}
	}
}


// Addition of matrices, giving matout, of size dim, going from r_start to r_end 
// on the rows in each matrix (and correspondingly for c_start and c_end and columns)
void stand_add(int* mata, int* matb, int* matout, int dim, int a_r_start, int a_r_end,
	int a_c_start, int a_c_end, int b_r_start, int b_r_end, int b_c_start, int b_c_end, bool subtract) {

	subtract = -(2*subtract - 1);

	int row_len = a_r_end - a_r_start;
	int col_len = a_c_end - a_c_start;

	for (int i = 0; i < row_len; i++) {
		for (int j = 0; j < col_len; j++) {
			matout[i*row_len + j] = mata[(a_r_start+i)*dim + (a_c_start+j)] + (subtract * matb[(b_r_start+i)*dim + (b_c_start+j)]);
		}
	}
}

// This way you can just call A_11 like in strassens and you don't cross your eyes
void stand_add_helper(int*mata, int ar_half, int ac_half, int* matb, int br_half, int bc_half, int* matout, int dim, int m_dim, bool full, bool subtract) { 
	ar_half -= 1; ac_half -= 1; br_half -= 1; bc_half -= 1;

	if (full == false) {
		stand_add(mata, matb, matout, dim, ar_half*m_dim, (ar_half+1)*m_dim, ac_half*m_dim, (ac_half+1)*m_dim,
		br_half*m_dim, (br_half+1)*m_dim, bc_half*m_dim, (bc_half+1)*m_dim, subtract);
	}

	// Don't want submatrices, the full thing
	else {
		stand_add(mata, matb, matout, dim, 0, dim, 0, dim, 0, dim, 0, dim,subtract);
	}
}

void stand_full_add_helper(int* mata, int* matb, int* matout, int dim, bool subtract) {
	stand_add_helper(mata,-1,-1,matb,-1,-1,matout,dim,-1,true,subtract);
}

// Multiplication of two matrices using strassen 
void strass_mult(int* mata, int* matb, int* matout, int dim) {

	// Dimension of an M matrix
	int m_dim = dim/2;
	int* m1 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* m2 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* m3 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* m4 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* m5 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* m6 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* m7 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* addend1 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* addend2 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* c11 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* c12 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* c21 = malloc((m_dim)*(m_dim)*sizeof(int));
	int* c22 = malloc((m_dim)*(m_dim)*sizeof(int));

	// M1
	stand_add_helper(mata,1,1,mata,2,2,addend1,dim,m_dim,false,false);
	stand_add_helper(matb,1,1,matb,2,2,addend2,dim,m_dim,false,false);
	stand_mult(addend1,addend2,m1,m_dim);

	// M2
	stand_add_helper(mata,2,1,mata,2,2,addend1,dim,m_dim,false,false);
	stand_add_helper(matb,1,1,matb,2,2,addend2,dim,m_dim,false,false); // fix 
	stand_mult(addend1,addend2,m2,m_dim);

	// M3
	stand_add_helper(mata,1,1,mata,2,2,addend1,dim,m_dim,false,false); // fix
	stand_add_helper(matb,1,2,matb,2,2,addend2,dim,m_dim,false,false);
	stand_mult(addend1,addend2,m3,m_dim);
	
	// M4
	stand_add_helper(mata,1,1,mata,2,2,addend1,dim,m_dim,false,false); // fix
	stand_add_helper(matb,2,1,matb,1,1,addend2,dim,m_dim,false,false);
	stand_mult(addend1,addend2,m4,m_dim);

	// M5
	stand_add_helper(mata,1,1,mata,1,2,addend1,dim,m_dim,false,false);
	stand_add_helper(matb,1,1,matb,2,2,addend2,dim,m_dim,false,false); // fix
	stand_mult(addend1,addend2,m5,m_dim);

	// M6
	stand_add_helper(mata,2,1,mata,1,1,addend1,dim,m_dim,false,false);
	stand_add_helper(matb,1,1,matb,1,2,addend2,dim,m_dim,false,false);
	stand_mult(addend1,addend2,m6,m_dim);
		
	// M7
	stand_add_helper(mata,1,2,mata,2,2,addend1,dim,m_dim,false,false);
	stand_add_helper(matb,2,1,matb,2,2,addend2,dim,m_dim,false,false);
	stand_mult(addend1,addend2,m7,m_dim);


	// C11 
	stand_full_add_helper(m1,m4,addend1,dim,false);
	stand_full_add_helper(m5,m7,addend2,dim,false);
	stand_full_add_helper(addend1,addend2,c11,dim,true);

	// C12
	stand_full_add_helper(m3,m5,c12,dim,false);

	// C21
	stand_full_add_helper(m2,m4,c21,dim,false);

	// C22
	stand_full_add_helper(m2,m3,addend1,dim,false);
	stand_full_add_helper(addend1,m6,addend2,dim,false);
	stand_full_add_helper(m1,addend2,c22,dim,true);


	/*print_matrix(addend1,m_dim,0,m_dim,0,m_dim);
	print_matrix(addend2,m_dim,0,m_dim,0,m_dim);
	print_matrix(m1,m_dim,0,m_dim,0,m_dim);*/

}

// RNG function used
float cust_rand()
{
    return (float)rand() / (float) RAND_MAX ;
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