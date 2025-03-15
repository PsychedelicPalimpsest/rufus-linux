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
 

PF_TYPE_DECL(WINAPI, HRESULT, WldpQueryWindowsLockdownMode, (PWLDP_WINDOWS_LOCKDOWN_MODE));
BOOL isSMode(void)
{
	BOOL r = FALSE;
	WLDP_WINDOWS_LOCKDOWN_MODE mode;
	PF_INIT_OR_OUT(WldpQueryWindowsLockdownMode, Wldp);

	HRESULT hr = pfWldpQueryWindowsLockdownMode(&mode);
	if (hr != S_OK) {
		SetLastError((DWORD)hr);
		uprintf("Could not detect S Mode: %s", WindowsErrorString());
	} else {
		r = (mode != WLDP_WINDOWS_LOCKDOWN_MODE_UNLOCKED);
	}

out:
	return r;
}

/*
 * Why oh why does Microsoft make it so convoluted to retrieve a measly executable's version number ?
 */
 version_t* GetExecutableVersion(const char* path)
 {
     static version_t version, *r = NULL;
     uint8_t* buf = NULL;
     UINT uLen;
     DWORD dwSize, dwHandle;
     VS_FIXEDFILEINFO* version_info;
 
     memset(&version, 0, sizeof(version));
 
     dwSize = GetFileVersionInfoSizeU(path, &dwHandle);
     if (dwSize == 0)
         goto out;
 
     buf = malloc(dwSize);
     if (buf == NULL)
         goto out;;
     if (!GetFileVersionInfoU(path, dwHandle, dwSize, buf))
         goto out;
 
     if (!VerQueryValueA(buf, "\\", (LPVOID*)&version_info, &uLen) || uLen == 0)
         goto out;
 
     if (version_info->dwSignature != 0xfeef04bd)
         goto out;
 
     version.Major = (version_info->dwFileVersionMS >> 16) & 0xffff;
     version.Minor = (version_info->dwFileVersionMS >> 0) & 0xffff;
     version.Micro = (version_info->dwFileVersionLS >> 16) & 0xffff;
     version.Nano = (version_info->dwFileVersionLS >> 0) & 0xffff;
     r = &version;
 
 out:
     free(buf);
     return r;
 }
 

/*
 * Retrieve the SID of the current user. The returned PSID must be freed by the caller using LocalFree()
 */
 static PSID GetSID(void) {
	TOKEN_USER* tu = NULL;
	DWORD len;
	HANDLE token;
	PSID ret = NULL;
	char* psid_string = NULL;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
		uprintf("OpenProcessToken failed: %s", WindowsErrorString());
		return NULL;
	}

	if (!GetTokenInformation(token, TokenUser, tu, 0, &len)) {
		if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
			uprintf("GetTokenInformation (pre) failed: %s", WindowsErrorString());
			return NULL;
		}
		tu = (TOKEN_USER*)calloc(1, len);
	}
	if (tu == NULL) {
		return NULL;
	}

	if (GetTokenInformation(token, TokenUser, tu, len, &len)) {
		/*
		 * now of course, the interesting thing is that if you return tu->User.Sid
		 * but free tu, the PSID pointer becomes invalid after a while.
		 * The workaround? Convert to string then back to PSID
		 */
		if (!ConvertSidToStringSidA(tu->User.Sid, &psid_string)) {
			uprintf("Unable to convert SID to string: %s", WindowsErrorString());
			ret = NULL;
		} else {
			if (!ConvertStringSidToSidA(psid_string, &ret)) {
				uprintf("Unable to convert string back to SID: %s", WindowsErrorString());
				ret = NULL;
			}
			// MUST use LocalFree()
			LocalFree(psid_string);
		}
	} else {
		ret = NULL;
		uprintf("GetTokenInformation (real) failed: %s", WindowsErrorString());
	}
	free(tu);
	return ret;
}

