/*
 * Rufus: The Reliable USB Formatting Utility
 * Standard Windows function calls
 * Copyright © 2013-2024 Pete Batard <pete@akeo.ie>
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
#include <sddl.h>
#include <gpedit.h>
#include <assert.h>
#include <accctrl.h>
#include <aclapi.h>

#include "re.h"
#include "rufus.h"
#include "missing.h"
#include "resource.h"
#include "msapi_utf8.h"
#include "localization.h"

#include "settings.h"

// MinGW doesn't yet know these (from wldp.h)
typedef enum WLDP_WINDOWS_LOCKDOWN_MODE
{
	WLDP_WINDOWS_LOCKDOWN_MODE_UNLOCKED = 0,
	WLDP_WINDOWS_LOCKDOWN_MODE_TRIAL,
	WLDP_WINDOWS_LOCKDOWN_MODE_LOCKED,
	WLDP_WINDOWS_LOCKDOWN_MODE_MAX,
} WLDP_WINDOWS_LOCKDOWN_MODE, * PWLDP_WINDOWS_LOCKDOWN_MODE;

windows_version_t WindowsVersion = { 0 };

/*
 * Hash table functions - modified From glibc 2.3.2:
 * [Aho,Sethi,Ullman] Compilers: Principles, Techniques and Tools, 1986
 * [Knuth]            The Art of Computer Programming, part 3 (6.4)
 */

/*
 * For the used double hash method the table size has to be a prime. To
 * correct the user given table size we need a prime test.  This trivial
 * algorithm is adequate because the code is called only during init and
 * the number is likely to be small
 */
static uint32_t isprime(uint32_t number)
{
	// no even number will be passed
	uint32_t divider = 3;

	while((divider * divider < number) && (number % divider != 0))
		divider += 2;

	return (number % divider != 0);
}

/*
 * Before using the hash table we must allocate memory for it.
 * We allocate one element more as the found prime number says.
 * This is done for more effective indexing as explained in the
 * comment for the hash function.
 */
BOOL htab_create(uint32_t nel, htab_table* htab)
{
	if (htab == NULL) {
		return FALSE;
	}
	if_not_assert(htab->table == NULL) {
		uprintf("Warning: htab_create() was called with a non empty table");
		return FALSE;
	}

	// Change nel to the first prime number not smaller as nel.
	nel |= 1;
	while(!isprime(nel))
		nel += 2;

	htab->size = nel;
	htab->filled = 0;

	// allocate memory and zero out.
	htab->table = (htab_entry*)calloc(htab->size + 1, sizeof(htab_entry));
	if (htab->table == NULL) {
		uprintf("Could not allocate space for hash table");
		return FALSE;
	}

	return TRUE;
}

/* After using the hash table it has to be destroyed.  */
void htab_destroy(htab_table* htab)
{
	size_t i;

	if ((htab == NULL) || (htab->table == NULL)) {
		return;
	}

	for (i=0; i<htab->size+1; i++) {
		if (htab->table[i].used) {
			safe_free(htab->table[i].str);
		}
	}
	htab->filled = 0; htab->size = 0;
	safe_free(htab->table);
	htab->table = NULL;
}

/*
 * This is the search function. It uses double hashing with open addressing.
 * We use a trick to speed up the lookup. The table is created with one
 * more element available. This enables us to use the index zero special.
 * This index will never be used because we store the first hash index in
 * the field used where zero means not used. Every other value means used.
 * The used field can be used as a first fast comparison for equality of
 * the stored and the parameter value. This helps to prevent unnecessary
 * expensive calls of strcmp.
 */
