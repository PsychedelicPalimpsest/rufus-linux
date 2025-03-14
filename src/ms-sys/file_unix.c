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

#ifdef __linux__

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "rufus.h"
#include "file.h"

#include <unistd.h>
#include <errno.h>

extern unsigned long ulBytesPerSector;



int64_t write_sectors(HANDLE hDrive, uint64_t SectorSize,
                             uint64_t StartSector, uint64_t nSectors,
                             const void *pBuf)
{
    if((nSectors*SectorSize) > 0xFFFFFFFFUL)
    {
        uprintf("write_sectors: nSectors x SectorSize is too big\n");
        return -1;
    }
    ssize_t size = (ssize_t) nSectors*SectorSize;
    if (-1 == lseek(hDrive, StartSector*SectorSize, SEEK_SET))
    {
        uprintf("write_sectors: nSectors x SectorSize is too big\nErrno: %s\n", strerror(errno));
        errno = 0;
        return -1;
    }
    ssize_t wret = write(hDrive, pBuf, size);

    if (size != wret)
    {
      /* Some large drives return 0, even though all the data was written - See github #787 */
      if (large_drive && size == 0) {
         uprintf("Warning: Possible short write\n");
         return 0;
      }
      uprintf("write_sectors: Write error\nErrno: %s\n", strerror(errno));
      uprintf("  Wrote: %d, Expected: %" PRIu64 "\n", wret, nSectors*SectorSize);
      uprintf("  StartSector: 0x%08" PRIx64 ", nSectors: 0x%" PRIx64 ", SectorSize: 0x%" PRIx64 "\n", StartSector, nSectors, SectorSize);
      errno = 0;
    }

    return (int64_t)size;
}


int64_t read_sectors(HANDLE hDrive, uint64_t SectorSize,
                     uint64_t StartSector, uint64_t nSectors,
                     void *pBuf)
{
    if((nSectors*SectorSize) > 0xFFFFFFFFUL)
    {
        uprintf("read_sectors: nSectors x SectorSize is too big\n");
        return -1;
    }
    uint64_t size = nSectors*SectorSize;
    if (-1 == lseek(hDrive, StartSector*SectorSize, SEEK_SET))
    {
        uprintf("read_sectors: nSectors x SectorSize is too big\nErrno: %s\n", strerror(errno));
        errno = 0;
        return -1;
    }
    ssize_t rret = read(hDrive, pBuf, size);
    if (rret != size)
    {
		uprintf("read_sectors: Read error %s\n", strerror(errno));
		uprintf("  Read: %d, Expected: %" PRIu64 "\n",  rret, nSectors*SectorSize);
		uprintf("  StartSector: 0x%08" PRIx64 ", nSectors: 0x%" PRIx64 ", SectorSize: 0x%" PRIx64 "\n", StartSector, nSectors, SectorSize);
		errno = 0;
    }
    return rret;
}

#endif /* __linux__ */



