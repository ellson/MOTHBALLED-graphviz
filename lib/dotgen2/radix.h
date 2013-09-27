typedef struct radixrec_s {
	unsigned long	key;
	void	*data;
} radixrec_t;

void radix_sort(radixrec_t *source, unsigned long N);
