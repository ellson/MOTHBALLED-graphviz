/* $Id$ $Revision$ */
/* vim:set shiftwidth=4 ts=8: */

/*************************************************************************
 * Copyright (c) 2011 AT&T Intellectual Property 
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors: See CVS logs. Details at http://www.graphviz.org/
 *************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

static int openF(char *fname)
{
    char buf[BUFSIZ];
    int fd = open(fname, O_RDONLY);

    if (fd < 0) {
	sprintf(buf, "Could not open %s for reading\n", fname);
	perror(buf);
	exit(1);
    }
    return fd;
}

static int fileSize(int fd)
{
    struct stat sb;

    fstat(fd, &sb);
    return sb.st_size;
}

main(int argc, char *argv[])
{
    register char *p1;
    register char *p2;
    register int i, count;
    int f1, f2;
    int l1, l2;
    int n1, n2, n;
    int xval, base, len;
    char buf1[BUFSIZ];
    char buf2[BUFSIZ];

    if (argc != 3) {
	fprintf(stderr, "Usage: %s <file1> <file2>\n", argv[0]);
	exit(1);
    }

    f1 = openF(argv[1]);
    f2 = openF(argv[2]);

    l1 = fileSize(f1);
    l2 = fileSize(f2);
    if (l1 < l2) {
	len = l1;
	base = l2;
	count = l2 - l1;
    } else {
	len = l2;
	base = l1;
	count = l1 - l2;
    }

    i = 0;
    while (i < len) {
	n1 = read(f1, buf1, BUFSIZ);
	n2 = read(f2, buf2, BUFSIZ);
	if (n1 < n2)
	    n = i + n1;
	else
	    n = i + n2;
	p1 = buf1;
	p2 = buf2;
	while (i < n) {
	    if (*p1++ != *p2++)
		count++;
	    i++;
	}
    }

    if (count) {
	printf("%d/%d(%f)\n", count, base, ((double) count) / base);
	xval = 2;
    } else
	xval = 0;

    exit(xval);

}
