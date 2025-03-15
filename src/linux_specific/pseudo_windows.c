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

// Found in the common/ dir
#include "pseudo_windows.h"
#include <wctype.h>



#include <time.h>


ULONGLONG GetTickCount64(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ULONGLONG)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}



int strncat_s(char *dest, size_t destsz, const char *src, size_t count){
	size_t start;
	int i;

	if (dest == NULL || src == NULL || destsz == 0) return EINVAL;
	start = strlen(dest);
	
	
	// Can not even add one char
	if (start+1 >= destsz){
		dest[0] = 0;
		return ERANGE;
	}

	// Only copy while it is safe and while it has something to copy
	for(i = 0; i + start < destsz - 1 && src[i] && i < count; i++){
		dest[i + start] = src[i];
	}

	dest[i + start] = 0;

    // Something went wrong
    if (src[i] && (count == _TRUNCATE || i == count)) return STRUNCATE;

	return 0;
}

int lstrcmpiA(const char *str1, const char *str2)
{
    if (!str1 && !str2) return 0;
    if (!str1) return -1;
    if (!str2) return 1;

    while (*str1 && *str2) {
        unsigned char c1 = tolower((unsigned char)*str1);
        unsigned char c2 = tolower((unsigned char)*str2);
        if (c1 != c2) return c1 - c2;
        str1++;
        str2++;
    }
    return tolower((unsigned char)*str1) - tolower((unsigned char)*str2);
}