uint32_t htab_hash(char* str, htab_table* htab)
{
	uint32_t hval, hval2;
	uint32_t idx;
	uint32_t r = 0;
	int c;
	char* sz = str;

	if ((htab == NULL) || (htab->table == NULL) || (str == NULL)) {
		return 0;
	}

	// Compute main hash value using sdbm's algorithm (empirically
	// shown to produce half the collisions as djb2's).
	// See http://www.cse.yorku.ca/~oz/hash.html
	while ((c = *sz++) != 0)
		r = c + (r << 6) + (r << 16) - r;
	if (r == 0)
		++r;

	// compute table hash: simply take the modulus
	hval = r % htab->size;
	if (hval == 0)
		++hval;

	// Try the first index
	idx = hval;

	if (htab->table[idx].used) {
		if ( (htab->table[idx].used == hval)
		  && (safe_strcmp(str, htab->table[idx].str) == 0) ) {
			// existing hash
			return idx;
		}
		// uprintf("Hash collision ('%s' vs '%s')", str, htab->table[idx].str);

		// Second hash function, as suggested in [Knuth]
		hval2 = 1 + hval % (htab->size - 2);

		do {
			// Because size is prime this guarantees to step through all available indexes
			if (idx <= hval2) {
				idx = ((uint32_t)htab->size) + idx - hval2;
			} else {
				idx -= hval2;
			}

			// If we visited all entries leave the loop unsuccessfully
			if (idx == hval) {
				break;
			}

			// If entry is found use it.
			if ( (htab->table[idx].used == hval)
			  && (safe_strcmp(str, htab->table[idx].str) == 0) ) {
				return idx;
			}
		}
		while (htab->table[idx].used);
	}

	// Not found => New entry

	// If the table is full return an error
	if_not_assert(htab->filled < htab->size) {
		uprintf("Hash table is full (%d entries)", htab->size);
		return 0;
	}

	safe_free(htab->table[idx].str);
	htab->table[idx].used = hval;
	htab->table[idx].str = (char*) malloc(safe_strlen(str) + 1);
	if (htab->table[idx].str == NULL) {
		uprintf("Could not duplicate string for hash table");
		return 0;
	}
	memcpy(htab->table[idx].str, str, safe_strlen(str) + 1);
	++htab->filled;

	return idx;
}

static const char* GetEdition(DWORD ProductType)
{
	static char unknown_edition_str[64];

	// From: https://docs.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getproductinfo
	// These values can be found in the winnt.h header.
	switch (ProductType) {
	case 0x00000000: return "";	//  Undefined
	case 0x00000001: return "Ultimate";
	case 0x00000002: return "Home Basic";
	case 0x00000003: return "Home Premium";
	case 0x00000004: return "Enterprise";
	case 0x00000005: return "Home Basic N";
	case 0x00000006: return "Business";
	case 0x00000007: return "Server Standard";
	case 0x00000008: return "Server Datacenter";
	case 0x00000009: return "Smallbusiness Server";
	case 0x0000000A: return "Server Enterprise";
	case 0x0000000B: return "Starter";
	case 0x0000000C: return "Server Datacenter (Core)";
	case 0x0000000D: return "Server Standard (Core)";
	case 0x0000000E: return "Server Enterprise (Core)";
	case 0x00000010: return "Business N";
	case 0x00000011: return "Web Server";
	case 0x00000012: return "HPC Edition";
	case 0x00000013: return "Storage Server (Essentials)";
	case 0x0000001A: return "Home Premium N";
	case 0x0000001B: return "Enterprise N";
	case 0x0000001C: return "Ultimate N";
	case 0x00000022: return "Home Server";
	case 0x00000024: return "Server Standard without Hyper-V";
	case 0x00000025: return "Server Datacenter without Hyper-V";
	case 0x00000026: return "Server Enterprise without Hyper-V";
	case 0x00000027: return "Server Datacenter without Hyper-V (Core)";
	case 0x00000028: return "Server Standard without Hyper-V (Core)";
	case 0x00000029: return "Server Enterprise without Hyper-V (Core)";
	case 0x0000002A: return "Hyper-V Server";
	case 0x0000002F: return "Starter N";
	case 0x00000030: return "Pro";
	case 0x00000031: return "Pro N";
	case 0x00000034: return "Server Solutions Premium";
	case 0x00000035: return "Server Solutions Premium (Core)";
	case 0x00000040: return "Server Hyper Core V";
	case 0x00000042: return "Starter E";
	case 0x00000043: return "Home Basic E";
	case 0x00000044: return "Premium E";
	case 0x00000045: return "Pro E";
	case 0x00000046: return "Enterprise E";
	case 0x00000047: return "Ultimate E";
	case 0x00000048: return "Enterprise (Eval)";
	case 0x0000004F: return "Server Standard (Eval)";
	case 0x00000050: return "Server Datacenter (Eval)";
	case 0x00000054: return "Enterprise N (Eval)";
	case 0x00000057: return "Thin PC";
	case 0x00000058: case 0x00000059: case 0x0000005A: case 0x0000005B: case 0x0000005C: return "Embedded";
	case 0x00000062: return "Home N";
	case 0x00000063: return "Home China";
	case 0x00000064: return "Home Single Language";
	case 0x00000065: return "Home";
	case 0x00000067: return "Pro with Media Center";
	case 0x00000069: case 0x0000006A: case 0x0000006B: case 0x0000006C: return "Embedded";
	case 0x0000006F: return "Home Connected";
	case 0x00000070: return "Pro Student";
	case 0x00000071: return "Home Connected N";
	case 0x00000072: return "Pro Student N";
	case 0x00000073: return "Home Connected Single Language";
	case 0x00000074: return "Home Connected China";
	case 0x00000079: return "Education";
	case 0x0000007A: return "Education N";
	case 0x0000007D: return "Enterprise LTSB";
	case 0x0000007E: return "Enterprise LTSB N";
	case 0x0000007F: return "Pro S";
	case 0x00000080: return "Pro S N";
	case 0x00000081: return "Enterprise LTSB (Eval)";
	case 0x00000082: return "Enterprise LTSB N (Eval)";
	case 0x0000008A: return "Pro Single Language";
	case 0x0000008B: return "Pro China";
	case 0x0000008C: return "Enterprise Subscription";
	case 0x0000008D: return "Enterprise Subscription N";
	case 0x00000091: return "Server Datacenter SA (Core)";
	case 0x00000092: return "Server Standard SA (Core)";
	case 0x00000095: return "Utility VM";
	case 0x000000A1: return "Pro for Workstations";
	case 0x000000A2: return "Pro for Workstations N";
	case 0x000000A4: return "Pro for Education";
	case 0x000000A5: return "Pro for Education N";
	case 0x000000AB: return "Enterprise G";	// I swear Microsoft are just making up editions...
	case 0x000000AC: return "Enterprise G N";
	case 0x000000B2: return "Cloud";
	case 0x000000B3: return "Cloud N";
	case 0x000000B6: return "Home OS";
	case 0x000000B7: case 0x000000CB: return "Cloud E";
	case 0x000000B9: return "IoT OS";
	case 0x000000BA: case 0x000000CA: return "Cloud E N";
	case 0x000000BB: return "IoT Edge OS";
	case 0x000000BC: return "IoT Enterprise";
	case 0x000000BD: return "Lite";
	case 0x000000BF: return "IoT Enterprise S";
	case 0x000000C0: case 0x000000C2: case 0x000000C3: case 0x000000C4: case 0x000000C5: case 0x000000C6: return "XBox";
	case 0x000000C7: case 0x000000C8: case 0x00000196: case 0x00000197: case 0x00000198: return "Azure Server";
	case 0xABCDABCD: return "(Unlicensed)";
	default:
		static_sprintf(unknown_edition_str, "(Unknown Edition 0x%02X)", (uint32_t)ProductType);
		return unknown_edition_str;
	}
}

