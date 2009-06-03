/* $Id$Revision:  */
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
#ifndef MATRIX_MARKET_H
#define MATRIX_MARKET_H

#include "mmio.h"
#include "SparseMatrix.h"
int mm_get_type(MM_typecode typecode);
void SparseMatrix_export_matrix_market(FILE * file, SparseMatrix A,
				       char *comment);
SparseMatrix SparseMatrix_import_matrix_market(FILE * f, int format);

#endif
