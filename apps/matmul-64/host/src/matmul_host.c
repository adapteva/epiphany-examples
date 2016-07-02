/*
  matmul_host.c

  Copyright (C) 2012 Adapteva, Inc.
  Contributed by Yaniv Sapir <yaniv@adapteva.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program, see the file COPYING.  If not, see
  <http://www.gnu.org/licenses/>.
*/


// This program is the host part of the matmul() example project.
//
// This program runs on the linux host and invokes the Epiphany matmul()
// implementation. It communicates with the system via the eHost library.
// After establishing a connection using the e-server, operand matrices
// are generated based on the run-time parameter "seed". A reference
// calculation is done on the host and is compared to the Epiphany
// result. A succes/error message is printed on the terminal according
// to the result of the comparison.
//
// Jan-2012, YS.

//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "e-hal.h"
#include "matlib.h"
#include "matmul.h"
#include "common_buffers.h"

#define __DO_STRASSEN__
#define __WIPE_OUT_RESULT_MATRIX__
#undef  __DUMP_MATRICES__

#define _MAX_MEMBER_ 32
#define eMHz 600
#define aMHz 667

int   main(int argc, char *argv[]);
void  matrix_init(int seed);
int   matmul_go(e_mem_t *pDRAM);
int   matcmp(volatile float * a, volatile float * b, int NN);
int   matprt(volatile float * a, int NN);
void  strassen(float **a, float **b, float **c, int tam, int LEAF_SIZE);
void  matmul_strassen(volatile float * a, volatile float * b, volatile float * c, int NN);


typedef struct {
	e_bool_t reset_target;
	e_bool_t broadcast;
	e_bool_t run_target;
	e_hal_diag_t verbose;
	char elfFile[4096];
} args_t;

args_t ar = {E_TRUE, E_FALSE, E_TRUE, H_D0, ""};
void get_args(int argc, char *argv[]);

FILE *fo, *fi;
float Aepi[_Smtx * _Smtx];
float Bepi[_Smtx * _Smtx];
float Cref[_Smtx * _Smtx];
float Cdiff[_Smtx * _Smtx];

struct timespec timer[6];

extern e_platform_t e_platform;

int main(int argc, char *argv[])
{
	e_epiphany_t Epiphany, *pEpiphany;
	e_mem_t      DRAM,     *pDRAM;
	unsigned int msize;
	float        seed;
	unsigned int addr; //, clocks;
	size_t       sz;
	double       tdiff[4];
	int          result, rerval;
	
	pEpiphany = &Epiphany;
	pDRAM     = &DRAM;
	msize     = 0x00400000;

	get_args(argc, argv);


	fo = stderr;
	fi = stdin;

	printf("\nMatrix: C[%d][%d] = A[%d][%d] * B[%d][%d]\n\n", _Smtx, _Smtx, _Smtx, _Smtx, _Smtx, _Smtx);
	printf("Using %d x %d cores\n\n", _Nside, _Nside);
	seed = 0.0;
	printf("Seed = %f\n", seed);



	// Connect to device for communicating with the Epiphany system
	// Prepare device
	e_set_host_verbosity(H_D0);
	e_init(NULL);
	e_reset_system();

	if (e_alloc(pDRAM, 0x00000000, msize))
	{
		printf("\nERROR: Can't allocate Epiphany DRAM!\n\n");
		exit(1);
	}
	if (e_open(pEpiphany, 0, 0, e_platform.chip[0].rows, e_platform.chip[0].cols))
	{
		printf("\nERROR: Can't establish connection to Epiphany device!\n\n");
		exit(1);
	}

	// Initialize Epiphany "Ready" state
	addr = offsetof(shared_buf_t, core.ready);
	Mailbox.core.ready = 0;
	e_write(pDRAM, 0, 0, addr, &Mailbox.core.ready, sizeof(Mailbox.core.ready));

	printf("Loading program on Epiphany chip...\n");
	e_set_loader_verbosity(ar.verbose);
	result = e_load_group(ar.elfFile, pEpiphany, 0, 0, pEpiphany->rows, pEpiphany->cols, ar.run_target);
	if (result == E_ERR) {
		printf("Error loading Epiphany program.\n");
		exit(1);
	}


	// Generate operand matrices based on a provided seed
	matrix_init(seed);


#ifdef __WIPE_OUT_RESULT_MATRIX__
	// Wipe-out any previous remains in result matrix (for verification)
	addr = offsetof(shared_buf_t, C[0]);
	sz = sizeof(Mailbox.C);
	printf("Writing C[%uB] to address %08x...\n", sz, addr);
	e_write(pDRAM, 0, 0, addr, (void *) Mailbox.C, sz);
#endif

	clock_gettime(CLOCK_MONOTONIC, &timer[0]);

	// Copy operand matrices to Epiphany system
	addr = offsetof(shared_buf_t, A[0]);
	sz = sizeof(Mailbox.A);
	printf("Writing A[%uB] to address %08x...\n", sz, addr);
	e_write(pDRAM, 0, 0, addr, (void *) Mailbox.A, sz);
	
	addr = offsetof(shared_buf_t, B[0]);
	sz = sizeof(Mailbox.B);
	printf("Writing B[%uB] to address %08x...\n", sz, addr);
	e_write(pDRAM, 0, 0, addr, (void *) Mailbox.B, sz);


	// Call the Epiphany matmul() function
	printf("GO Epiphany! ...   ");
	clock_gettime(CLOCK_MONOTONIC, &timer[1]);
	matmul_go(pDRAM);
	clock_gettime(CLOCK_MONOTONIC, &timer[2]);
	printf("Finished calculating Epiphany result.\n");


	// Read result matrix and timing
	addr = offsetof(shared_buf_t, C[0]);
	sz = sizeof(Mailbox.C);
	printf("Reading result from address %08x...\n", addr);
	e_read(pDRAM, 0, 0, addr, (void *) Mailbox.C, sz);

	clock_gettime(CLOCK_MONOTONIC, &timer[3]);


	// Calculate a reference result
	printf("Calculating result on Host ...   ");
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[4]);
#ifndef __DO_STRASSEN__
	matmul(Mailbox.A, Mailbox.B, Cref, _Smtx);