BOOL FileIO(enum file_io_type io_type, char* path, char** buffer, DWORD* size)
{
	SECURITY_ATTRIBUTES s_attr, *sa = NULL;
	SECURITY_DESCRIPTOR s_desc;
	const LARGE_INTEGER liZero = { .QuadPart = 0ULL };
	PSID sid = NULL;
	HANDLE handle;
	DWORD dwDesiredAccess = 0, dwCreationDisposition = 0;
	BOOL r = FALSE;
	BOOL ret = FALSE;

	// Change the owner from admin to regular user
	sid = GetSID();
	if ( (sid != NULL)
	  && InitializeSecurityDescriptor(&s_desc, SECURITY_DESCRIPTOR_REVISION)
	  && SetSecurityDescriptorOwner(&s_desc, sid, FALSE) ) {
		s_attr.nLength = sizeof(SECURITY_ATTRIBUTES);
		s_attr.bInheritHandle = FALSE;
		s_attr.lpSecurityDescriptor = &s_desc;
		sa = &s_attr;
	} else {
		uprintf("Could not set security descriptor: %s", WindowsErrorString());
	}

	switch (io_type) {
	case FILE_IO_READ:
		*buffer = NULL;
		dwDesiredAccess = GENERIC_READ;
		dwCreationDisposition = OPEN_EXISTING;
		break;
	case FILE_IO_WRITE:
		dwDesiredAccess = GENERIC_WRITE;
		dwCreationDisposition = CREATE_ALWAYS;
		break;
	case FILE_IO_APPEND:
		dwDesiredAccess = FILE_APPEND_DATA;
		dwCreationDisposition = OPEN_ALWAYS;
		break;
	default:
		assert(FALSE);
		break;
	}
	handle = CreateFileU(path, dwDesiredAccess, FILE_SHARE_READ, sa,
		dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);

	if (handle == INVALID_HANDLE_VALUE) {
		uprintf("Could not open '%s': %s", path, WindowsErrorString());
		goto out;
	}

	switch (io_type) {
	case FILE_IO_READ:
		*size = GetFileSize(handle, NULL);
		*buffer = (char*)malloc(*size);
		if (*buffer == NULL) {
			uprintf("Could not allocate buffer for reading file");
			goto out;
		}
		r = ReadFile(handle, *buffer, *size, size, NULL);
		break;
	case FILE_IO_APPEND:
		SetFilePointerEx(handle, liZero, NULL, FILE_END);
		// Fall through
	case FILE_IO_WRITE:
		r = WriteFile(handle, *buffer, *size, size, NULL);
		break;
	}

	if (!r) {
		uprintf("I/O Error: %s", WindowsErrorString());
		goto out;
	}

	PrintInfoDebug(0, (io_type == FILE_IO_READ) ? MSG_215 : MSG_216, path);
	ret = TRUE;

out:
	CloseHandle(handle);
	if (!ret && (io_type == FILE_IO_READ)) {
		// Only leave the buffer allocated if we were able to read data
		safe_free(*buffer);
		*size = 0;
	}
	return ret;
}



