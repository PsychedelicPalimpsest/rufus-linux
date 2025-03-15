/*
 * Rufus: The Reliable USB Formatting Utility
 * Copyright © 2011-2025 Pete Batard <pete@akeo.ie>
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
#include <assert.h>
#include <pseudo_windows.h>
#include <malloc.h>
#include <inttypes.h>

#include "rufus.h"

#pragma once



#define ComboBox_GetCurItemData(hCtrl) ComboBox_GetItemData(hCtrl, ComboBox_GetCurSel(hCtrl))


/* Custom Windows messages */
enum user_message_type {
	UM_FORMAT_COMPLETED = WM_APP,
	UM_MEDIA_CHANGE,
	UM_PROGRESS_INIT,
	UM_PROGRESS_EXIT,
	UM_NO_UPDATE,
	UM_UPDATE_CSM_TOOLTIP,
	UM_RESIZE_BUTTONS,
	UM_ENABLE_CONTROLS,
	UM_SELECT_ISO,
	UM_TIMER_START,
	UM_FORMAT_START,
	// Start of the WM IDs for the language menu items
	UM_LANGUAGE_MENU = WM_APP + 0x100
};

/* Custom notifications */
enum notification_type {
	MSG_INFO,
	MSG_WARNING,
	MSG_ERROR,
	MSG_QUESTION,
	MSG_WARNING_QUESTION
};
typedef INT_PTR (CALLBACK *Callback_t)(HWND, UINT, WPARAM, LPARAM);
typedef struct {
	WORD id;
	union {
		Callback_t callback;
		char* url;
	};
} notification_info;	// To provide a "More info..." on notifications

/* Status Bar sections */
#define SB_SECTION_LEFT         0
#define SB_SECTION_RIGHT        1
#define SB_TIMER_SECTION_SIZE   58.0f

/* Timers used throughout the program */
enum timer_type {
	TID_MESSAGE_INFO = 0x1000,
	TID_MESSAGE_STATUS,
	TID_OUTPUT_INFO,
	TID_OUTPUT_STATUS,
	TID_BADBLOCKS_UPDATE,
	TID_APP_TIMER,
	TID_BLOCKING_TIMER,
	TID_REFRESH_TIMER,
	TID_MARQUEE_TIMER
};

/* Action type, for progress bar breakdown */
enum action_type {
	OP_NOOP_WITH_TASKBAR = -3,
	OP_NOOP = -2,
	OP_INIT = -1,
	OP_ANALYZE_MBR = 0,
	OP_BADBLOCKS,
	OP_ZERO_MBR,
	OP_PARTITION,
	OP_FORMAT,
	OP_CREATE_FS,
	OP_FIX_MBR,
	OP_FILE_COPY,
	OP_PATCH,
	OP_FINALIZE,
	OP_EXTRACT_ZIP,
	OP_MAX
};

/* File system indexes in our FS combobox */
enum fs_type {
	FS_UNKNOWN = -1,
	FS_FAT16 = 0,
	FS_FAT32,
	FS_NTFS,
	FS_UDF,
	FS_EXFAT,
	FS_REFS,
	FS_EXT2,
	FS_EXT3,
	FS_EXT4,
	FS_MAX
};

e

// Windows User Experience (unattend.xml) flags and masks
#define UNATTEND_SECUREBOOT_TPM_MINRAM      0x00001
#define UNATTEND_NO_ONLINE_ACCOUNT          0x00004
#define UNATTEND_NO_DATA_COLLECTION         0x00008
#define UNATTEND_OFFLINE_INTERNAL_DRIVES    0x00010
#define UNATTEND_DUPLICATE_LOCALE           0x00020
#define UNATTEND_SET_USER                   0x00040
#define UNATTEND_DISABLE_BITLOCKER          0x00080
#define UNATTEND_FORCE_S_MODE               0x00100
#define UNATTEND_USE_MS2023_BOOTLOADERS     0x00200
#define UNATTEND_FULL_MASK                  0x003FF
#define UNATTEND_DEFAULT_MASK               0x000FF
#define UNATTEND_WINDOWS_TO_GO              0x10000		// Special flag for Windows To Go