/*
 * Modified from smartmontools' os_win32.cpp
 */
void GetWindowsVersion(windows_version_t* windows_version)
{
	OSVERSIONINFOEXA vi, vi2;
	DWORD dwProductType = 0;
	const char* w = NULL;
	const char* arch_name;
	char *vptr;
	size_t vlen;
	DWORD major = 0, minor = 0;
	USHORT ProcessMachine = IMAGE_FILE_MACHINE_UNKNOWN, NativeMachine = IMAGE_FILE_MACHINE_UNKNOWN;
	ULONGLONG major_equal, minor_equal;
	BOOL ws, is_wow64 = FALSE;

	PF_TYPE_DECL(WINAPI, BOOL, IsWow64Process2, (HANDLE, USHORT*, USHORT*));
	PF_INIT(IsWow64Process2, Kernel32);

	memset(windows_version, 0, sizeof(windows_version_t));
	static_strcpy(windows_version->VersionStr, "Windows Undefined");

	memset(&vi, 0, sizeof(vi));
	vi.dwOSVersionInfoSize = sizeof(vi);
	if (!GetVersionExA((OSVERSIONINFOA *)&vi)) {
		memset(&vi, 0, sizeof(vi));
		vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);
		if (!GetVersionExA((OSVERSIONINFOA *)&vi))
			return;
	}

	if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT) {

		if (vi.dwMajorVersion > 6 || (vi.dwMajorVersion == 6 && vi.dwMinorVersion >= 2)) {
			// Starting with Windows 8.1 Preview, GetVersionEx() does no longer report the actual OS version
			// See: http://msdn.microsoft.com/en-us/library/windows/desktop/dn302074.aspx
			// And starting with Windows 10 Preview 2, Windows enforces the use of the application/supportedOS
			// manifest in order for VerSetConditionMask() to report the ACTUAL OS major and minor...

			major_equal = VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL);
			for (major = vi.dwMajorVersion; major <= 9; major++) {
				memset(&vi2, 0, sizeof(vi2));
				vi2.dwOSVersionInfoSize = sizeof(vi2); vi2.dwMajorVersion = major;
				if (!VerifyVersionInfoA(&vi2, VER_MAJORVERSION, major_equal))
					continue;
				if (vi.dwMajorVersion < major) {
					vi.dwMajorVersion = major; vi.dwMinorVersion = 0;
				}

				minor_equal = VerSetConditionMask(0, VER_MINORVERSION, VER_EQUAL);
				for (minor = vi.dwMinorVersion; minor <= 9; minor++) {
					memset(&vi2, 0, sizeof(vi2)); vi2.dwOSVersionInfoSize = sizeof(vi2);
					vi2.dwMinorVersion = minor;
					if (!VerifyVersionInfoA(&vi2, VER_MINORVERSION, minor_equal))
						continue;
					vi.dwMinorVersion = minor;
					break;
				}

				break;
			}
		}

		if (vi.dwMajorVersion <= 0xf && vi.dwMinorVersion <= 0xf) {
			ws = (vi.wProductType <= VER_NT_WORKSTATION);
			windows_version->Version = vi.dwMajorVersion << 4 | vi.dwMinorVersion;
			switch (windows_version->Version) {
			case WINDOWS_XP: w = "XP";
				break;
			case WINDOWS_2003: w = (ws ? "XP_64" : (!GetSystemMetrics(89) ? "Server 2003" : "Server 2003_R2"));
				break;
			case WINDOWS_VISTA: w = (ws ? "Vista" : "Server 2008");
				break;
			case WINDOWS_7: w = (ws ? "7" : "Server 2008_R2");
				break;
			case WINDOWS_8: w = (ws ? "8" : "Server 2012");
				break;
			case WINDOWS_8_1: w = (ws ? "8.1" : "Server 2012_R2");
				break;
			case WINDOWS_10_PREVIEW1: w = (ws ? "10 (Preview 1)" : "Server 10 (Preview 1)");
				break;
			// Starting with Windows 10 Preview 2, the major is the same as the public-facing version
			case WINDOWS_10:
				if (vi.dwBuildNumber < 20000) {
					w = (ws ? "10" : ((vi.dwBuildNumber < 17763) ? "Server 2016" : "Server 2019"));
					break;
				}
				windows_version->Version = WINDOWS_11;
				major = 11;
				// Fall through
			case WINDOWS_11: w = (ws ? "11" : "Server 2022");
				break;
			default:
				if (windows_version->Version < WINDOWS_XP)
					windows_version->Version = WINDOWS_UNDEFINED;
				else
					w = "12 or later";
				break;
			}
		}
	}
	windows_version->Major = major;
	windows_version->Minor = minor;

	if ((pfIsWow64Process2 != NULL) && pfIsWow64Process2(GetCurrentProcess(), &ProcessMachine, &NativeMachine)) {
		windows_version->Arch = NativeMachine;
	} else {
		// Assume same arch as the app
		windows_version->Arch = GetApplicationArch();
		// Fix the Arch if we have a 32-bit app running under WOW64
		if ((sizeof(uintptr_t) < 8) && IsWow64Process(GetCurrentProcess(), &is_wow64) && is_wow64) {
			if (windows_version->Arch == IMAGE_FILE_MACHINE_I386)
				windows_version->Arch = IMAGE_FILE_MACHINE_AMD64;
			else if (windows_version->Arch == IMAGE_FILE_MACHINE_ARM)
				windows_version->Arch = IMAGE_FILE_MACHINE_ARM64;
			else // I sure wanna be made aware of this scenario...
				assert(FALSE);
		}
		uprintf("Note: Underlying Windows architecture was guessed and may be incorrect...");
	}
	arch_name = GetArchName(windows_version->Arch);

	GetProductInfo(vi.dwMajorVersion, vi.dwMinorVersion, vi.wServicePackMajor, vi.wServicePackMinor, &dwProductType);
	vptr = &windows_version->VersionStr[sizeof("Windows ") - 1];
	vlen = sizeof(windows_version->VersionStr) - sizeof("Windows ") - 1;
	if (!w)
		safe_sprintf(vptr, vlen, "%s %u.%u %s", (vi.dwPlatformId == VER_PLATFORM_WIN32_NT ? "NT" : "??"),
			(unsigned)vi.dwMajorVersion, (unsigned)vi.dwMinorVersion, arch_name);
	else if (vi.wServicePackMinor)
		safe_sprintf(vptr, vlen, "%s SP%u.%u %s", w, vi.wServicePackMajor, vi.wServicePackMinor, arch_name);
	else if (vi.wServicePackMajor)
		safe_sprintf(vptr, vlen, "%s SP%u %s", w, vi.wServicePackMajor, arch_name);
	else
		safe_sprintf(vptr, vlen, "%s%s%s %s",
			w, (dwProductType != 0) ? " " : "", GetEdition(dwProductType), arch_name);

	windows_version->Edition = (int)dwProductType;

	// Add the build number (including UBR if available)
	windows_version->BuildNumber = vi.dwBuildNumber;
	windows_version->Ubr = ReadRegistryKey32(REGKEY_HKLM, "Software\\Microsoft\\Windows NT\\CurrentVersion\\UBR");
	vptr = &windows_version->VersionStr[safe_strlen(windows_version->VersionStr)];
	vlen = sizeof(windows_version->VersionStr) - safe_strlen(windows_version->VersionStr) - 1;
	if (windows_version->Ubr != 0)
		safe_sprintf(vptr, vlen, " (Build %lu.%lu)", windows_version->BuildNumber, windows_version->Ubr);
	else
		safe_sprintf(vptr, vlen, " (Build %lu)", windows_version->BuildNumber);
	vptr = &windows_version->VersionStr[safe_strlen(windows_version->VersionStr)];
	vlen = sizeof(windows_version->VersionStr) - safe_strlen(windows_version->VersionStr) - 1;
	if (isSMode())
		safe_sprintf(vptr, vlen, " in S Mode");
}


