/* print histogram of node degrees */
BEGIN {
  int degrees[];
  int maxd = 0;
  int i, d;
}
N{ 
  degrees[degree]++;
  if (degree > maxd) maxd = degree;
}
END {
  for (i = 1; i <= maxd; i++) {
    d = degrees[i];
    if (d > 0) printf ("[%d] %d\n", i, d);
  }
}