#else
	matmul_strassen(Mailbox.A, Mailbox.B, Cref, _Smtx);
#endif
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &timer[5]);
	printf("Finished calculating Host result.\n");


	addr = offsetof(shared_buf_t, core.clocks);
	sz = sizeof(Mailbox.core.clocks);
	printf("Reading time from address %08x...\n", addr);
	e_read(pDRAM,0, 0, addr, &Mailbox.core.clocks, sizeof(Mailbox.core.clocks));
//	clocks = Mailbox.core.clocks;


	// Calculate the difference between the Epiphany result and the reference result
	printf("\n*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***\n");
	printf("Verifying result correctness ...   ");
	matsub(Mailbox.C, Cref, Cdiff, _Smtx);

	tdiff[0] = (timer[2].tv_sec - timer[1].tv_sec) * 1000 + ((double) (timer[2].tv_nsec - timer[1].tv_nsec) / 1000000.0);//total
	tdiff[1] = (timer[1].tv_sec - timer[0].tv_sec) * 1000 + ((double) (timer[1].tv_nsec - timer[0].tv_nsec) / 1000000.0);//write
	tdiff[2] = (timer[3].tv_sec - timer[2].tv_sec) * 1000 + ((double) (timer[3].tv_nsec - timer[2].tv_nsec) / 1000000.0);//read
	tdiff[3] = (timer[5].tv_sec - timer[4].tv_sec) * 1000 + ((double) (timer[5].tv_nsec - timer[4].tv_nsec) / 1000000.0);//ref


	// If the difference is 0, then the matrices are identical and the
	// calculation was correct
	if (iszero(Cdiff, _Smtx))
	{
		printf("C_epiphany == C_host\n");
		rerval = 0;
	} else {
		printf("\n\nERROR: C_epiphany is different from C_host !!!\n");
		rerval = 1;
	}
	printf("*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***\n");
	printf("\n");
	printf("Epiphany (compute):  %9.1f msec  (@ %03d MHz)\n"   , tdiff[0], eMHz);
	printf("         (write)  :  %9.1f msec \n"                , tdiff[1]);
	printf("         (read)   :  %9.1f msec\n"                 , tdiff[2]);
	printf("         (*total*):  %9.1f msec\n\n"               , tdiff[2]+tdiff[1]+tdiff[0]);
	printf("Host     (*total*):  %9.1f msec  (@ %03d MHz)\n"   , tdiff[3], aMHz);