/*
 * Get a resource from the RC. If needed that resource can be duplicated.
 * If duplicate is true and len is non-zero, the a zeroed buffer of 'len'
 * size is allocated for the resource. Else the buffer is allocated for
 * the resource size.
 */
 uint8_t* GetResource(HMODULE module, char* name, char* type, const char* desc, DWORD* len, BOOL duplicate)
 {
     HGLOBAL res_handle;
     HRSRC res;
     DWORD res_len;
     uint8_t* p = NULL;
 
     res = FindResourceA(module, name, type);
     if (res == NULL) {
         uprintf("Could not locate resource '%s': %s", desc, WindowsErrorString());
         goto out;
     }
     res_handle = LoadResource(module, res);
     if (res_handle == NULL) {
         uprintf("Could not load resource '%s': %s", desc, WindowsErrorString());
         goto out;
     }
     res_len = SizeofResource(module, res);
 
     if (duplicate) {
         if (*len == 0)
             *len = res_len;
         p = calloc(*len, 1);
         if (p == NULL) {
             uprintf("Could not allocate resource '%s'", desc);
             goto out;
         }
         memcpy(p, LockResource(res_handle), min(res_len, *len));
         if (res_len > *len)
             uprintf("WARNING: Resource '%s' was truncated by %d bytes!", desc, res_len - *len);
     } else {
         p = LockResource(res_handle);
     }
     *len = res_len;
 
 out:
     return p;
 }
 
 DWORD GetResourceSize(HMODULE module, char* name, char* type, const char* desc)
 {
     DWORD len = 0;
     return (GetResource(module, name, type, desc, &len, FALSE) == NULL)?0:len;
 }
 
 // Run a console command, with optional redirection of stdout and stderr to our log
 // as well as optional progress reporting if msg is not 0.
 DWORD RunCommandWithProgress(const char* cmd, const char* dir, BOOL log, int msg)
 {
     DWORD i, ret, dwRead, dwAvail, dwPipeSize = 4096;
     STARTUPINFOA si = { 0 };
     PROCESS_INFORMATION pi = { 0 };
     SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
     HANDLE hOutputRead = INVALID_HANDLE_VALUE, hOutputWrite = INVALID_HANDLE_VALUE;
     int match_length;
     static char* output;
     // For detecting typical dism.exe commandline progress report of type:
     // "\r[====                       8.0%                           ]\r\n"
     re_t pattern = re_compile("\\s*\\[[= ]+[\\d\\.]+%[= ]+\\]\\s*");
 
     si.cb = sizeof(si);
     if (log) {
         // NB: The size of a pipe is a suggestion, NOT an absolute guarantee
         // This means that you may get a pipe of 4K even if you requested 1K
         if (!CreatePipe(&hOutputRead, &hOutputWrite, &sa, dwPipeSize)) {
             ret = GetLastError();
             uprintf("Could not set commandline pipe: %s", WindowsErrorString());
             goto out;
         }
         si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES | STARTF_PREVENTPINNING | STARTF_TITLEISAPPID;
         si.wShowWindow = SW_HIDE;
         si.hStdOutput = hOutputWrite;
         si.hStdError = hOutputWrite;
     }
 
     if (!CreateProcessU(NULL, cmd, NULL, NULL, TRUE,
         NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, dir, &si, &pi)) {
         ret = GetLastError();
         uprintf("Unable to launch command '%s': %s", cmd, WindowsErrorString());
         goto out;
     }
 
     if (log || msg != 0) {
         if (msg != 0)
             UpdateProgressWithInfoInit(NULL, FALSE);
         while (1) {
             // Check for user cancel
             if (IS_ERROR(ErrorStatus) && (SCODE_CODE(ErrorStatus) == ERROR_CANCELLED)) {
                 if (!TerminateProcess(pi.hProcess, ERROR_CANCELLED)) {
                     uprintf("Could not terminate command: %s", WindowsErrorString());
                 } else switch (WaitForSingleObject(pi.hProcess, 5000)) {
                 case WAIT_TIMEOUT:
                     uprintf("Command did not terminate within timeout duration");
                     break;
                 case WAIT_OBJECT_0:
                     uprintf("Command was terminated by user");
                     break;
                 default:
                     uprintf("Error while waiting for command to be terminated: %s", WindowsErrorString());
                     break;
                 }
                 ret = ERROR_CANCELLED;
                 goto out;
             }
             // coverity[string_null]
             if (PeekNamedPipe(hOutputRead, NULL, dwPipeSize, NULL, &dwAvail, NULL)) {
                 if (dwAvail != 0) {
                     output = malloc(dwAvail + 1);
                     if ((output != NULL) && (ReadFile(hOutputRead, output, dwAvail, &dwRead, NULL)) && (dwRead != 0)) {
                         output[dwAvail] = 0;
                         // Process a commandline progress bar into a percentage
                         if ((msg != 0) && (re_matchp(pattern, output, &match_length) != -1)) {
                             float f = 0.0f;
                             i = 0;
 next_progress_line:
                             for (; (i < dwAvail) && (output[i] < '0' || output[i] > '9'); i++);
                             IGNORE_RETVAL(sscanf(&output[i], "%f*", &f));
                             UpdateProgressWithInfo(OP_FORMAT, msg, (uint64_t)(f * 100.0f), 100 * 100ULL);
                             // Go to next line
                             while ((++i < dwAvail) && (output[i] != '\n') && (output[i] != '\r'));
                             while ((++i < dwAvail) && ((output[i] == '\n') || (output[i] == '\r')));
                             // Print additional lines, if any
                             if (i < dwAvail) {
                                 // Might have two consecutive progress lines in our buffer
                                 if (re_matchp(pattern, &output[i], &match_length) != -1)
                                     goto next_progress_line;
                                 uprintf("%s", &output[i]);
                             }
                         } else if (log) {
                             // output may contain a '%' so don't feed it as a naked format string
                             uprintf("%s", output);
                         }
                     }
                     free(output);
                 }
             }
             if (WaitForSingleObject(pi.hProcess, 0) == WAIT_OBJECT_0)
                 break;
             Sleep(100);
         };
     } else {
         // TODO: Detect user cancellation here?
         WaitForSingleObject(pi.hProcess, INFINITE);
     }
 
     if (!GetExitCodeProcess(pi.hProcess, &ret))
         ret = GetLastError();
     CloseHandle(pi.hProcess);
     CloseHandle(pi.hThread);
 
 out:
     safe_closehandle(hOutputWrite);
     safe_closehandle(hOutputRead);
     return ret;
 }
 
 
