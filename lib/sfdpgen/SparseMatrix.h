/* vim:set shiftwidth=4 ts=8: */

/**********************************************************
*      This software is part of the graphviz package      *
*                http://www.graphviz.org/                 *
*                                                         *
*            Copyright (c) 1994-2004 AT&T Corp.           *
*                and is licensed under the                *
*            Common Public License, Version 1.0           *
*                      by AT&T Corp.                      *
*                                                         *
*        Information and Software Systems Research        *
*              AT&T Research, Florham Park NJ             *
**********************************************************/

#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include <stdio.h>
#include <sfdpinternal.h>

#define SYMMETRY_EPSILON 0.0000001
enum { FORMAT_CSC, FORMAT_CSR, FORMAT_COORD };
enum { UNMASKED = -10 };
enum { MATRIX_PATTERN_SYMMETRIC = 1 << 0, MATRIX_SYMMETRIC =
	1 << 1, MATRIX_SKEW = 1 << 2, MATRIX_HERMITIAN =
	1 << 3, MATRIX_UNDIRECTED = 1 << 4
};

typedef struct {
    int m;			/* row dimension */
    int n;			/* column dimension */
    int nz;			/* The actual length used is nz, for CSR/CSC matrix this is the same as ia[n] */
    int nzmax;			/* the current length of ja and a (if exists) allocated. */
    int type;			/* whether it is real/complex matrix, or pattern only */
    int *ia;			/* row pointer for CSR format, or row indices for coordinate format. 0-based */
    int *ja;			/* column indices. 0-based */
    void *a;			/* entry values. If NULL, pattern matrix */
    int format;			/* whether it is CSR, CSC, COORD. By default it is in CSR format */
    int property;		/* pattern_symmetric/symmetric/skew/hermitian */
} SparseMatrix;

enum { MATRIX_TYPE_REAL = 1 << 0, MATRIX_TYPE_COMPLEX =
	1 << 1, MATRIX_TYPE_INTEGER = 1 << 2, MATRIX_TYPE_PATTERN =
	1 << 3, MATRIX_TYPE_UNKNOWN = 1 << 4
};

SparseMatrix *SparseMatrix_new(int m, int n, int nz, int type, int format);

SparseMatrix *SparseMatrix_from_coordinate_format(SparseMatrix * A);

SparseMatrix *SparseMatrix_from_coordinate_arrays(int nz, int m, int n,
						  int *irn, int *jcn,
						  void *val, int type);

void SparseMatrix_print(char *, SparseMatrix * A);	/*print to stdout in Mathematica format */

void SparseMatrix_export(FILE * f, SparseMatrix * A);	/* export into MM format except the header */

SparseMatrix *SparseMatrix_import_binary(char *name);

void SparseMatrix_export_binary(char *name, SparseMatrix * A, int *flag);

void SparseMatrix_delete(SparseMatrix * A);

SparseMatrix *SparseMatrix_add(SparseMatrix * A, SparseMatrix * B);
SparseMatrix *SparseMatrix_multiply(SparseMatrix * A, SparseMatrix * B);
SparseMatrix *SparseMatrix_sum_repeat_entries(SparseMatrix * A);
SparseMatrix *SparseMatrix_coordinate_form_add_entries(SparseMatrix * A,
						       int nentries,
						       int *irn, int *jcn,
						       void *val);
int SparseMatrix_is_symmetric(SparseMatrix * A,
			      int test_pattern_symmetry_only);
SparseMatrix *SparseMatrix_transpose(SparseMatrix * A);
SparseMatrix *SparseMatrix_symmetrize(SparseMatrix * A,
				      int pattern_symmetric_only);
void SparseMatrix_multiply_vector(SparseMatrix * A, real * v, real ** res, int transposed);	/* if v = NULL, v is assumed to be {1,1,...,1} */
SparseMatrix *SparseMatrix_remove_diagonal(SparseMatrix * A);
SparseMatrix *SparseMatrix_remove_upper(SparseMatrix * A);	/* remove diag and upper diag */
SparseMatrix *SparseMatrix_divide_row_by_degree(SparseMatrix * A);
SparseMatrix *SparseMatrix_get_real_adjacency_matrix_symmetrized(SparseMatrix * A);	/* symmetric, all entries to 1, diaginal removed */
SparseMatrix *SparseMatrix_normalize_to_rowsum1(SparseMatrix * A);	/* for real only! */
void SparseMatrix_multiply_dense(SparseMatrix * A, int ATranspose,
				 real * v, int vTransposed, real ** res,
				 int res_transpose, int dim);
SparseMatrix *SparseMatrix_apply_fun(SparseMatrix * A, double (*fun) (double x));	/* for real only! */
SparseMatrix *SparseMatrix_copy(SparseMatrix * A);
int SparseMatrix_has_diagonal(SparseMatrix * A);
SparseMatrix *SparseMatrix_normalize_by_row(SparseMatrix * A);	/* divide by max of each row */
SparseMatrix *SparseMatrix_crop(SparseMatrix * A, real epsilon);	/*remove any entry <= epsilon */
SparseMatrix *SparseMatrix_scaled_by_vector(SparseMatrix * A, real * v,
					    int apply_to_row);
SparseMatrix *SparseMatrix_multiply_by_scaler(SparseMatrix * A, real s);
SparseMatrix *SparseMatrix_make_undirected(SparseMatrix * A);	/* make it strictly low diag only, and set flag to undirected */
int SparseMatrix_connectedQ(SparseMatrix * A);
int SparseMatrix_pseudo_diameter_only(SparseMatrix * A);
int SparseMatrix_pseudo_diameter(SparseMatrix * A0, int root, int aggressive, int *end1, int *end2, int *connectedQ);	/* assume unit edge length, unsymmetric matrix ill be symmetrized */
void SparseMatrix_level_sets(SparseMatrix * A, int root, int *nlevel,
			     int **levelset_ptr, int **levelset,
			     int **mask, int reintialize_mask);
void SparseMatrix_weakly_connected_components(SparseMatrix * A0,
					      int *ncomp, int **comps,
					      int **comps_ptr);
void SparseMatrix_decompose_to_supervariables(SparseMatrix * A,
					      int *ncluster, int **cluster,
					      int **clusterp);


#define set_flag(a, flag) ((a)=((a)|(flag)))
#define test_flag(a, flag) ((a)&(flag))
#define clear_flag(a, flag) ((a) &=(~(flag)))

#define SparseMatrix_set_undirected(A) set_flag((A)->property, MATRIX_UNDIRECTED)
#define SparseMatrix_set_symmetric(A) set_flag((A)->property, MATRIX_SYMMETRIC)
#define SparseMatrix_set_pattern_symmetric(A) set_flag((A)->property, MATRIX_PATTERN_SYMMETRIC)
#define SparseMatrix_set_skew(A) set_flag((A)->property, MATRIX_SKEW)
#define SparseMatrix_set_hemitian(A) set_flag((A)->property, MATRIX_HERMITIAN)

#define SparseMatrix_known_undirected(A) test_flag((A)->property, MATRIX_UNDIRECTED)
#define SparseMatrix_known_symmetric(A) test_flag((A)->property, MATRIX_SYMMETRIC)
#define SparseMatrix_known_strucural_symmetric(A) test_flag((A)->property, MATRIX_PATTERN_SYMMETRIC)
#define SparseMatrix_known_skew(A) test_flag((A)->property, MATRIX_SKEW)
#define SparseMatrix_known_hemitian(A) test_flag((A)->property, MATRIX_HERMITIAN)




#endif
