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

// Allow the underscore FS functions
#define _openU open
#define _mkdirU mkdir
#define _unlink unlink

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