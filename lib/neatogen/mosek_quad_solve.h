#ifdef MOSEK
#ifndef _QSOLVE_H_
#define _QSOLVE_H_

#include <mosek.h> /* Include the MOSEK definition file. */
#include "types.h"
#include <csolve_VPSC.h>

typedef struct {
	int r;
	MSKenv_t  env;
	MSKtask_t task;
	double *qval;
	int *qsubi,*qsubj;
	double *xx;
	int num_variables;
} MosekEnv;

MosekEnv* mosek_init_hier(float* lap, int n,int *ordering,int *level_indexes,int num_divisions, float separation);
void mosek_quad_solve_hier(MosekEnv*,float *b,int n,float* coords, float *hierarchy_boundaries);
MosekEnv* mosek_init_sep(float* lap, int nv, int ndv, Constraint** cs, int m);
void mosek_quad_solve_sep(MosekEnv*,int n,float *b,float* coords);
void mosek_delete(MosekEnv*);


#endif // _QSOLVE_H_
#endif // MOSEK
