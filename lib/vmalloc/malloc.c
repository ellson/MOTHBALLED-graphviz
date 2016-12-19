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

#include	"vmhdr.h"

#if _std_malloc || _BLD_INSTRUMENT_ || cray
int _STUB_malloc;
#else

/*	malloc compatibility functions.
**	These are aware of debugging/profiling and driven by the environment variables:
**	VMETHOD: select an allocation method by name.
**
**	VMPROFILE: if is a file name, write profile data to it.
**	VMTRACE: if is a file name, write trace data to it.
**	The pattern %p in a file name will be replaced by the process ID.
**
**	VMDEBUG:
**		a:			abort on any warning
**		[decimal]:		period to check arena.
**		0x[hexadecimal]:	address to watch.
**
**	Written by Kiem-Phong Vo, kpv@research.att.com, 01/16/94.
*/

#if _hdr_stat
#include	<stat.h>
#else
#if _sys_stat
#include	<sys/stat.h>
#endif
#endif

#if defined(S_IRUSR)&&defined(S_IWUSR)&&defined(S_IRGRP)&&defined(S_IROTH)
#define CREAT_MODE	(S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)
#else
#define CREAT_MODE	0644
#endif

#undef malloc
#undef free
#undef realloc
#undef calloc
#undef cfree
#undef memalign
#undef valloc

static Vmulong_t atou(char **sp)
{
    char *s = *sp;
    Vmulong_t v = 0;

    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
	for (s += 2; *s; ++s) {
	    if (*s >= '0' && *s <= '9')
		v = (v << 4) + (*s - '0');
	    else if (*s >= 'a' && *s <= 'f')
		v = (v << 4) + (*s - 'a') + 10;
	    else if (*s >= 'A' && *s <= 'F')
		v = (v << 4) + (*s - 'A') + 10;
	    else
		break;
	}
    } else {
	for (; *s; ++s) {
	    if (*s >= '0' && *s <= '9')
		v = v * 10 + (*s - '0');
	    else
		break;
	}
    }

    *sp = s;
    return v;
}

static int _Vmflinit = 0;
static Vmulong_t _Vmdbcheck = 0;
static Vmulong_t _Vmdbtime = 0;
static int _Vmpffd = -1;
#define VMFLINIT() \
	{ if(!_Vmflinit)	vmflinit(); \
	  if(_Vmdbcheck && (++_Vmdbtime % _Vmdbcheck) == 0 && \
	     Vmregion->meth.meth == VM_MTDEBUG) \
		vmdbcheck(Vmregion); \
	}

static char *insertpid(char *begs, char *ends)
{
    int pid;
    char *s;

    if ((pid = getpid()) < 0)
	return NIL(char *);

    s = ends;
    do {
	if (s == begs)
	    return NIL(char *);
	*--s = '0' + pid % 10;
    } while ((pid /= 10) > 0);
    while (s < ends)
	*begs++ = *s++;

    return begs;
}

static int createfile(char *file)
{
    char buf[1024];
    char *next, *endb;

    next = buf;
    endb = buf + sizeof(buf);
    while (*file) {
	if (*file == '%') {
	    switch (file[1]) {
	    case 'p':
		if (!(next = insertpid(next, endb)))
		    return -1;
		file += 2;
		break;
	    default:
		goto copy;
	    }
	} else {
	  copy:
	    *next++ = *file++;
	}

	if (next >= endb)
	    return -1;
    }

    *next = '\0';
    return creat(buf, CREAT_MODE);
}

static void pfprint(void)
{
    if (Vmregion->meth.meth == VM_MTPROFILE)
	vmprofile(Vmregion, _Vmpffd);
}

