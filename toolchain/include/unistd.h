#ifndef _UNISTD_H
/*
 * This file is part of the Mingw32 package.
 *
 * unistd.h maps (roughly) to io.h
 * Other headers included by unistd.h may be selectively processed;
 * __UNISTD_H_SOURCED__ enables such selective processing.
 */
#define _UNISTD_H
#define __UNISTD_H_SOURCED__ 1

#include <io.h>
#include <process.h>
#include <getopt.h>

/* These are also defined in stdio.h. */
#ifndef	SEEK_SET
#define SEEK_SET 0
#endif

#ifndef	SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Compatibility modification
// sleep and usleep in MinGW are not defined by default
// the following macros make use of the Windows Sleep function instead (drahosp)
#include <Windows.h>
#define sleep(x) Sleep((x)*1000)

#if !defined __NO_ISOCEXT
#include <sys/types.h> /* For useconds_t. */

// Using Windows Sleep instead of the built in implementation
#define usleep(x) Sleep((x)/1000.0f+0.5f)
//int __cdecl __MINGW_NOTHROW usleep(useconds_t useconds);
#endif  /* Not __NO_ISOCEXT */

/* This is defined as a real library function to allow autoconf
   to verify its existence. */
int ftruncate(int, off_t);
#ifndef __NO_INLINE__
__CRT_INLINE int ftruncate(int __fd, off_t __length)
{
  return _chsize (__fd, __length);
}
#endif

#ifdef __cplusplus
}
#endif

#undef __UNISTD_H_SOURCED__
#endif /* _UNISTD_H */