#define UNATTEND_WINPE_SETUP_MASK           (UNATTEND_SECUREBOOT_TPM_MINRAM)
#define UNATTEND_SPECIALIZE_DEPLOYMENT_MASK (UNATTEND_NO_ONLINE_ACCOUNT)
#define UNATTEND_OOBE_SHELL_SETUP_MASK      (UNATTEND_NO_DATA_COLLECTION | UNATTEND_SET_USER | UNATTEND_DUPLICATE_LOCALE)
#define UNATTEND_OOBE_INTERNATIONAL_MASK    (UNATTEND_DUPLICATE_LOCALE)
#define UNATTEND_OOBE_MASK                  (UNATTEND_OOBE_SHELL_SETUP_MASK | UNATTEND_OOBE_INTERNATIONAL_MASK | UNATTEND_DISABLE_BITLOCKER | UNATTEND_USE_MS2023_BOOTLOADERS)
#define UNATTEND_OFFLINE_SERVICING_MASK     (UNATTEND_OFFLINE_INTERNAL_DRIVES | UNATTEND_FORCE_S_MODE)
#define UNATTEND_DEFAULT_SELECTION_MASK     (UNATTEND_SECUREBOOT_TPM_MINRAM | UNATTEND_NO_ONLINE_ACCOUNT | UNATTEND_OFFLINE_INTERNAL_DRIVES)

/* Used with ListDirectoryContent */
#define LIST_DIR_TYPE_FILE			0x01
#define LIST_DIR_TYPE_DIRECTORY		0x02
#define LIST_DIR_TYPE_RECURSIVE		0x80

/* Hash tables */
typedef struct htab_entry {
	uint32_t used;
	char* str;
	void* data;
} htab_entry;
typedef struct htab_table {
	htab_entry* table;
	uint32_t size;
	uint32_t filled;
} htab_table;
#define HTAB_EMPTY { NULL, 0, 0 }
extern BOOL htab_create(uint32_t nel, htab_table* htab);
extern void htab_destroy(htab_table* htab);
extern uint32_t htab_hash(char* str, htab_table* htab);


/*
 * Globals
 */
extern HINSTANCE hMainInstance;
extern HWND hMainDialog, hLogDialog, hStatus, hDeviceList, hCapacity, hImageOption;
extern HWND hPartitionScheme, hTargetSystem, hFileSystem, hClusterSize, hLabel, hBootType;
extern HWND hNBPasses, hLog, hInfo, hProgress;
extern const int nb_steps[FS_MAX];
extern float fScale;
extern windows_version_t WindowsVersion;

/*
 * Shared prototypes
 */