BOOL IsFontAvailable(const char* font_name)
{
	BOOL r;
	LOGFONTA lf = { 0 };
	HDC hDC = GetDC(hMainDialog);

	if (font_name == NULL) {
		safe_release_dc(hMainDialog, hDC);
		return FALSE;
	}

	lf.lfCharSet = DEFAULT_CHARSET;
	safe_strcpy(lf.lfFaceName, LF_FACESIZE, font_name);

	r = EnumFontFamiliesExA(hDC, &lf, EnumFontFamExProc, 0, 0);
	safe_release_dc(hMainDialog, hDC);
	return r;
}

/*
 * Set or restore a Local Group Policy DWORD key indexed by szPath/SzPolicy
 */
// I've seen rare cases where pLGPO->lpVtbl->Save(...) gets stuck, which prevents the
// application from launching altogether. To alleviate this, use a thread that we can
// terminate if needed...
typedef struct {
	BOOL bRestore;
	BOOL* bExistingKey;
	const char* szPath;
	const char* szPolicy;
	DWORD dwValue;
} SetLGP_Params;

DWORD WINAPI SetLGPThread(LPVOID param)
{
	SetLGP_Params* p = (SetLGP_Params*)param;
	LONG r;
	DWORD disp, regtype, val=0, val_size=sizeof(DWORD);
	HRESULT hr;
	IGroupPolicyObject* pLGPO;
	// Along with global 'existing_key', this static value is used to restore initial state
	static DWORD original_val;
	HKEY path_key = NULL, policy_key = NULL;
	// MSVC is finicky about these ones even if you link against gpedit.lib => redefine them
	const IID my_IID_IGroupPolicyObject =
		{ 0xea502723L, 0xa23d, 0x11d1, { 0xa7, 0xd3, 0x0, 0x0, 0xf8, 0x75, 0x71, 0xe3 } };
	const IID my_CLSID_GroupPolicyObject =
		{ 0xea502722L, 0xa23d, 0x11d1, { 0xa7, 0xd3, 0x0, 0x0, 0xf8, 0x75, 0x71, 0xe3 } };
	GUID ext_guid = REGISTRY_EXTENSION_GUID;
	// Can be anything really
	GUID snap_guid = { 0x3D271CFCL, 0x2BC6, 0x4AC2, {0xB6, 0x33, 0x3B, 0xDF, 0xF5, 0xBD, 0xAB, 0x2A} };

	// Reinitialize COM since it's not shared between threads
	IGNORE_RETVAL(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));

	// We need an IGroupPolicyObject instance to set a Local Group Policy
	hr = CoCreateInstance(&my_CLSID_GroupPolicyObject, NULL, CLSCTX_INPROC_SERVER, &my_IID_IGroupPolicyObject, (LPVOID*)&pLGPO);
	if (FAILED(hr)) {
		ubprintf("SetLGP: CoCreateInstance failed; hr = %lx", hr);
		goto error;
	}

	hr = pLGPO->lpVtbl->OpenLocalMachineGPO(pLGPO, GPO_OPEN_LOAD_REGISTRY);
	if (FAILED(hr)) {
		ubprintf("SetLGP: OpenLocalMachineGPO failed - error %lx", hr);
		goto error;
	}

	hr = pLGPO->lpVtbl->GetRegistryKey(pLGPO, GPO_SECTION_MACHINE, &path_key);
	if (FAILED(hr)) {
		ubprintf("SetLGP: GetRegistryKey failed - error %lx", hr);
		goto error;
	}

	r = RegCreateKeyExA(path_key, p->szPath, 0, NULL, 0, KEY_SET_VALUE | KEY_QUERY_VALUE,
		NULL, &policy_key, &disp);
	if (r != ERROR_SUCCESS) {
		ubprintf("SetLGP: Failed to open LGPO path %s - error %lx", p->szPath, hr);
		policy_key = NULL;
		goto error;
	}

	if ((disp == REG_OPENED_EXISTING_KEY) && (!p->bRestore) && (!(*(p->bExistingKey)))) {
		// backup existing value for restore
		*(p->bExistingKey) = TRUE;
		regtype = REG_DWORD;
		r = RegQueryValueExA(policy_key, p->szPolicy, NULL, &regtype, (LPBYTE)&original_val, &val_size);
		if (r == ERROR_FILE_NOT_FOUND) {
			// The Key exists but not its value, which is OK
			*(p->bExistingKey) = FALSE;
		} else if (r != ERROR_SUCCESS) {
			ubprintf("SetLGP: Failed to read original %s policy value - error %lx", p->szPolicy, r);
		}
	}

	if ((!p->bRestore) || (*(p->bExistingKey))) {
		val = (p->bRestore)?original_val:p->dwValue;
		r = RegSetValueExA(policy_key, p->szPolicy, 0, REG_DWORD, (BYTE*)&val, sizeof(val));
	} else {
		r = RegDeleteValueA(policy_key, p->szPolicy);
	}
	if (r != ERROR_SUCCESS) {
		ubprintf("SetLGP: RegSetValueEx / RegDeleteValue failed - error %lx", r);
	}
	RegCloseKey(policy_key);
	policy_key = NULL;

	// Apply policy
	hr = pLGPO->lpVtbl->Save(pLGPO, TRUE, (p->bRestore)?FALSE:TRUE, &ext_guid, &snap_guid);
	if (hr != S_OK) {
		ubprintf("SetLGP: Unable to apply %s policy - error %lx", p->szPolicy, hr);
		goto error;
	} else {
		if ((!p->bRestore) || (*(p->bExistingKey))) {
			ubprintf("SetLGP: Successfully %s %s policy to 0x%08lX", (p->bRestore)?"restored":"set", p->szPolicy, val);
		} else {
			ubprintf("SetLGP: Successfully removed %s policy key", p->szPolicy);
		}
	}

	RegCloseKey(path_key);
	pLGPO->lpVtbl->Release(pLGPO);
	return TRUE;

