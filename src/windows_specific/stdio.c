/*
 * Rufus: The Reliable USB Formatting Utility
 * Standard User I/O Routines (logging, status, error, etc.)
 * Copyright © 2011-2024 Pete Batard <pete@akeo.ie>
 * Copyright © 2020 Mattiwatti <mattiwatti@gmail.com>
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


 #ifdef _CRTDBG_MAP_ALLOC
 #include <stdlib.h>
 #include <crtdbg.h>
 #endif
 
 #include <pseudo_windows.h>
 #include <windowsx.h>
 #include <stdio.h>
 #include <string.h>
 #include <stdlib.h>
 #include <wininet.h>
 #include <winternl.h>
 #include <dbghelp.h>
 #include <assert.h>
 #include <ctype.h>
 #include <math.h>
 
 #include "rufus.h"
 #include "missing.h"
 #include "settings.h"
 #include "resource.h"
 #include "msapi_utf8.h"
 #include "localization.h"
 #include "bled/bled.h"


 /*
 * Globals
 */

 const HANDLE hRufus = (HANDLE)0x0000005275667573ULL;	// "\0\0\0Rufus"
 size_t ubuffer_pos = 0;