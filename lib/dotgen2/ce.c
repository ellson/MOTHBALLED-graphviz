static int Base = -1;		/* zero-based arrays */
#define Odd(n)	((n) % 2)
static int CountEdges(int Tree[], int n, int last)
{
	int		Pos, Sum;

	if (n == last) return 0;
	Pos = n + Base + 1;
	Sum = 0;
	while (Pos >= 1) {
		if (Odd(Pos))
			Sum = Sum + Tree[Pos - 1];
		Pos = Pos / 2;
	}
	return (Tree[1 + Base] - Sum);
}