static int vmflinit(void)
{
    char *env;
    Vmalloc_t *vm;
    int fd;
    Vmulong_t addr;
    char *file;
    int line;

    /* this must be done now to avoid any inadvertent recursion (more below) */
    _Vmflinit = 1;
    VMFILELINE(Vmregion, file, line);

    /* if getenv() calls malloc(), this may not be caught by the eventual region */
    vm = NIL(Vmalloc_t *);
    if ((env = getenv("VMETHOD"))) {
	if (strcmp(env, "Vmdebug") == 0 || strcmp(env, "vmdebug") == 0)
	    vm = vmopen(Vmdcsbrk, Vmdebug, 0);
	else if (strcmp(env, "Vmprofile") == 0
		 || strcmp(env, "vmprofile") == 0)
	    vm = vmopen(Vmdcsbrk, Vmprofile, 0);
	else if (strcmp(env, "Vmlast") == 0 || strcmp(env, "vmlast") == 0)
	    vm = vmopen(Vmdcsbrk, Vmlast, 0);
	else if (strcmp(env, "Vmpool") == 0 || strcmp(env, "vmpool") == 0)
	    vm = vmopen(Vmdcsbrk, Vmpool, 0);
	else if (strcmp(env, "Vmbest") == 0 || strcmp(env, "vmbest") == 0)
	    vm = Vmheap;
    }

    if ((!vm || vm->meth.meth == VM_MTDEBUG) &&
	(env = getenv("VMDEBUG")) && env[0]) {
	if (vm || (vm = vmopen(Vmdcsbrk, Vmdebug, 0))) {
	    reg int setcheck = 0;

	    while (*env) {
		if (*env == 'a')
		    vmset(vm, VM_DBABORT, 1);

		if (*env < '0' || *env > '9')
		    env += 1;
		else if (env[0] == '0' && (env[1] == 'x' || env[1] == 'X')) {
		    if ((addr = atou(&env)) != 0)
			vmdbwatch((void *) addr);
		} else {
		    _Vmdbcheck = atou(&env);
		    setcheck = 1;
		}
	    }
	    if (!setcheck)
		_Vmdbcheck = 1;
	}
    }

    if ((!vm || vm->meth.meth == VM_MTPROFILE) &&
	(env = getenv("VMPROFILE")) && env[0]) {
	_Vmpffd = createfile(env);
	if (!vm)
	    vm = vmopen(Vmdcsbrk, Vmprofile, 0);
    }

    /* slip in the new region now so that malloc() will work fine */
    if (vm)
	Vmregion = vm;

    /* turn on tracing if requested */
    if ((env = getenv("VMTRACE")) && env[0] && (fd = createfile(env)) >= 0) {
	vmset(Vmregion, VM_TRACE, 1);
	vmtrace(fd);
    }

    /* make sure that profile data is output upon exiting */
    if (vm && vm->meth.meth == VM_MTPROFILE) {
	if (_Vmpffd < 0)
	    _Vmpffd = 2;
	/* this may wind up calling malloc(), but region is ok now */
	atexit(pfprint);
    } else if (_Vmpffd >= 0) {
	close(_Vmpffd);
	_Vmpffd = -1;
    }

    /* reset file and line number to correct values for the call */
    Vmregion->file = file;
    Vmregion->line = line;

    return 0;
}

void *malloc(reg size_t size)
{
    VMFLINIT();
    return (*Vmregion->meth.allocf) (Vmregion, size);
}

/**
 * @param data block to be reallocated
 * @param size new size
 */
void *realloc(reg void * data, reg size_t size)
{
    VMFLINIT();
    return (*Vmregion->meth.resizef) (Vmregion, data, size,
				      VM_RSCOPY | VM_RSMOVE);
}

void free(reg void * data)
{
    VMFLINIT();
    (void) (*Vmregion->meth.freef) (Vmregion, data);
}

void *calloc(reg size_t n_obj, reg size_t s_obj)
{
    VMFLINIT();
    return (*Vmregion->meth.resizef) (Vmregion, NIL(void *),
				      n_obj * s_obj, VM_RSZERO);
}

void cfree(reg void * data)
{
    VMFLINIT();
    (void) (*Vmregion->meth.freef) (Vmregion, data);
}

void *memalign(reg size_t align, reg size_t size)
{
    VMFLINIT();
    return (*Vmregion->meth.alignf) (Vmregion, size, align);
}

void *valloc(reg size_t size)
{
    VMFLINIT();
    GETPAGESIZE(_Vmpagesize);
    return (*Vmregion->meth.alignf) (Vmregion, size, _Vmpagesize);
}

#if _hdr_malloc

#define calloc	______calloc
#define free	______free
#define malloc	______malloc
#define realloc	______realloc

#include	<malloc.h>

/* in Windows, this is a macro defined in malloc.h and not a function */
#undef alloca

#if _lib_mallopt
int mallopt(int cmd, int value)
{
    VMFLINIT();
    return 0;
}
#endif

#if _lib_mallinfo
struct mallinfo mallinfo(void)
{
    Vmstat_t sb;
    struct mallinfo mi;

    VMFLINIT();
    memset(&mi, 0, sizeof(mi));
    if (vmstat(Vmregion, &sb) >= 0) {
	mi.arena = sb.extent;
	mi.ordblks = sb.n_busy + sb.n_free;
	mi.uordblks = sb.s_busy;
	mi.fordblks = sb.s_free;
    }
    return mi;
}
#endif

#if _lib_mstats
struct mstats mstats(void)
{
    Vmstat_t sb;
    struct mstats ms;

    VMFLINIT();
    memset(&ms, 0, sizeof(ms));
    if (vmstat(Vmregion, &sb) >= 0) {
	ms.bytes_total = sb.extent;
	ms.chunks_used = sb.n_busy;
	ms.bytes_used = sb.s_busy;
	ms.chunks_free = sb.n_free;
	ms.bytes_free = sb.s_free;
    }
    return ms;
}
#endif

#endif/*_hdr_malloc*/

#endif /*_std_malloc || _BLD_INSTRUMENT_ || cray*/
