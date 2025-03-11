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