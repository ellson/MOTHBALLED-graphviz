/*
From: Andre Reinald <Andre.Reinald@vtcom.fr>
To: submissive@cubic.org

I though you might be intersted by some modifications I brought to your
code in order to improve legibility and slightly speed.

Well, my source is commented and I often wrote 4 lines where you had
only one, but the generated code is smaller and it gives more
opportunity to check intermediate steps while debugging.

By the way, I did so on a PowerMac with CodeWarrior compiler, so your
source is really portable.
*/

/* hacked by north to support a data pointer with each record */

#include <stdlib.h>
#include <assert.h>
#include "radix.h"

/* complicated expression better fits as macro (or inline in C++) */
#define ByteOf(x) (((x) >> bitsOffset) & 0xff)

typedef unsigned long ulong;
typedef radixrec_t rec;

/* replaced byte with bitsOffset to avoid *8 operation in loop */
static void radix (short bitsOffset, ulong N, rec *source, rec *dest)
{
/* suppressed the need for index as it is reported in count */
        ulong count[256];
/* added temp variables to simplify writing, understanding and compiler optimization job */
/* most of them will be allocated as registers */
        ulong *cp, c, i, s;
		rec	  *sp, srec;

/* faster than MemSet */
        cp = count;
        for (i = 256; i > 0; --i, ++cp)
                *cp = 0;

/* count occurrences of every byte value */
        sp = source;
        for (i = N; i > 0; --i, ++sp) {
                cp = count + ByteOf (sp->key);
                ++(*cp);
        }

/* transform count into index by summing elements and storing into same array */
        s = 0;
        cp = count;
        for (i = 256; i > 0; --i, ++cp) {
                c = *cp;
                *cp = s;
                s += c;
        }

/* fill dest with the right values in the right place */
        sp = source;
        for (i = N; i > 0; --i, ++sp) {
                srec = *sp;
                cp = count + ByteOf (srec.key);
                dest[*cp] = srec;
                ++(*cp);
        }
}

void radix_sort (rec *source, ulong N)
{
/* allocate heap memory to avoid the need of additional parameter */
        rec *temp = malloc (N * sizeof (rec));
        assert (temp != NULL);

        radix (0, N, source, temp);
        radix (8, N, temp, source);
        radix (16, N, source, temp);
        radix (24, N, temp, source);

        free (temp);
}

#ifdef MAIN
static void make_random (rec *data, ulong N)
{
        for ( ; N > 0; --N, ++data) {
                data->key = rand () | (rand () << 16);
				data->data = (void*)((data->key)^0xdeadbeef);
		}
}

static void check_order (rec *data, ulong N)
{
/* only signal errors if any (should not be) */
        --N;
        for ( ; N > 0; --N, ++data) {
                assert (data[0].key <= data[1].key);
				assert (data->data == (void*)((data->key)^0xdeadbeef));
		}
}

/* test for big number of elements */
static void test_radix (ulong N)
{
        rec *data = malloc (N * sizeof (rec));
        assert (data != NULL);

        make_random (data, N);
        radix_sort (data, N);
        check_order (data, N);

        free (data);
}

void main (void)
{
        test_radix (10000000);
}
#endif