#ifdef __DUMP_MATRICES__
	printf("\n\n\n");
	printf("A[][] = \n");
	matprt(Mailbox.A, _Smtx);
	printf("B[][] = \n");
	matprt(Mailbox.B, _Smtx);
	printf("C[][] = \n");
	matprt(Mailbox.C, _Smtx);
	printf("Cref[][] = \n");
	matprt(Cref, _Smtx);

	int i, j;
	for (i=0; i<_Nside; i++)
		for (j=0; j<_Nside; j++)
		{
			e_read(pEpiphany, i, j, 0x2000+0*sizeof(float), &Aepi[(i*_Score+0)*_Smtx + j*_Score], 2*sizeof(float));
			e_read(pEpiphany, i, j, 0x2000+2*sizeof(float), &Aepi[(i*_Score+1)*_Smtx + j*_Score], 2*sizeof(float));
			e_read(pEpiphany, i, j, 0x4000+0*sizeof(float), &Bepi[(i*_Score+0)*_Smtx + j*_Score], 2*sizeof(float));
			e_read(pEpiphany, i, j, 0x4000+2*sizeof(float), &Bepi[(i*_Score+1)*_Smtx + j*_Score], 2*sizeof(float));
		}
	printf("Aepi[][] = \n");
	matprt(Aepi, _Smtx);
	printf("Bepi[][] = \n");
	matprt(Bepi, _Smtx);
#endif

	printf("\n* * *   EPIPHANY FTW !!!   * * *\n");


	// Close connection to device
	if (e_close(pEpiphany))
	{
		printf("\nERROR: Can't close connection to Epiphany device!\n\n");
		exit(1);
	}
	if (e_free(pDRAM))
	{
		printf("\nERROR: Can't release Epiphany DRAM!\n\n");
		exit(1);
	}

	e_finalize();

	return rerval;
}


// Call (invoke) the matmul() function
int matmul_go(e_mem_t *pDRAM)
{
	unsigned int addr;
	
	// Wait until cores finished previous calculation
	if (ar.verbose > 0) printf("Waiting for Epiphany to be ready...\n");
	addr = offsetof(shared_buf_t, core.go);
	Mailbox.core.go = 1;
	while (Mailbox.core.go != 0)
		e_read(pDRAM, 0, 0, addr, &Mailbox.core.go, sizeof(Mailbox.core.go));

	// Signal cores to start crunching
	printf("Writing the GO!...\n");
	addr = offsetof(shared_buf_t, core.go);
	Mailbox.core.go = _MAX_MEMBER_;
	e_write(pDRAM, 0, 0, addr, &Mailbox.core.go, sizeof(Mailbox.core.go));

	// Wait until cores finished calculation
	addr = offsetof(shared_buf_t, core.go);
	Mailbox.core.go = 1;
	while (Mailbox.core.go != 0)
		e_read(pDRAM, 0, 0, addr, &Mailbox.core.go, sizeof(Mailbox.core.go));

	printf("Done...\n");

	return 0;
}


// Initialize operand matrices
void matrix_init(int seed)
{
	int i, j, p;

	p = 0;
	for (i=0; i<_Smtx; i++)
		for (j=0; j<_Smtx; j++)
			Mailbox.A[p++] = (i + j + seed) % _MAX_MEMBER_;

	p = 0;
	for (i=0; i<_Smtx; i++)
		for (j=0; j<_Smtx; j++)
			Mailbox.B[p++] = ((i + j) * 2 + seed) % _MAX_MEMBER_;

	p = 0;
	for (i=0; i<_Smtx; i++)
		for (j=0; j<_Smtx; j++)
			Mailbox.C[p++] = 0x8dead;

	return;
}


// Compare two matrices a and b  NNxNN and print different elements
int matcmp(volatile float * a, volatile float * b, int NN)
{
	int i, j, z;

	z = 0;
	for (i=0; i<NN; i++)
		for (j=0; j<NN; j++)
			if (fabs(a[i*NN+j] - b[i*NN+j]) > EPS)
			{
				printf("%10d , %10d , %f , %f\n", i, j, a[i*NN+j], b[i*NN+j]);
				z = z | 1;
			}

	return (!z);
}


