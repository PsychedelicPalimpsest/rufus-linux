/*
 * Rufus: The Reliable USB Formatting Utility
 * Copyright © 2025 PsychedelicPalimpsest
 *
 * A bare bones implementation of some windows functions.
 * Some are stollen from Wine, some are PsychedelicPalimpsest
 * originals! Just acts like the normal windows.h on windows though!
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifdef _WIN32
#include <windows.h>
#else
#include <stddef.h>

/**
 * I know this is not one to one, but just working
 * with raw unix file ids and pretending they are
 * Windows HANDLEs will work well enough for Rufus
 */



#define INVALID_HANDLE_VALUE ((int) -1)
typedef int HANDLE;

#include <stdlib.h>

// For some reason libbb break when _mm_malloc is defined
#ifndef LIBBB_H
#define _mm_malloc(SIZE_OF, ALLIGN_OF) aligned_alloc(ALLIGN_OF, SIZE_OF)
#define _mm_free free
#endif

#include "../linux_specific/mini_winnt.h"
#include "../linux_specific/locale.h"

#include <errno.h>
#include <assert.h>
#include <inttypes.h>
#include <string.h>

// Is this a close enough aproximation?
#define WindowsErrorString() strerror(errno)


#define _snprintf_s(a,b,c,...) snprintf(a,b,__VA_ARGS__)


#include <sys/stat.h>
#include <fcntl.h>

#define _strdup strdup
#define _chmod chmod
/** The following is my best attempt to Emulate the windows file parameters.
 *  Unfortinatly I can not emulate them all one-to-one
 */

#define _O_RDONLY      O_RDONLY
#define _O_WRONLY      O_WRONLY
#define _O_RDWR        O_RDWR
#define _O_ACCMODE     O_ACCMODE
#define _O_APPEND      O_APPEND
#define _O_CREAT       O_CREAT
#define _O_TRUNC       O_TRUNC
#define _O_EXCL        O_EXCL

/* Linux has no direct file open flag for random or sequential access.  */
#define _O_RANDOM      0
#define _O_SEQUENTIAL  0

/* _O_TEMPORARY: Windows flag for temporary files.
   On Linux you might consider O_TMPFILE (if available and with proper usage)
   or use mkstemp() and unlink the file immediately.
   Adjust based on your needs. */
#ifdef O_TMPFILE
#define _O_TEMPORARY   O_TMPFILE
#else
#define _O_TEMPORARY   0
#endif

/* _O_NOINHERIT: No direct equivalent.
   To prevent descriptor inheritance in Linux, set FD_CLOEXEC via fcntl(). */
#define _O_NOINHERIT   0

/* Linux does not differentiate between text and binary modes.
   _O_TEXT and _O_BINARY are defined as 0. */
#define _O_TEXT        0
#define _O_BINARY      0



/** And here are the permisions. Not all are exact 
 */
#define _S_IEXEC   S_IXUSR
#define _S_IWRITE  S_IWUSR
#define _S_IREAD   S_IRUSR
#define _S_IFIFO   S_IFIFO
#define _S_IFCHR   S_IFCHR
#define _S_IFDIR   S_IFDIR
#define _S_IFREG   S_IFREG
#define _S_IFMT    S_IFMT



#define _SH_DENYNO -1
// NOTE: This removes ALL safety. I need to find a better method, until then
// lets see if this bugs anything out!
static __inline int _sopen_s(int *pfh, const char *filename, int flags, int share, int mode)
{
    (void) share;  // Unused in POSIX
    int fd = open(filename, flags, mode);
    if (fd == -1) return errno;
    *pfh = fd;
    return 0;
}
/* Error Masks */
#define APPLICATION_ERROR_MASK       0x20000000
#define ERROR_SEVERITY_SUCCESS       0x00000000
#define ERROR_SEVERITY_INFORMATIONAL 0x40000000
#define ERROR_SEVERITY_WARNING       0x80000000
#define ERROR_SEVERITY_ERROR         0xC0000000

#include "../linux_specific/winerror.h"
#include "../linux_specific/win_fs_functions.h"





// Might not exist
#ifndef STRUNCATE
#define STRUNCATE 80
#endif

#define _TRUNCATE  ((size_t)-1)
// Should be the same as the microsofty implementation
int strncat_s(char *dest, size_t destsz, const char *src, size_t count);




DWORD CharUpperBuffW(WCHAR *str, DWORD len);
ULONGLONG GetTickCount64(void);

#endif
