/*
* Rufus: The Reliable USB Formatting Utility
* Some functions meant to simulate the windows FS apis.
* This file is not to be confused with winio.h
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

#pragma once

#include <sys/stat.h>
#include <stdio.h>

// Allow the underscore FS functions
#define _openU open
#define _mkdirU mkdir
#define _unlink unlink
#define fopenU fopen

#define DeleteFileW(PATH_NAME) !remove(PATH_NAME)
#define CreateFileW CreateFileA


#define CREATE_NEW        1
#define CREATE_ALWAYS     2
#define OPEN_EXISTING     3
#define OPEN_ALWAYS       4
#define TRUNCATE_EXISTING 5




int create_file_linux(const char *name, int access, int creation, int attributes);



static __inline BOOL PathFileExistsA(
  LPCSTR pszPath
){
	struct stat buff;
	int r = stat(pszPath, &buff);
	if(!r) return 1;
	
	// We don't want the errno to be poluted
	// when the file is not found.
	errno = 0;
	return 0;
}
static __inline BOOL MoveFileU(LPCSTR src, LPCSTR dest){
	return !rename(src, dest);
}



static __inline DWORD windowsAccessToLinux(DWORD access){
    if (access & GENERIC_ALL) return O_RDWR;
    if ((access & GENERIC_READ) && (access & GENERIC_WRITE)) return O_RDWR;
    if (access & GENERIC_READ) return O_RDONLY;
    if (access & GENERIC_WRITE) return O_WRONLY;
	return O_RDONLY;
}


#define _LINUX_FLAGS_TO_STR(FLAGS) ((FLAGS) & O_RDWR ? "r+" : ((FLAGS) & O_WRONLY ? "w" : "r"))

static __inline HANDLE CreateFileA(LPCSTR name, DWORD access, DWORD sharing,
	LPSECURITY_ATTRIBUTES sa, DWORD creation,
	DWORD attributes, HANDLE template){

	access = windowsAccessToLinux(access);
	int fd = create_file_linux(name, access, creation, attributes);
	if (fd == -1) return INVALID_HANDLE_VALUE;

	return fdopen(fd, _LINUX_FLAGS_TO_STR(access));
}
BOOL WriteFile(HANDLE handle, LPCVOID buffer, DWORD count, LPDWORD result, void* lpOverlapped_ignored){
	if (handle == NULL) return 0;
	FILE* f = (FILE*) handle;
	*result = fwrite(buffer, count, 1, f);
	
	return !errno;
}




static __inline VOID CloseHandle(HANDLE handle){
	FILE* f = (FILE*) handle;
	if (f == NULL) return;
	fclose(f);
}