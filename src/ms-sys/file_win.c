/******************************************************************
    Copyright (C) 2009  Henrik Carlqvist
    Modified for Rufus/Windows (C) 2011-2019  Pete Batard

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
******************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "rufus.h"
#include "file.h"

#ifdef _WIN32

/* Returns the number of bytes written or -1 on error */
int64_t write_sectors(HANDLE hDrive, uint64_t SectorSize,
                      uint64_t StartSector, uint64_t nSectors,
                      const void *pBuf)
{
   LARGE_INTEGER ptr;
   DWORD Size;

   if((nSectors*SectorSize) > 0xFFFFFFFFUL)
   {
      uprintf("write_sectors: nSectors x SectorSize is too big\n");
      return -1;
   }
   Size = (DWORD)(nSectors*SectorSize);

   ptr.QuadPart = StartSector*SectorSize;
   if(!SetFilePointerEx(hDrive, ptr, NULL, FILE_BEGIN))
   {
      uprintf("write_sectors: Could not access sector 0x%08" PRIx64 " - %s\n", StartSector, WindowsErrorString());
      return -1;
   }

   LastWriteError = 0;
   if(!WriteFileWithRetry(hDrive, pBuf, Size, &Size, WRITE_RETRIES))
   {
      LastWriteError = RUFUS_ERROR(GetLastError());
      uprintf("write_sectors: Write error %s\n", WindowsErrorString());
      uprintf("  StartSector: 0x%08" PRIx64 ", nSectors: 0x%" PRIx64 ", SectorSize: 0x%" PRIx64 "\n", StartSector, nSectors, SectorSize);
      return -1;
   }
   if (Size != nSectors*SectorSize)
   {
      /* Some large drives return 0, even though all the data was written - See github #787 */
      if (large_drive && Size == 0) {
         uprintf("Warning: Possible short write\n");
         return 0;
      }
      uprintf("write_sectors: Write error\n");
      LastWriteError = RUFUS_ERROR(ERROR_WRITE_FAULT);
      uprintf("  Wrote: %d, Expected: %" PRIu64 "\n", Size, nSectors*SectorSize);
      uprintf("  StartSector: 0x%08" PRIx64 ", nSectors: 0x%" PRIx64 ", SectorSize: 0x%" PRIx64 "\n", StartSector, nSectors, SectorSize);
      return -1;
   }

   return (int64_t)Size;
}

/* Returns the number of bytes read or -1 on error */
int64_t read_sectors(HANDLE hDrive, uint64_t SectorSize,
                     uint64_t StartSector, uint64_t nSectors,
                     void *pBuf)
{
   LARGE_INTEGER ptr;
   DWORD Size;

   if((nSectors*SectorSize) > 0xFFFFFFFFUL)
   {
      uprintf("read_sectors: nSectors x SectorSize is too big\n");
      return -1;
   }
   Size = (DWORD)(nSectors*SectorSize);

   ptr.QuadPart = StartSector*SectorSize;
   if(!SetFilePointerEx(hDrive, ptr, NULL, FILE_BEGIN))
   {
      uprintf("read_sectors: Could not access sector 0x%08" PRIx64 " - %s\n", StartSector, WindowsErrorString());
      return -1;
   }

   if((!ReadFile(hDrive, pBuf, Size, &Size, NULL)) || (Size != nSectors*SectorSize))
   {
      uprintf("read_sectors: Read error %s\n", (GetLastError()!=ERROR_SUCCESS)?WindowsErrorString():"");
      uprintf("  Read: %d, Expected: %" PRIu64 "\n",  Size, nSectors*SectorSize);
      uprintf("  StartSector: 0x%08" PRIx64 ", nSectors: 0x%" PRIx64 ", SectorSize: 0x%" PRIx64 "\n", StartSector, nSectors, SectorSize);
   }

   return (int64_t)Size;
}



#endif
