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



#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <sys/xattr.h>


int create_file_linux(const char *name, int access, int creation, int attributes) {
    int flags = access; // Start with read/write flags
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // Default 0644

    switch (creation) {
        case CREATE_NEW:        flags |= O_CREAT | O_EXCL; break;
        case CREATE_ALWAYS:     flags |= O_CREAT | O_TRUNC; break;
        case OPEN_EXISTING:     break; // No flags needed, just use access
        case OPEN_ALWAYS:       flags |= O_CREAT; break;
        case TRUNCATE_EXISTING: flags |= O_TRUNC; break;
        default: return -1;
    }

    int fd = open(name, flags, mode);
    if (fd < 0) return -1;

    // Handle read-only attribute
    if (attributes & FILE_ATTRIBUTE_READONLY) {
        chmod(name, S_IRUSR | S_IRGRP | S_IROTH);
    }

    // Handle hidden attribute (prefix with dot)
    if (attributes & FILE_ATTRIBUTE_HIDDEN) {
        char hidden_name[256];
        snprintf(hidden_name, sizeof(hidden_name), ".%s", name);
        rename(name, hidden_name);
    }

    return fd;
}
