/* 
	http://www.cubic.org/~submissive/sourcerer/radix.htm
	http://www.cubic.org/~submissive/sourcerer/download/radix_ar_2001.c
*/

#include <stdlib.h>
#include <assert.h>

// complicated expression better fits as macro (or inline in C++)
#define ByteOf(x) (*(uchar *) (x))

typedef unsigned long ulong;
typedef unsigned char uchar;

typedef struct rec_s {
	ulong		key;
} rec;

// replaced byte with bitsOffset to avoid *8 operation in loop
static void radix (short byteOffset, ulong N,  rec *source, rec *dest)
{
// suppressed the need for index as it is reported in count
        ulong count[256];
// added temp variables to simplify writing, understanding and compiler optimization job
// most of them will be allocated as registers
        ulong *cp, s, c, i;
        uchar *bp;
		rec   *sp;

// faster than MemSet
        cp = count;
        for (i = 256; i > 0; --i, ++cp)
                *cp = 0;

// count occurrences of every byte value
        bp = ((uchar *) source) + byteOffset;
        for (i = N; i > 0; --i, bp += sizeof(rec)) {
                cp = count + *bp;
                ++(*cp);
        }

// transform count into index by summing elements and storing into same array
        s = 0;
        cp = count;
        for (i = 256; i > 0; --i, ++cp) {
                c = *cp;
                *cp = s;
                s += c;
        }

// fill dest with the right values in the right place
        bp = ((uchar *) source) + byteOffset;
        sp = source;
        for (i = N; i > 0; --i, bp += sizeof(rec), ++sp) {
                cp = count + *bp;
                dest[*cp] = *sp;
                ++(*cp);
        }
}

static void radix_sort (rec *source, ulong N)
{
// allocate heap memory to avoid the need of additional parameter
        rec *temp = (rec *) malloc (N * sizeof (rec));
        assert (temp != NULL);

        radix (3, N, source, temp);
        radix (2, N, temp, source);
        radix (1, N, source, temp);
        radix (0, N, temp, source);

        free (temp);
}

static void make_random (rec *data, ulong N)
{
        for ( ; N > 0; --N, ++data)
                data->key = rand () | (rand () << 16);
}

static void check_order (rec *data, ulong N)
{
// only signal errors if any (should not be)
        --N;
        for ( ; N > 0; --N, ++data)
                assert (data[0].key <= data[1].key);
}

// test for big number of elements
static void test_radix (ulong N)
{
        rec *data = (rec *) malloc (N * sizeof (rec));
        assert (data != NULL);

        make_random (data, N);
        radix_sort (data, N);
        check_order (data, N);

        free (data);
}

int main (int argc, const char ** argv)
{
        test_radix (5000000);
}
