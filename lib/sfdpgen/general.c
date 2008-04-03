#include "general.h"

real drand(){
  return rand()/(real) RAND_MAX;
}

int irand(int n){
  /* 0, 1, ..., n-1 */
  assert(n > 1);
  /*return (int) MIN(floor(drand()*n),n-1);*/
  return rand()%n;
}

int *random_permutation(int n){
  int *p;
  int i, j, pp, len;
  if (n <= 0) return NULL;
  p = MALLOC(sizeof(int)*n);
  for (i = 0; i < n; i++) p[i] = i;

  len = n;
  while (len > 1){
    j = irand(len);
    pp = p[len-1];
    p[len-1] = p[j];
    p[j] = pp;
    len--;
  }
  return p;
}


real* vector_subtract_to(int n, real *x, real *y){
  /* y = x-y */
  int i;
  for (i = 0; i < n; i++) y[i] = x[i] - y[i];
  return y;
}

real vector_product(int n, real *x, real *y){
  real res = 0;
  int i;
  for (i = 0; i < n; i++) res += x[i]*y[i];
  return res;
}

real* vector_saxpy(int n, real *x, real *y, real beta){
  /* y = x+beta*y */
  int i;
  for (i = 0; i < n; i++) y[i] = x[i] + beta*y[i];
  return y;
}

real* vector_saxpy2(int n, real *x, real *y, real beta){
  /* x = x+beta*y */
  int i;
  for (i = 0; i < n; i++) x[i] = x[i] + beta*y[i];
  return x;
}

void vector_print(char *s, int n, real *x){
  int i;
    printf("%s{",s); 
    for (i = 0; i < n; i++) {
      if (i > 0) printf(",");
      printf("%f",x[i]); 
    }
    printf("}\n");
}


int excute_system_command3(char *s1, char *s2, char *s3){
  char c[1000];

  strcpy(c, s1);
  strcat(c, s2);
  strcat(c, s3);
  return system(c);
}

int excute_system_command(char *s1, char *s2){
  char c[1000];

  strcpy(c, s1);
  strcat(c, s2);
  return system(c);
}