// Print NNxNN matrix
int matprt(volatile float * a, int NN)
{
	int i, j;

	for (i=0; i<NN; i++)
	{
		for (j=0; j<NN; j++)
		{
			printf("%9.1f  ", a[i*NN+j]);
		}
		printf("\n");
	}

	return 0;
}


// Process command line args
void get_args(int argc, char *argv[])
{
	int n;

	strcpy(ar.elfFile, "");

	for (n=1; n<argc; n++)
	{
		if (!strcmp(argv[n], "-no-reset"))
		{
			ar.reset_target = E_FALSE;
			continue;
		}

		if (!strcmp(argv[n], "-broadcast"))
		{
			ar.broadcast = E_TRUE;
			continue;
		}

		if (!strcmp(argv[n], "-no-run"))
		{
			ar.run_target = E_FALSE;
			continue;
		}

		if (!strcmp(argv[n], "-verbose"))
		{
			n++;
			if (n < argc)
			{
				ar.verbose = atoi(argv[n]);
				if (ar.verbose < 0)
						ar.verbose = 0;
			}
			continue;
		}

		if (!strcmp(argv[n], "-h") || !strcmp(argv[n], "--help"))
		{
			printf("Usage: matmul-16_host.elf [-no-reset] [-broadcast] [-no-run] [-verbose N] [-h | --help] [ELF_file]\n");
			printf("       N: available levels of diagnostics\n");
			exit(0);
		}

		strcpy(ar.elfFile, argv[n]);
	}

	if (!strcmp(ar.elfFile, ""))
		strcpy(ar.elfFile, "matmul-16.elf");

	return;
}




#ifdef __DO_STRASSEN__
// from: http://en.wikipedia.org/wiki/Strassen_algorithm

/*------------------------------------------------------------------------------*/

/* Compile this without linking since there is no main method.                  */
/* Assuming that the file name is Strassen.c this can be done using gcc:        */
/*     gcc -c Strassen.c                                                        */

#define DOUBLE double
#define double float

void strassen(double **a, double **b, double **c, int tam, int LEAF_SIZE);
void matmul_leaf(double **a, double **b, double **c, int tam);
void sum(double **a, double **b, double **result, int tam);
void subtract(double **a, double **b, double **result, int tam);
double **allocate_real_matrix(int tam, int random);
double **free_real_matrix(double **v, int tam);