extern void GetWindowsVersion(windows_version_t* WindowsVersion);
extern version_t* GetExecutableVersion(const char* path);
extern const char* WindowsErrorString(void);
extern void DumpBufferHex(void *buf, size_t size);
extern void PrintStatusInfo(BOOL info, BOOL debug, unsigned int duration, int msg_id, ...);
#define PrintStatus(...) PrintStatusInfo(FALSE, FALSE, __VA_ARGS__)
#define PrintStatusDebug(...) PrintStatusInfo(FALSE, TRUE, __VA_ARGS__)
#define PrintInfo(...) PrintStatusInfo(TRUE, FALSE, __VA_ARGS__)
#define PrintInfoDebug(...) PrintStatusInfo(TRUE, TRUE, __VA_ARGS__)
extern void UpdateProgress(int op, float percent);
extern void _UpdateProgressWithInfo(int op, int msg, uint64_t processed, uint64_t total, BOOL force);
#define UpdateProgressWithInfo(op, msg, processed, total) _UpdateProgressWithInfo(op, msg, processed, total, FALSE)
#define UpdateProgressWithInfoForce(op, msg, processed, total) _UpdateProgressWithInfo(op, msg, processed, total, TRUE)
#define UpdateProgressWithInfoInit(hProgressDialog, bNoAltMode) UpdateProgressWithInfo(OP_INIT, (int)bNoAltMode, (uint64_t)(uintptr_t)hProgressDialog, 0);
extern const char* StrError(DWORD error_code, BOOL use_default_locale);
extern char* GuidToString(const GUID* guid, BOOL bDecorated);
extern GUID* StringToGuid(const char* str);
extern HWND MyCreateDialog(HINSTANCE hInstance, int Dialog_ID, HWND hWndParent, DLGPROC lpDialogFunc);
extern INT_PTR MyDialogBox(HINSTANCE hInstance, int Dialog_ID, HWND hWndParent, DLGPROC lpDialogFunc);
extern void CenterDialog(HWND hDlg, HWND hParent);
extern void ResizeMoveCtrl(HWND hDlg, HWND hCtrl, int dx, int dy, int dw, int dh, float scale);
extern void ResizeButtonHeight(HWND hDlg, int id);
extern void CreateStatusBar(void);
extern void CreateStaticFont(HDC hDC, HFONT* hFont, BOOL underlined);
extern void SetTitleBarIcon(HWND hDlg);
extern BOOL CreateTaskbarList(void);
extern BOOL SetTaskbarProgressState(TASKBAR_PROGRESS_FLAGS tbpFlags);
extern BOOL SetTaskbarProgressValue(ULONGLONG ullCompleted, ULONGLONG ullTotal);
extern INT_PTR CreateAboutBox(void);
extern BOOL CreateTooltip(HWND hControl, const char* message, int duration);
extern void DestroyTooltip(HWND hWnd);
extern void DestroyAllTooltips(void);
extern BOOL Notification(int type, const char* dont_display_setting, const notification_info* more_info, char* title, char* format, ...);
extern int CustomSelectionDialog(int style, char* title, char* message, char** choices, int size, int mask, int username_index);
#define SelectionDialog(title, message, choices, size) CustomSelectionDialog(BS_AUTORADIOBUTTON, title, message, choices, size, 1, -1)
extern void ListDialog(char* title, char* message, char** items, int size);
extern SIZE GetTextSize(HWND hCtrl, char* txt);
extern BOOL ExtractAppIcon(const char* filename, BOOL bSilent);
extern BOOL ExtractDOS(const char* path);
extern BOOL ExtractISO(const char* src_iso, const char* dest_dir, BOOL scan);
extern BOOL ExtractZip(const char* src_zip, const char* dest_dir);
extern int64_t ExtractISOFile(const char* iso, const char* iso_file, const char* dest_file, DWORD attributes);
extern uint32_t ReadISOFileToBuffer(const char* iso, const char* iso_file, uint8_t** buf);
extern BOOL CopySKUSiPolicy(const char* drive_name);
extern BOOL HasEfiImgBootLoaders(void);
extern BOOL DumpFatDir(const char* path, int32_t cluster);
extern BOOL InstallSyslinux(DWORD drive_index, char drive_letter, int fs);
extern uint16_t GetSyslinuxVersion(char* buf, size_t buf_size, char** ext);
extern BOOL SetAutorun(const char* path);
extern char* FileDialog(BOOL save, char* path, const ext_t* ext, UINT* selected_ext);
extern BOOL FileIO(enum file_io_type io_type, char* path, char** buffer, DWORD* size);
extern uint8_t* GetResource(HMODULE module, char* name, char* type, const char* desc, DWORD* len, BOOL duplicate);
extern DWORD GetResourceSize(HMODULE module, char* name, char* type, const char* desc);
extern DWORD RunCommandWithProgress(const char* cmdline, const char* dir, BOOL log, int msg);
#define RunCommand(cmd, dir, log) RunCommandWithProgress(cmd, dir, log, 0)
extern BOOL CompareGUID(const GUID *guid1, const GUID *guid2);
extern BOOL MountRegistryHive(const HKEY key, const char* pszHiveName, const char* pszHivePath);
extern BOOL UnmountRegistryHive(const HKEY key, const char* pszHiveName);
extern BOOL SetLGP(BOOL bRestore, BOOL* bExistingKey, const char* szPath, const char* szPolicy, DWORD dwValue);
extern LONG GetEntryWidth(HWND hDropDown, const char* entry);
extern uint64_t DownloadToFileOrBufferEx(const char* url, const char* file, const char* user_agent,
	BYTE** buffer, HWND hProgressDialog, BOOL bTaskBarProgress);
#define DownloadToFileOrBuffer(url, file, buffer, hProgressDialog, bTaskBarProgress) \
	DownloadToFileOrBufferEx(url, file, NULL, buffer, hProgressDialog, bTaskBarProgress)