error:
	if (path_key != NULL)
		RegCloseKey(path_key);
	if (pLGPO != NULL)
		pLGPO->lpVtbl->Release(pLGPO);
	CoUninitialize();
	return FALSE;
}

BOOL SetLGP(BOOL bRestore, BOOL* bExistingKey, const char* szPath, const char* szPolicy, DWORD dwValue)
{
	SetLGP_Params params = {bRestore, bExistingKey, szPath, szPolicy, dwValue};
	DWORD r = FALSE;
	HANDLE thread_id;

	if (ReadSettingBool(SETTING_DISABLE_LGP)) {
		ubprintf("LPG handling disabled, per settings");
		return FALSE;
	}

	thread_id = CreateThread(NULL, 0, SetLGPThread, (LPVOID)&params, 0, NULL);
	if (thread_id == NULL) {
		ubprintf("SetLGP: Unable to start thread");
		return FALSE;
	}
	if (WaitForSingleObject(thread_id, 5000) != WAIT_OBJECT_0) {
		ubprintf("SetLGP: Killing stuck thread!");
		TerminateThread(thread_id, 0);
		CloseHandle(thread_id);
		return FALSE;
	}
	if (!GetExitCodeThread(thread_id, &r))
		return FALSE;
	return (BOOL) r;
}

/*
 * This call tries to evenly balance the affinities for an array of
 * num_threads, according to the number of cores at our disposal...
 */