/*
 * String array manipulation
 */
void StrArrayCreate(StrArray* arr, uint32_t initial_size)
{
	if (arr == NULL) return;
	arr->Max = initial_size; arr->Index = 0;
	arr->String = (char**)calloc(arr->Max, sizeof(char*));
	if (arr->String == NULL)
		uprintf("Could not allocate string array");
}

int32_t StrArrayAdd(StrArray* arr, const char* str, BOOL duplicate)
{
	char** old_table;
	if ((arr == NULL) || (arr->String == NULL) || (str == NULL))
		return -1;
	if (arr->Index == arr->Max) {
		arr->Max *= 2;
		old_table = arr->String;
		arr->String = (char**)realloc(arr->String, arr->Max*sizeof(char*));
		if (arr->String == NULL) {
			free(old_table);
			uprintf("Could not reallocate string array");
			return -1;
		}
	}
	arr->String[arr->Index] = (duplicate)?safe_strdup(str):(char*)str;
	if (arr->String[arr->Index] == NULL) {
		uprintf("Could not store string in array");
		return -1;
	}
	return arr->Index++;
}

int32_t StrArrayFind(StrArray* arr, const char* str)
{
	uint32_t i;
	if ((str == NULL) || (arr == NULL) || (arr->String == NULL))
		return -1;
	for (i = 0; i<arr->Index; i++) {
		if (strcmp(arr->String[i], str) == 0)
			return (int32_t)i;
	}
	return -1;
}

void StrArrayClear(StrArray* arr)
{
	uint32_t i;
	if ((arr == NULL) || (arr->String == NULL))
		return;
	for (i = 0; i < arr->Index; i++) {
		safe_free(arr->String[i]);
	}
	arr->Index = 0;
}

void StrArrayDestroy(StrArray* arr)
{
	StrArrayClear(arr);
	if (arr != NULL)
		safe_free(arr->String);
}

// TODO: Some GUID linux compat
BOOL CompareGUID(const GUID *guid1, const GUID *guid2) {
	if ((guid1 != NULL) && (guid2 != NULL)) {
		return (memcmp(guid1, guid2, sizeof(GUID)) == 0);
	}
	return FALSE;
}

static BOOL CALLBACK EnumFontFamExProc(const LOGFONTA *lpelfe,
	const TEXTMETRICA *lpntme, DWORD FontType, LPARAM lParam)
{
	return TRUE;
}
