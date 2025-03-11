/*
 * Rufus: The Reliable USB Formatting Utility
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


#ifndef _WIN32
#include "pseudo_windows.h"
#include <wctype.h>



// This can and should be done better,
// but will work in this case!
DWORD CharUpperBuffW(WCHAR *str, DWORD len)
{
    for (DWORD i = 0; i < len; i++)
    {
    	str[i] = (WCHAR) towupper((wint_t) str[i]);
    }
    return len;
}

#endif