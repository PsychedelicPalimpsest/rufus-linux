/*
 * Rufus: The Reliable USB Formatting Utility
 * Copyright (C) 2009  Henrik Carlqvist
 * Modified for Rufus/Windows (C) 2011-2019  Pete Batard
 * Modified for Rufus/linux (C) 2025 PsychedelicPalimpsest
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

/** 
 * Alright, some might call me a heretic for the way I am handling linux
 * devices, and those people are correct. Evil things are happening with
 * HANDLE.
 */



#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../rufus.h"
#include "file.h"


#ifdef _WIN32
#define _a_malloc(SIZE, ALLIGN) _mm_malloc((SIZE), (ALLIGN))
#define _a_free(PTR) _mm_free((PTR))

#else

#define _a_malloc(SIZE, ALLIGN) aligned_alloc((ALLIGN), (SIZE))
#define _a_free(PTR) free((PTR))
#endif


extern unsigned long ulBytesPerSector;

/*
* The following calls use a hijacked fp on Windows that contains:
* fp->_handle: a Windows handle
* fp->_offset: a file offset
*/

int contains_data(FILE *fp, uint64_t Position,
	const void *pData, uint64_t Len)
{
   int r = 0;
   unsigned char *aucBuf = _a_malloc(MAX_DATA_LEN, 16);

   if(aucBuf == NULL)
      return 0;

   if(!read_data(fp, Position, aucBuf, Len))
      goto out;

   if(memcmp(pData, aucBuf, (size_t)Len))
      goto out;

   r = 1;

out:
   _a_free(aucBuf);
   return r;
} /* contains_data */

int read_data(FILE *fp, uint64_t Position,
              void *pData, uint64_t Len)
{
   int r = 0;
   unsigned char *aucBuf = _a_malloc(MAX_DATA_LEN, 16);

   #ifdef _WIN32
   FAKE_FD* fd = (FAKE_FD*)fp;
   HANDLE hDrive = (HANDLE)fd->_handle;
   #else
   HANDLE hDrive = fileno(fp);
   #endif


   uint64_t StartSector, EndSector, NumSectors;

   if (aucBuf == NULL)
      return 0;

   #ifdef _WIN32
   Position += fd->_offset;
   #else
   Position += ftell(fp);
   #endif

   StartSector = Position/ulBytesPerSector;
   EndSector   = (Position+Len+ulBytesPerSector -1)/ulBytesPerSector;
   NumSectors  = (size_t)(EndSector - StartSector);

   if((NumSectors*ulBytesPerSector) > MAX_DATA_LEN)
   {
      uprintf("read_data: Please increase MAX_DATA_LEN in file.h\n");
      goto out;
   }

   if(Len > 0xFFFFFFFFUL)
   {
      uprintf("read_data: Len is too big\n");
      goto out;
   }

   if(read_sectors(hDrive, ulBytesPerSector, StartSector,
                     NumSectors, aucBuf) <= 0)
   goto out;

   memcpy(pData, &aucBuf[Position - StartSector*ulBytesPerSector], (size_t)Len);

   r = 1;

out:
   _a_free(aucBuf);
   return r;
}  /* read_data */

/* May read/write the same sector many times, but compatible with existing ms-sys */
int write_data(FILE *fp, uint64_t Position,
               const void *pData, uint64_t Len)
{
   int r = 0;
   /* Windows' WriteFile() may require a buffer that is aligned to the sector size */
   unsigned char *aucBuf = _a_malloc(MAX_DATA_LEN, 4096);
   
   #ifdef _WIN32
   FAKE_FD* fd = (FAKE_FD*)fp;
   HANDLE hDrive = (HANDLE)fd->_handle;
   #else
   HANDLE hDrive = fileno(fp);
   #endif

   uint64_t StartSector, EndSector, NumSectors;

   if (aucBuf == NULL)
      return 0;

   #ifdef _WIN32
   Position += fd->_offset;
   #else
   Position += ftell(fp);
   #endif
   StartSector = Position/ulBytesPerSector;
   EndSector   = (Position+Len+ulBytesPerSector-1)/ulBytesPerSector;
   NumSectors  = EndSector - StartSector;

   if((NumSectors*ulBytesPerSector) > MAX_DATA_LEN)
   {
      uprintf("write_data: Please increase MAX_DATA_LEN in file.h\n");
      goto out;
   }

   if(Len > 0xFFFFFFFFUL)
   {
      uprintf("write_data: Len is too big\n");
      goto out;
   }

   /* Data to write may not be aligned on a sector boundary => read into a sector buffer first */
   if(read_sectors(hDrive, ulBytesPerSector, StartSector,
                     NumSectors, aucBuf) <= 0)
      goto out;

   if(!memcpy(&aucBuf[Position - StartSector*ulBytesPerSector], pData, (size_t)Len))
      goto out;

   if(write_sectors(hDrive, ulBytesPerSector, StartSector,
                     NumSectors, aucBuf) <= 0)
      goto out;

   r = 1;

out:
   _a_free(aucBuf);
   return r;
} /* write_data */


