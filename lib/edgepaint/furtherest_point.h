void furtherest_point(int k, int dim, real *wgt, real *pts, real *center, real width, int max_level, real (*usr_dist)(int, real*, real*), real *dist_max, real **argmax);
void furtherest_point_in_list(int k, int dim, real *wgt, real *pts, QuadTree qt, int max_level,
			      real (*usr_dist)(int, real*, real*), real *dist_max, real **argmax);