void strassen(double **a, double **b, double **c, int tam, int LEAF_SIZE) {

    // trivial case: when the matrix is 1 X 1:
    if (tam == LEAF_SIZE) {
//        c[0][0] = a[0][0] * b[0][0];
        matmul_leaf(a, b, c, LEAF_SIZE);
        return;
    }

    // other cases are treated here:
        int newTam = tam/2;
        double **a11, **a12, **a21, **a22;
        double **b11, **b12, **b21, **b22;
        double **c11, **c12, **c21, **c22;
        double **p1, **p2, **p3, **p4, **p5, **p6, **p7;

        // memory allocation:
        a11 = allocate_real_matrix(newTam, -1);
        a12 = allocate_real_matrix(newTam, -1);
        a21 = allocate_real_matrix(newTam, -1);
        a22 = allocate_real_matrix(newTam, -1);

        b11 = allocate_real_matrix(newTam, -1);
        b12 = allocate_real_matrix(newTam, -1);
        b21 = allocate_real_matrix(newTam, -1);
        b22 = allocate_real_matrix(newTam, -1);

        c11 = allocate_real_matrix(newTam, -1);
        c12 = allocate_real_matrix(newTam, -1);
        c21 = allocate_real_matrix(newTam, -1);
        c22 = allocate_real_matrix(newTam, -1);

        p1 = allocate_real_matrix(newTam, -1);
        p2 = allocate_real_matrix(newTam, -1);
        p3 = allocate_real_matrix(newTam, -1);
        p4 = allocate_real_matrix(newTam, -1);
        p5 = allocate_real_matrix(newTam, -1);
        p6 = allocate_real_matrix(newTam, -1);
        p7 = allocate_real_matrix(newTam, -1);

        double **aResult = allocate_real_matrix(newTam, -1);
        double **bResult = allocate_real_matrix(newTam, -1);

        int i, j;

        //dividing the matrices in 4 sub-matrices:
        for (i = 0; i < newTam; i++) {
            for (j = 0; j < newTam; j++) {
                a11[i][j] = a[i][j];
                a12[i][j] = a[i][j + newTam];
                a21[i][j] = a[i + newTam][j];
                a22[i][j] = a[i + newTam][j + newTam];

                b11[i][j] = b[i][j];
                b12[i][j] = b[i][j + newTam];
                b21[i][j] = b[i + newTam][j];
                b22[i][j] = b[i + newTam][j + newTam];
            }
        }

        // Calculating p1 to p7:

        sum(a11, a22, aResult, newTam); // a11 + a22
        sum(b11, b22, bResult, newTam); // b11 + b22
        strassen(aResult, bResult, p1, newTam, LEAF_SIZE); // p1 = (a11+a22) * (b11+b22)

        sum(a21, a22, aResult, newTam); // a21 + a22
        strassen(aResult, b11, p2, newTam, LEAF_SIZE); // p2 = (a21+a22) * (b11)

        subtract(b12, b22, bResult, newTam); // b12 - b22
        strassen(a11, bResult, p3, newTam, LEAF_SIZE); // p3 = (a11) * (b12 - b22)

        subtract(b21, b11, bResult, newTam); // b21 - b11
        strassen(a22, bResult, p4, newTam, LEAF_SIZE); // p4 = (a22) * (b21 - b11)

        sum(a11, a12, aResult, newTam); // a11 + a12
        strassen(aResult, b22, p5, newTam, LEAF_SIZE); // p5 = (a11+a12) * (b22)

        subtract(a21, a11, aResult, newTam); // a21 - a11
        sum(b11, b12, bResult, newTam); // b11 + b12
        strassen(aResult, bResult, p6, newTam, LEAF_SIZE); // p6 = (a21-a11) * (b11+b12)

        subtract(a12, a22, aResult, newTam); // a12 - a22
        sum(b21, b22, bResult, newTam); // b21 + b22
        strassen(aResult, bResult, p7, newTam, LEAF_SIZE); // p7 = (a12-a22) * (b21+b22)

        // calculating c21, c21, c11 e c22:

        sum(p3, p5, c12, newTam); // c12 = p3 + p5
        sum(p2, p4, c21, newTam); // c21 = p2 + p4

        sum(p1, p4, aResult, newTam); // p1 + p4
        sum(aResult, p7, bResult, newTam); // p1 + p4 + p7
        subtract(bResult, p5, c11, newTam); // c11 = p1 + p4 - p5 + p7

        sum(p1, p3, aResult, newTam); // p1 + p3
        sum(aResult, p6, bResult, newTam); // p1 + p3 + p6
        subtract(bResult, p2, c22, newTam); // c22 = p1 + p3 - p2 + p6

        // Grouping the results obtained in a single matrix:
        for (i = 0; i < newTam ; i++) {
            for (j = 0 ; j < newTam ; j++) {
                c[i][j] = c11[i][j];
                c[i][j + newTam] = c12[i][j];
                c[i + newTam][j] = c21[i][j];
                c[i + newTam][j + newTam] = c22[i][j];
            }
        }

        // deallocating memory (free):
        a11 = free_real_matrix(a11, newTam);
        a12 = free_real_matrix(a12, newTam);
        a21 = free_real_matrix(a21, newTam);
        a22 = free_real_matrix(a22, newTam);

        b11 = free_real_matrix(b11, newTam);
        b12 = free_real_matrix(b12, newTam);
        b21 = free_real_matrix(b21, newTam);
        b22 = free_real_matrix(b22, newTam);

        c11 = free_real_matrix(c11, newTam);
        c12 = free_real_matrix(c12, newTam);
        c21 = free_real_matrix(c21, newTam);
        c22 = free_real_matrix(c22, newTam);

        p1 = free_real_matrix(p1, newTam);
        p2 = free_real_matrix(p2, newTam);
        p3 = free_real_matrix(p3, newTam);
        p4 = free_real_matrix(p4, newTam);
        p5 = free_real_matrix(p5, newTam);
        p6 = free_real_matrix(p6, newTam);
        p7 = free_real_matrix(p7, newTam);
        aResult = free_real_matrix(aResult, newTam);
        bResult = free_real_matrix(bResult, newTam);

} // end of Strassen function

