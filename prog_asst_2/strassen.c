#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void print_matrix(int* mat, int dim, int start_r, int end_r, int start_c, int end_c) {

	for (int i = start_r; i < end_r; i++) {
		for (int j = start_c; j < end_c; j++) {
			printf("%d ",mat[i*dim + j]);
		}
		printf("\n");
	}
	printf("\n\n");
}

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


void stand_add(int* mata, int* matb, int* matout, int dim, int a_r_start, int a_r_end,
	int a_c_start, int a_c_end, int b_r_start, int b_r_end, int b_c_start, int b_c_end) {

	int row_len = a_r_end - a_r_start;
	int col_len = a_c_end - a_c_start;

	for (int i = 0; i < row_len; i++) {
		for (int j = 0; j < col_len; j++) {
			matout[i*row_len + j] = mata[(a_r_start+i)*dim + (a_c_start+j)] + matb[(b_r_start+i)*dim + (b_c_start+j)];
		}
	}
}

void strass_mult(int* mata, int* matb, int* matout, int dim) {

	// Dimension of an M matrix
	int m_dim = dim/2;
	int* m1 = malloc((m_dim)*(m_dim)*sizeof(int));

	stand_add(mata,matb,m1,dim,0,m_dim,0,m_dim,0,m_dim,0,m_dim);
	print_matrix(m1,m_dim,0,m_dim,0,m_dim);

	stand_add(mata,matb,m1,dim,m_dim,dim,m_dim,dim,m_dim,dim,m_dim,m_dim);
	print_matrix(m1,m_dim,0,m_dim,0,m_dim);

	stand_add(mata,matb,m1,dim,0,m_dim,0,m_dim,0,m_dim,0,m_dim);
	print_matrix(m1,m_dim,0,m_dim,0,m_dim);

	stand_add(mata,matb,m1,dim,0,m_dim,0,m_dim,0,m_dim,0,m_dim);
	print_matrix(m1,m_dim,0,m_dim,0,m_dim);

}

float cust_rand()
{
    return (float)rand() / (float)RAND_MAX ;
}



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


	print_matrix(mat1,dim,0,dim,0,dim);
	print_matrix(mat2,dim,0,dim,0,dim);

	strass_mult(mat1,mat2,mat3,dim);

	print_matrix(mat3,dim,0,dim,0,dim);

	free(mat1);
	free(mat2);
	free(mat3);
}