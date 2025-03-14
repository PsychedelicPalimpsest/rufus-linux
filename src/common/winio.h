/*
* Rufus: The Reliable USB Formatting Utility
* Windows I/O redefinitions, that would be totally unnecessary had
* Microsoft done a proper job with their asynchronous APIs. While
* also allowing these functions to be easily ported to Linux.
* This .h file has it's functions implmented in TWO .c files.
*	See: windows_specific/winio.c and linux_specific/winio.c
* 
* Copyright © 2021-2024 Pete Batard <pete@akeo.ie>
* Copyright © 2025 PsychedelicPalimpsest
* 
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
#include "msapi_utf8.h"

#pragma once





/// <summary>
/// Open a file for asynchronous access. The values for the flags are the same as the ones
/// for the native CreateFile() call. Note that FILE_FLAG_OVERLAPPED will always be added
/// to dwFlagsAndAttributes before the file is instantiated, and that an internal
/// OVERLAPPED structure with its associated wait event is also created.
/// </summary>
/// <param name="lpFileName">The name of the file or device to be created or opened</param>
/// <param name="dwDesiredAccess">The requested access to the file or device</param>
/// <param name="dwShareMode">The requested sharing mode of the file or device</param>
/// <param name="dwCreationDisposition">Action to take on a file or device that exists or does not exist</param>
/// <param name="dwFlagsAndAttributes">The file or device attributes and flags</param>
/// <returns>Non NULL on success</returns>
void* CreateFileAsync(LPCSTR lpFileName, DWORD dwDesiredAccess,
	DWORD dwShareMode, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes);

/// <summary>
/// Close a previously opened asynchronous file
/// </summary>
/// <param name="h">An async handle, created by a call to CreateFileAsync()</param>
VOID CloseFileAsync(void* h);

/// <summary>
/// Initiate a read operation for asynchronous I/O.
/// </summary>
/// <param name="h">An async handle, created by a call to CreateFileAsync()</param>
/// <param name="lpBuffer">The buffer that receives the data</param>
/// <param name="nNumberOfBytesToRead">Number of bytes requested</param>
/// <returns>TRUE on success, FALSE on error</returns>
BOOL ReadFileAsync(void* h, LPVOID lpBuffer, DWORD nNumberOfBytesToRead);

/// <summary>
/// Initiate a write operation for asynchronous I/O.
/// </summary>
/// <param name="h">An async handle, created by a call to CreateFileAsync()</param>
/// <param name="lpBuffer">The buffer that contains the data</param>
/// <param name="nNumberOfBytesToWrite">Number of bytes to write</param>
/// <returns>TRUE on success, FALSE on error</returns>
BOOL WriteFileAsync(void* h, LPVOID lpBuffer, DWORD nNumberOfBytesToWrite);
/// <summary>
/// Wait for an asynchronous operation to complete, with timeout.
/// This function also succeeds if the I/O already completed synchronously.
/// </summary>
/// <param name="h">An async handle, created by a call to CreateFileAsync()</param>
/// <param name="dwTimeout">A timeout value, in ms</param>
/// <returns>TRUE on success, FALSE on error</returns>
BOOL WaitFileAsync(void* h, DWORD dwTimeout);

/// <summary>
/// Return the number of bytes read or written and keep track/update the
/// current offset for an asynchronous read operation.
/// </summary>
/// <param name="h">An async handle, created by a call to CreateFileAsync()</param>
/// <param name="lpNumberOfBytes">A pointer that receives the number of bytes transferred.</param>
/// <returns>TRUE on success, FALSE on error</returns>
BOOL GetSizeAsync(void* h, LPDWORD lpNumberOfBytes);