BOOL SetThreadAffinity(DWORD_PTR* thread_affinity, size_t num_threads)
{
	size_t i, j, pc;
	DWORD_PTR affinity, dummy;

	memset(thread_affinity, 0, num_threads * sizeof(DWORD_PTR));
	if (!GetProcessAffinityMask(GetCurrentProcess(), &affinity, &dummy))
		return FALSE;
	uuprintf("\r\nThread affinities:");
	uuprintf("  avail:\t%s", printbitslz(affinity));

	// If we don't have enough virtual cores to evenly spread our load forget it
	pc = popcnt64(affinity);
	if (pc < num_threads)
		return FALSE;

	// Spread the affinity as evenly as we can
	thread_affinity[num_threads - 1] = affinity;
	for (i = 0; i < num_threads - 1; i++) {
		for (j = 0; j < pc / num_threads; j++) {
			thread_affinity[i] |= affinity & (-1LL * affinity);
			affinity ^= affinity & (-1LL * affinity);
		}
		uuprintf("  thr_%d:\t%s", i, printbitslz(thread_affinity[i]));
		thread_affinity[num_threads - 1] ^= thread_affinity[i];
	}
	uuprintf("  thr_%d:\t%s", i, printbitslz(thread_affinity[i]));
	return TRUE;
}

/*
 * Returns true if:
 * 1. The OS supports UAC, UAC is on, and the current process runs elevated, or
 * 2. The OS doesn't support UAC or UAC is off, and the process is being run by a member of the admin group
 */
BOOL IsCurrentProcessElevated(void)
{
	BOOL r = FALSE;
	DWORD size;
	HANDLE token = INVALID_HANDLE_VALUE;
	TOKEN_ELEVATION te;
	SID_IDENTIFIER_AUTHORITY auth = { SECURITY_NT_AUTHORITY };
	PSID psid;

	if (ReadRegistryKey32(REGKEY_HKLM, "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System\\EnableLUA") == 1) {
		uprintf("Note: UAC is active");
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
			uprintf("Could not get current process token: %s", WindowsErrorString());
			goto out;
		}
		if (!GetTokenInformation(token, TokenElevation, &te, sizeof(te), &size)) {
			uprintf("Could not get token information: %s", WindowsErrorString());
			goto out;
		}
		r = (te.TokenIsElevated != 0);
	} else {
		uprintf("Note: UAC is either disabled or not available");
		if (!AllocateAndInitializeSid(&auth, 2, SECURITY_BUILTIN_DOMAIN_RID,
			DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &psid))
			goto out;
		if (!CheckTokenMembership(NULL, psid, &r))
			r = FALSE;
		FreeSid(psid);
	}

out:
	safe_closehandle(token);
	return r;
}

char* ToLocaleName(DWORD lang_id)
{
	static char mui_str[LOCALE_NAME_MAX_LENGTH];
	wchar_t wmui_str[LOCALE_NAME_MAX_LENGTH];

	if (LCIDToLocaleName(lang_id, wmui_str, LOCALE_NAME_MAX_LENGTH, 0) > 0) {
		wchar_to_utf8_no_alloc(wmui_str, mui_str, LOCALE_NAME_MAX_LENGTH);
	} else {
		static_strcpy(mui_str, "en-US");
	}
	return mui_str;
}

/*
 * From: https://stackoverflow.com/a/40390858/1069307
 */
BOOL SetPrivilege(HANDLE hToken, LPCWSTR pwzPrivilegeName, BOOL bEnable)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;

	if (!LookupPrivilegeValue(NULL, pwzPrivilegeName, &luid)) {
		uprintf("Could not lookup '%S' privilege: %s", pwzPrivilegeName, WindowsErrorString());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = bEnable ? SE_PRIVILEGE_ENABLED : 0;

	if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		uprintf("Could not %s '%S' privilege: %s",
			bEnable ? "enable" : "disable", pwzPrivilegeName, WindowsErrorString());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
		uprintf("Error assigning privileges: %s", WindowsErrorString());
		return FALSE;
	}

	return TRUE;
}

/*
 * Mount an offline registry hive located at <pszHivePath> into <key>\<pszHiveName>.
 * <key> should be HKEY_LOCAL_MACHINE or HKEY_USERS.
 */
