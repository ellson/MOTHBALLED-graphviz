#ifndef GENERAL_H
#define GENERAL_H


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#define real double

#define set_flag(a, flag) ((a)=((a)|(flag)))
#define test_flag(a, flag) ((a)&(flag))
#define clear_flag(a, flag) ((a) &=(~(flag)))

#define MALLOC malloc
#define FREE free
#define REALLOC realloc
#define MEMCPY memcpy

#define N_NEW(n,t)   (t*)malloc((n)*sizeof(t))
#define NEW(t)       (t*)malloc(sizeof(t))

#ifndef DEBUG
#define NDEBUG /* switch off assert*/
#endif

#define MAX(a,b) ((a)>(b)?(a):b)
#define MIN(a,b) ((a)<(b)?(a):b)
#define ABS(a) (((a)>0)?(a):(-(a)))

#define TRUE 1
#define FALSE 0

extern int Verbose;

double _statistics[10];

int irand(int n);
real drand();
int *random_permutation(int n);/* random permutation of 0 to n-1 */

#define MAXINT 1<<30

#define PI 3.14159


real* vector_subtract_to(int n, real *x, real *y);

real vector_product(int n, real *x, real *y);

real* vector_saxpy(int n, real *x, real *y, real beta); /* y = x+beta*y */


real* vector_saxpy2(int n, real *x, real *y, real beta);/* x = x+beta*y */


void vector_print(char *s, int n, real *x);

#define MACHINEACC 1.0e-16

#define POINTS(inch) 72*(inch)

typedef unsigned int boolean;


int excute_system_command3(char *s1, char *s2, char *s3);
int excute_system_command(char *s1, char *s2);





#endif



