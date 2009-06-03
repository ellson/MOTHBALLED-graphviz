#include "mmio.h"
#include "SparseMatrix.h"
int mm_get_type(MM_typecode typecode);
void SparseMatrix_export_matrix_market(FILE *file, SparseMatrix A, char *comment);
SparseMatrix SparseMatrix_import_matrix_market(FILE *f, int format);