BOOL MountRegistryHive(const HKEY key, const char* pszHiveName, const char* pszHivePath)
{
	LSTATUS status;
	HANDLE token = INVALID_HANDLE_VALUE;

	if_not_assert((key == HKEY_LOCAL_MACHINE) || (key == HKEY_USERS))
		return FALSE;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token)) {
		uprintf("Could not get current process token: %s", WindowsErrorString());
		return FALSE;
	}

	// Ignore errors on those in case we can proceed without...
	SetPrivilege(token, SE_RESTORE_NAME, TRUE);
	SetPrivilege(token, SE_BACKUP_NAME, TRUE);

	status = RegLoadKeyA(key, pszHiveName, pszHivePath);
	if (status != ERROR_SUCCESS) {
		SetLastError(status);
		uprintf("Could not mount offline registry hive '%s': %s", pszHivePath, WindowsErrorString());
	} else
		uprintf("Mounted offline registry hive '%s' to '%s\\%s'",
			pszHivePath, (key == HKEY_LOCAL_MACHINE) ? "HKLM" : "HKCU", pszHiveName);

	safe_closehandle(token);
	return (status == ERROR_SUCCESS);
}

/*
 * Unmount an offline registry hive.
 * <key> should be HKEY_LOCAL_MACHINE or HKEY_USERS.
 */
BOOL UnmountRegistryHive(const HKEY key, const char* pszHiveName)
{
	LSTATUS status;

	if_not_assert((key == HKEY_LOCAL_MACHINE) || (key == HKEY_USERS))
		return FALSE;

	status = RegUnLoadKeyA(key, pszHiveName);
	if (status != ERROR_SUCCESS) {
		SetLastError(status);
		uprintf("Could not unmount offline registry hive: %s", WindowsErrorString());
	} else
		uprintf("Unmounted offline registry hive '%s\\%s'",
			(key == HKEY_LOCAL_MACHINE) ? "HKLM" : "HKCU", pszHiveName);

	return (status == ERROR_SUCCESS);
}

/*
 * Take administrative ownership of a file or directory, and grant all access rights.
 */
BOOL TakeOwnership(LPCSTR lpszOwnFile)
{
	BOOL ret = FALSE;
	HANDLE hToken = NULL;
	PSID pSIDAdmin = NULL;
	PACL pOldDACL = NULL, pNewDACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	EXPLICIT_ACCESS ea = { 0 };

	if (lpszOwnFile == NULL)
		return FALSE;

	// Create a SID for the BUILTIN\Administrators group.
	if (!AllocateAndInitializeSid(&SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pSIDAdmin))
		goto out;

	// Open a handle to the access token for the calling process.
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
		goto out;

	// Enable the SE_TAKE_OWNERSHIP_NAME privilege.
	if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE))
		goto out;

	// Set the owner in the object's security descriptor.
	if (SetNamedSecurityInfoU(lpszOwnFile, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION,
		pSIDAdmin, NULL, NULL, NULL) != ERROR_SUCCESS)
		goto out;

	// Disable the SE_TAKE_OWNERSHIP_NAME privilege.
	if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, FALSE))
		goto out;

	// Get a pointer to the existing DACL.
	if (GetNamedSecurityInfoU(lpszOwnFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
		NULL, NULL, &pOldDACL, NULL, &pSD) != ERROR_SUCCESS)
		goto out;

	// Initialize an EXPLICIT_ACCESS structure for the new ACE
	// with full control for Administrators.
	ea.grfAccessPermissions = GENERIC_ALL;
	ea.grfAccessMode = GRANT_ACCESS;
	ea.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
	ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea.Trustee.ptstrName = (LPTSTR)pSIDAdmin;

	// Create a new ACL that merges the new ACE into the existing DACL.
	if (SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL) != ERROR_SUCCESS)
		goto out;

	// Try to modify the object's DACL.
	if (SetNamedSecurityInfoU(lpszOwnFile, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION,
		NULL, NULL, pNewDACL, NULL) != ERROR_SUCCESS)
		goto out;

	ret = TRUE;

out:
	FreeSid(pSIDAdmin);
	LocalFree(pNewDACL);
	safe_closehandle(hToken);
	return ret;
}
