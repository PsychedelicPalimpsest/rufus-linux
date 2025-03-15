/*
* Rufus: The Reliable USB Formatting Utility
* Windows I/O redefinitions, but for linux!
* Copyright © 2025 PsychedelicPalimpsest
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

#include <pseudo_windows.h>
#include "winio.h"
#include "rufus.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <aio.h>

typedef struct {
    int fd;               // File descriptor
    off_t offset;         // File offset

    struct aiocb cb;
} ASYNC_FD;


ASYNC_FD* CreateFileAsync(LPCSTR lpFileName, DWORD dwDesiredAccess,
	DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes){
    ASYNC_FD* afd = calloc(1, sizeof(ASYNC_FD));
    if (!afd) return NULL;
    
    afd->fd = create_file_linux(lpFileName, windowsAccessToLinux(dwDesiredAccess), dwCreationDisposition, dwFlagsAndAttributes);
    if (afd->fd == -1){
        free(afd);
        return NULL;
    }
    return afd;
}

#define _IO_FUNC_GEN(IO_FUNC_NAME, BYTES_COUNT)\
    if (aio_error(&h->cb) == EINPROGRESS){\
        fprintf(stderr, "ERROR: " STRINGIFY(IO_FUNC_NAME) "() attempted while still preforming operation!\n");\
        return 0;\
    }\
    int r = aio_return(&h->cb);\
    if (r > 0)\
        h->offset += r; \
    \
    memset(&h->cb, 0, sizeof(struct aiocb));\
    h->cb.aio_fildes = h->fd;\
    h->cb.aio_buf = lpBuffer;\
    h->cb.aio_nbytes = BYTES_COUNT;\
    h->cb.aio_offset = h->offset;\
    if (-1 == IO_FUNC_NAME(&h->cb))\
        return 0;\
    \
    int stat = aio_error(&h->cb);\
    if (stat == 0 || stat == EINPROGRESS)\
        return 1;\
    \
    return 0;


BOOL ReadFileAsync(ASYNC_FD* h, LPVOID lpBuffer, DWORD nNumberOfBytesToRead){
    _IO_FUNC_GEN(aio_read, nNumberOfBytesToRead);
}

BOOL WriteFileAsync(ASYNC_FD* h, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite){
    _IO_FUNC_GEN(aio_write, nNumberOfBytesToWrite);
}


BOOL WaitFileAsync(ASYNC_FD* h, DWORD dwTimeout){
    const struct aiocb* cblist[1] = { &h->cb };

    struct timespec timeout;
    timeout.tv_sec = dwTimeout / 1000;
    timeout.tv_nsec = (dwTimeout % 1000) * 1000000000;

    // Wait until the asynchronous operation completes or timeout occurs.
    if (aio_suspend(cblist, 1, &timeout) == -1)
        return 0;  // Timeout or error occurred

    int status = aio_error(&h->cb);
    if (status == EINPROGRESS)
        return 0;

    return (status == 0);
}


// Not actually async
BOOL GetSizeAsync(ASYNC_FD* h, LPDWORD lpNumberOfBytes){
    int r = aio_return(&h->cb);
    if (r > 0)
        h->offset += r;
    
    memset(&h->cb, 0, sizeof(struct aiocb));

    *lpNumberOfBytes = h->offset;
    return 1;
}


void CloseFileAsync(ASYNC_FD* h){
    if (!h) return;
    close(h->fd);
    free(h);
}
