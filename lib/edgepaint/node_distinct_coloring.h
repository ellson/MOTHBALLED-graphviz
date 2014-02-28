enum {COLOR_RGB, COLOR_GRAY, COLOR_LAB};
enum {ERROR_BAD_LAB_GAMUT_FILE = -10, ERROR_BAD_COLOR_SCHEME = -9};
void node_distinct_coloring(char *color_scheme, char *lightness, int weightedQ, SparseMatrix A, real accuracy, int iter_max, int seed, int *cdim, real **colors, real *color_diff, real *color_diff_sum, int *flag);