extern DWORD DownloadSignedFile(const char* url, const char* file, HWND hProgressDialog, BOOL PromptOnError);
extern HANDLE DownloadSignedFileThreaded(const char* url, const char* file, HWND hProgressDialog, BOOL bPromptOnError);
extern INT_PTR CALLBACK UpdateCallback(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
extern BOOL IsShown(HWND hDlg);
extern char* get_token_data_file_indexed(const char* token, const char* filename, int index);
#define get_token_data_file(token, filename) get_token_data_file_indexed(token, filename, 1)
extern char* set_token_data_file(const char* token, const char* data, const char* filename);
extern char* get_token_data_buffer(const char* token, unsigned int n, const char* buffer, size_t buffer_size);
extern char* insert_section_data(const char* filename, const char* section, const char* data, BOOL dos2unix);
extern char* replace_in_token_data(const char* filename, const char* token, const char* src, const char* rep, BOOL dos2unix);
extern char* replace_char(const char* src, const char c, const char* rep);
extern char* remove_substr(const char* src, const char* sub);
extern void parse_update(char* buf, size_t len);
extern void* get_data_from_asn1(const uint8_t* buf, size_t buf_len, const char* oid_str, uint8_t asn1_type, size_t* data_len);
extern int sanitize_label(char* label);
extern int IsHDD(DWORD DriveIndex, uint16_t vid, uint16_t pid, const char* strid);

extern LONG ValidateSignature(HWND hDlg, const char* path);

extern BOOL SetThreadAffinity(DWORD_PTR* thread_affinity, size_t num_threads);
extern BOOL IsSignedBySecureBootAuthority(uint8_t* buf, uint32_t len);
extern int IsBootloaderRevoked(uint8_t* buf, uint32_t len);
extern void PrintRevokedBootloaderInfo(void);
extern BOOL IsBufferInDB(const unsigned char* buf, const size_t len);
#define printbits(x) _printbits(sizeof(x), &x, 0)
#define printbitslz(x) _printbits(sizeof(x), &x, 1)
extern char* _printbits(size_t const size, void const * const ptr, int leading_zeroes);
extern BOOL IsCurrentProcessElevated(void);
extern char* ToLocaleName(DWORD lang_id);
extern void SetAlertPromptMessages(void);
extern BOOL SetAlertPromptHook(void);
extern void ClrAlertPromptHook(void);
extern BOOL StartProcessSearch(void);
extern void StopProcessSearch(void);
extern BOOL SetProcessSearch(DWORD DeviceNum);
extern BYTE GetProcessSearch(uint32_t timeout, uint8_t access_mask, BOOL bIgnoreStaleProcesses);
extern BOOL EnablePrivileges(void);
extern void FlashTaskbar(HANDLE handle);
extern DWORD WaitForSingleObjectWithMessages(HANDLE hHandle, DWORD dwMilliseconds);
extern HICON CreateMirroredIcon(HICON hiconOrg);
extern HANDLE CreatePreallocatedFile(const char* lpFileName, DWORD dwDesiredAccess,
	DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
	DWORD dwFlagsAndAttributes, LONGLONG fileSize);

extern BOOL TakeOwnership(LPCSTR lpszOwnFile);
#define GetTextWidth(hDlg, id) GetTextSize(GetDlgItem(hDlg, id), NULL).cx

DWORD WINAPI HashThread(void* param);

/*
 * typedefs for the function prototypes. Use the something like:
 *   PF_DECL(FormatEx);
 * which translates to:
 *   FormatEx_t pfFormatEx = NULL;
 * in your code, to declare the entrypoint and then use:
 *   PF_INIT(FormatEx, Fmifs);
 * which translates to:
 *   pfFormatEx = (FormatEx_t) GetProcAddress(GetDLLHandle("fmifs"), "FormatEx");
 * to make it accessible.
 */
#define         MAX_LIBRARY_HANDLES 64
extern HMODULE  OpenedLibrariesHandle[MAX_LIBRARY_HANDLES];
extern uint16_t OpenedLibrariesHandleSize;
#define         OPENED_LIBRARIES_VARS HMODULE OpenedLibrariesHandle[MAX_LIBRARY_HANDLES]; uint16_t OpenedLibrariesHandleSize = 0
#define         CLOSE_OPENED_LIBRARIES while(OpenedLibrariesHandleSize > 0) FreeLibrary(OpenedLibrariesHandle[--OpenedLibrariesHandleSize])
static __inline HMODULE GetLibraryHandle(char* szLibraryName) {
	HMODULE h = NULL;
	wchar_t* wszLibraryName = NULL;
	int size;
	if (szLibraryName == NULL || szLibraryName[0] == 0)
		goto out;
	size = MultiByteToWideChar(CP_UTF8, 0, szLibraryName, -1, NULL, 0);
	if ((size <= 1) || ((wszLibraryName = (wchar_t*)calloc(size, sizeof(wchar_t))) == NULL) ||
		(MultiByteToWideChar(CP_UTF8, 0, szLibraryName, -1, wszLibraryName, size) != size))
		goto out;
	// If the library is already opened, just return a handle (that doesn't need to be freed)
	if ((h = GetModuleHandleW(wszLibraryName)) != NULL)
		goto out;
	// Sanity check
	if (OpenedLibrariesHandleSize >= MAX_LIBRARY_HANDLES) {
		uprintf("Error: MAX_LIBRARY_HANDLES is too small\n");
		goto out;
	}
	h = LoadLibraryExW(wszLibraryName, NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (h != NULL)
		OpenedLibrariesHandle[OpenedLibrariesHandleSize++] = h;
	else
		uprintf("Unable to load '%S.dll': %s", wszLibraryName, WindowsErrorString());
out:
	free(wszLibraryName);
	return h;
}
