/*
 * Copyright © 2019 Nikolay Sivov for CodeWeavers
 * Copyright © 2025 PsychedelicPalimpsest
 *
 * A modified version of the wine windows strings file.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#pragma once

#include <stdarg.h>
#include "pseudo_windows.h"

#define WINAPI


/* StrToIntEx flags */
#define STIF_DEFAULT     0x0
#define STIF_SUPPORT_HEX 0x1

#define StrCpyNA lstrcpynA

#include <wchar.h>
#include <ctype.h>
#include <wctype.h>

// Define a flag for case-insensitive comparison
#define NORM_IGNORECASE 0x00000001
#define LOCALE_USE_CP_ACP 0


int CompareStringW(
    const void* Locale,                // Ignored
    DWORD dwCmpFlags,           // Flags like NORM_IGNORECASE
    const wchar_t* lpString1,          // First string
    int cchCount1,              // Length of first string
    const wchar_t* lpString2,          // Second string
    int cchCount2               // Length of second string
) ;

int CompareStringA(
    const void* Locale,                // Ignored
    DWORD dwCmpFlags,           // Flags like NORM_IGNORECASE
    const char* lpString1,             // First string
    int cchCount1,              // Length of first string
    const char* lpString2,             // Second string
    int cchCount2               // Length of second string
) ;


BOOL IsDBCSLeadByte(unsigned char c) ;
BOOL IsDBCSLeadByteEx(WORD ignored_codepage, wchar_t wc) ;


// static BOOL char_compare(WORD ch1, WORD ch2, DWORD flags);

int WINAPI lstrcmpA( LPCSTR str1, LPCSTR str2 );

int WINAPI lstrcmpW(LPCWSTR str1, LPCWSTR str2);

int WINAPI lstrcmpiA(LPCSTR str1, LPCSTR str2);

int WINAPI lstrcmpiW(LPCWSTR str1, LPCWSTR str2);


LPSTR WINAPI lstrcpynA( LPSTR dst, LPCSTR src, INT n );

LPWSTR WINAPI lstrcpynW( LPWSTR dst, LPCWSTR src, INT n );

INT WINAPI lstrlenA( LPCSTR str );

INT WINAPI lstrlenW( LPCWSTR str );


DWORD WINAPI StrCmpCA(const char *str, const char *cmp);

DWORD WINAPI StrCmpCW(const WCHAR *str, const WCHAR *cmp);

DWORD WINAPI StrCmpICA(const char *str, const char *cmp);

DWORD WINAPI StrCmpICW(const WCHAR *str, const WCHAR *cmp);

DWORD WINAPI StrCmpNICA(const char *str, const char *cmp, DWORD len);

DWORD WINAPI StrCmpNICW(const WCHAR *str, const WCHAR *cmp, DWORD len);

char * WINAPI StrChrA(const char *str, WORD ch);

WCHAR * WINAPI StrChrW(const WCHAR *str, WCHAR ch);

char * WINAPI StrChrIA(const char *str, WORD ch);

WCHAR * WINAPI StrChrIW(const WCHAR *str, WCHAR ch);

WCHAR * WINAPI StrChrNW(const WCHAR *str, WCHAR ch, UINT max_len);

char * WINAPI StrDupA(const char *str);

WCHAR * WINAPI StrDupW(const WCHAR *str);

BOOL WINAPI ChrCmpIA(WORD ch1, WORD ch2);

BOOL WINAPI ChrCmpIW(WCHAR ch1, WCHAR ch2);

char * WINAPI StrStrA(const char *str, const char *search);

WCHAR * WINAPI StrStrW(const WCHAR *str, const WCHAR *search);

WCHAR * WINAPI StrStrNW(const WCHAR *str, const WCHAR *search, UINT max_len);

int WINAPI StrCmpNIA(const char *str, const char *cmp, int len);

WCHAR * WINAPI StrStrNIW(const WCHAR *str, const WCHAR *search, UINT max_len);

int WINAPI StrCmpNA(const char *str, const char *comp, int len);

int WINAPI StrCmpNW(const WCHAR *str, const WCHAR *comp, int len);

DWORD WINAPI StrCmpNCA(const char *str, const char *comp, int len);

DWORD WINAPI StrCmpNCW(const WCHAR *str, const WCHAR *comp, int len);

int WINAPI StrCmpNIW(const WCHAR *str, const WCHAR *comp, int len);

int WINAPI StrCmpW(const WCHAR *str, const WCHAR *comp);

int WINAPI StrCmpIW(const WCHAR *str, const WCHAR *comp);

WCHAR * WINAPI StrCpyNW(WCHAR *dst, const WCHAR *src, int count);