/*------------------------------------------------------------------------------*/
// function to sum two matrices
void sum(double **a, double **b, double **result, int tam) {

    int i, j;

    for (i = 0; i < tam; i++) {
        for (j = 0; j < tam; j++) {
            result[i][j] = a[i][j] + b[i][j];
        }
    }
}

/*------------------------------------------------------------------------------*/
// function to subtract two matrices
void subtract(double **a, double **b, double **result, int tam) {

    int i, j;

    for (i = 0; i < tam; i++) {
        for (j = 0; j < tam; j++) {
            result[i][j] = a[i][j] - b[i][j];
        }
    }
}

/*------------------------------------------------------------------------------*/
// This function allocates the matrix using malloc, and initializes it. If the variable random is passed
// as zero, it initializes the matrix with zero, if it's passed as 1, it initializes the matrix with random
// values. If it is passed with any other int value (like -1 for example) the matrix is initialized with no
// values in it. The variable tam defines the length of the matrix.
double **allocate_real_matrix(int tam, int random) {

    int i, j, n = tam, m = tam;
    double **v, a;         // pointer to the vector

    // allocates one vector of vectors (matrix)
    v = (double**) malloc(n * sizeof(double*));

    if (v == NULL) {
        printf ("** Error in matrix allocation: insufficient memory **");
        return (NULL);
    }

    // allocates each row of the matrix
    for (i = 0; i < n; i++) {
        v[i] = (double*) malloc(m * sizeof(double));

        if (v[i] == NULL) {
            printf ("** Error: Insufficient memory **");
            free_real_matrix(v, n);
            return (NULL);
        }

        // initializes the matrix with zeros
        if (random == 0) {
            for (j = 0; j < m; j++)
                v[i][j] = 0.0;
        }

        // initializes the matrix with random values between 0 and 10
        else {
            if (random == 1) {
                for (j = 0; j < m; j++) {
                    a = rand();
                    v[i][j] = (a - (int)a) * 10;
                }
            }
        }
    }

    return (v);     // returns the pointer to the vector.
}

/*------------------------------------------------------------------------------*/
// This function unallocated the matrix (frees memory)
double **free_real_matrix(double **v, int tam) {

    int i;

    if (v == NULL) {
        return (NULL);
    }

    for (i = 0; i < tam; i++) {
        if (v[i]) {
            free(v[i]); // frees a row of the matrix
            v[i] = NULL;
        }
    }

    free(v);         // frees the pointer /
    v = NULL;

    return (NULL);   //returns a null pointer /
}

/*------------------------------------------------------------------------------*/


void matmul_leaf(double **a, double **b, double **c, int NN)
{
	int i, j, k;

	for (i=0; i<NN; i++)
		for (j=0; j<NN; j++)
		{
			c[i][j] = 0;
			for (k=0; k<NN; k++)
				c[i][j] += a[i][k] * b[k][j];
		}

	return;
}


void matmul_strassen(volatile float * a, volatile float * b, volatile float * c, int NN)
{
	float **as, **bs, **cs;
	int i, j;
	int LEAF_SIZE;

    as = allocate_real_matrix(NN, -1);
    bs = allocate_real_matrix(NN, -1);
    cs = allocate_real_matrix(NN, -1);

    for (i=0; i<NN; i++)
    	for (j=0; j<NN; j++)
    	{
    		as[i][j] = a[i*NN+j];
    		bs[i][j] = b[i*NN+j];
    	}

	LEAF_SIZE = 32;
	strassen(as, bs, cs, NN, LEAF_SIZE);

    for (i=0; i<NN; i++)
    	for (j=0; j<NN; j++)
    	{
    		c[i*NN+j] = cs[i][j];
    	}

    as = free_real_matrix(as, NN);
    bs = free_real_matrix(bs, NN);
    cs = free_real_matrix(cs, NN);

    return;
}
#endif // __DO_STRASSEN__
