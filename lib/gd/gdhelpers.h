#ifdef __cplusplus
extern "C" {
#endif

#ifndef GDHELPERS_H
#define GDHELPERS_H 1

/* sys/types.h is needed for size_t on Sparc-SunOS-4.1 */
#include <sys/types.h>

/* TBB: strtok_r is not universal; provide an implementation of it. */

char *gd_strtok_r(char *s, char *sep, char **state);

/* These functions wrap memory management. gdFree is
        in gd.h, where callers can utilize it to correctly
        free memory allocated by these functions with the
        right version of free(). */
void *gdCalloc(size_t nmemb, size_t size);
void *gdMalloc(size_t size);
void *gdRealloc(void *ptr, size_t size);

/* Returns nonzero if multiplying the two quantities will
        result in integer overflow. Also returns nonzero if
        either quantity is negative. By Phil Knirsch based on
        netpbm fixes by Alan Cox. */

int overflow2(int a, int b);

/* 2.0.16: portable mutex support for thread safety. */

#ifdef DISABLE_THREADS
#define gdMutexDeclare(x)
#define gdMutexSetup(x)
#define gdMutexShutdown(x)
#define gdMutexLock(x)
#define gdMutexUnlock(x)
#else
#ifdef WIN32
/* 2.0.18: must include windows.h to get CRITICAL_SECTION. */
#include <windows.h>
#define gdMutexDeclare(x) CRITICAL_SECTION x
#define gdMutexSetup(x) InitializeCriticalSection(&x)
#define gdMutexShutdown(x) DeleteCriticalSection(&x)
#define gdMutexLock(x) EnterCriticalSection(&x)
#define gdMutexUnlock(x) LeaveCriticalSection(&x)
#else
#ifdef HAVE_PTHREAD
#include <pthread.h>
#define gdMutexDeclare(x) pthread_mutex_t x
#define gdMutexSetup(x) pthread_mutex_init(&x, 0)
#define gdMutexShutdown(x) pthread_mutex_destroy(&x)
#define gdMutexLock(x) pthread_mutex_lock(&x)
#define gdMutexUnlock(x) pthread_mutex_unlock(&x)
#else
#define gdMutexDeclare(x)
#define gdMutexSetup(x)
#define gdMutexShutdown(x)
#define gdMutexLock(x)
#define gdMutexUnlock(x)
#endif /* HAVE_PTHREAD */
#endif /* WIN32 */
#endif /* DISABLE_THREADS */

#endif /* GDHELPERS_H */

#ifdef __cplusplus
}
#endif