char * WINAPI StrStrIA(const char *str, const char *search);

WCHAR * WINAPI StrStrIW(const WCHAR *str, const WCHAR *search);

int WINAPI StrSpnA(const char *str, const char *match);

int WINAPI StrSpnW(const WCHAR *str, const WCHAR *match);

int WINAPI StrCSpnA(const char *str, const char *match);

int WINAPI StrCSpnW(const WCHAR *str, const WCHAR *match);

int WINAPI StrCSpnIA(const char *str, const char *match);

int WINAPI StrCSpnIW(const WCHAR *str, const WCHAR *match);

char * WINAPI StrRChrA(const char *str, const char *end, WORD ch);

WCHAR * WINAPI StrRChrW(const WCHAR *str, const WCHAR *end, WORD ch);

char * WINAPI StrRChrIA(const char *str, const char *end, WORD ch);

WCHAR * WINAPI StrRChrIW(const WCHAR *str, const WCHAR *end, WORD ch);

char * WINAPI StrRStrIA(const char *str, const char *end, const char *search);

WCHAR * WINAPI StrRStrIW(const WCHAR *str, const WCHAR *end, const WCHAR *search);

char * WINAPI StrPBrkA(const char *str, const char *match);

WCHAR * WINAPI StrPBrkW(const WCHAR *str, const WCHAR *match);

BOOL WINAPI StrTrimA(char *str, const char *trim);

BOOL WINAPI StrTrimW(WCHAR *str, const WCHAR *trim);

BOOL WINAPI StrToInt64ExA(const char *str, DWORD flags, LONGLONG *ret);

BOOL WINAPI StrToInt64ExW(const WCHAR *str, DWORD flags, LONGLONG *ret);

BOOL WINAPI StrToIntExA(const char *str, DWORD flags, INT *ret);

BOOL WINAPI StrToIntExW(const WCHAR *str, DWORD flags, INT *ret);

int WINAPI StrToIntA(const char *str);

int WINAPI StrToIntW(const WCHAR *str);

char * WINAPI StrCpyNXA(char *dst, const char *src, int len);

WCHAR * WINAPI StrCpyNXW(WCHAR *dst, const WCHAR *src, int len);


LPSTR WINAPI CharLowerA(char *str);


DWORD WINAPI CharLowerBuffA(char *str, DWORD len);

// This can and should be done better,
// but will work in this case!
DWORD CharLowerBuffW(WCHAR *str, DWORD len);

LPWSTR WINAPI CharLowerW(WCHAR *str);

LPSTR WINAPI CharNextA(const char *ptr);

LPSTR WINAPI CharNextExA(WORD codepage, const char *ptr, DWORD flags);

LPWSTR WINAPI CharNextW(const WCHAR *x);

LPSTR WINAPI CharPrevA(const char *start, const char *ptr);

LPSTR WINAPI CharPrevExA(WORD codepage, const char *start, const char *ptr, DWORD flags);

LPWSTR WINAPI CharPrevW(const WCHAR *start, const WCHAR *x);

// #if 0
LPSTR WINAPI CharUpperA(LPSTR str);
// #endif

DWORD WINAPI CharUpperBuffA(LPSTR str, DWORD len);

// This can and should be done better,
// but will work in this case!
DWORD CharUpperBuffW(WCHAR *str, DWORD len);

LPWSTR WINAPI CharUpperW(WCHAR *str);

#if 0
INT WINAPI DECLSPEC_HOTPATCH LoadStringW(HINSTANCE instance, UINT resource_id, LPWSTR buffer, INT buflen);

INT WINAPI DECLSPEC_HOTPATCH LoadStringA(HINSTANCE instance, UINT resource_id, LPSTR buffer, INT buflen);
#endif

int WINAPI StrCmpLogicalW(const WCHAR *str, const WCHAR *comp);

BOOL WINAPI StrIsIntlEqualA(BOOL case_sensitive, const char *str, const char *cmp, int len);

BOOL WINAPI StrIsIntlEqualW(BOOL case_sensitive, const WCHAR *str, const WCHAR *cmp, int len);

char * WINAPI StrCatBuffA(char *str, const char *cat, INT max_len);

WCHAR * WINAPI StrCatBuffW(WCHAR *str, const WCHAR *cat, INT max_len);

DWORD WINAPI StrCatChainW(WCHAR *str, DWORD max_len, DWORD at, const WCHAR *cat);

DWORD WINAPI SHTruncateString(char *str, DWORD size);

#if 0
HRESULT WINAPI SHLoadIndirectString(const WCHAR *src, WCHAR *dst, UINT dst_len, void **reserved);
#endif