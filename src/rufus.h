/*
 * Rufus: The Reliable USB Formatting Utility
 * Copyright © 2011-2025 Pete Batard <pete@akeo.ie>
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
#pragma once

#include <pseudo_windows.h>

#ifdef _WIN32
#include "windows_specific/win_rufus.h"
#endif

/* Special handling for old .c32 files we need to replace */
#define NB_OLD_C32          2
#define OLD_C32_NAMES       { "menu.c32", "vesamenu.c32" }
#define OLD_C32_THRESHOLD   { 53500, 148000 }


/* Isolate the Syslinux version numbers */
#define SL_MAJOR(x) ((uint8_t)((x)>>8))
#define SL_MINOR(x) ((uint8_t)(x))

typedef struct {
	char* id;
	char* name;
	char* display_name;
	char* label;
	char* hub;
	DWORD index;
	uint32_t port;
	uint64_t size;
} RUFUS_DRIVE;

typedef struct {
	uint16_t version[3];
	uint32_t platform_min[2];		// minimum platform version required
	char* download_url;
	char* release_notes;
} RUFUS_UPDATE;

typedef struct {
	DWORD Type;
	DWORD DeviceNum;
	DWORD BufSize;
	LONGLONG DeviceSize;
	char* DevicePath;
	char* ImagePath;
	char* Label;
} IMG_SAVE;





/**
 * Globals
 */
extern RUFUS_UPDATE update;
extern WORD selected_langid;
extern DWORD ErrorStatus, DownloadStatus, MainThreadId, LastWriteError;
extern BOOL use_own_c32[NB_OLD_C32], detect_fakes, op_in_progress, right_to_left_mode;
extern BOOL allow_dual_uefi_bios, large_drive, usb_debug;
extern uint8_t image_options, *pe256ssp;
extern uint16_t rufus_version[3], embedded_sl_version[2];
extern uint32_t pe256ssp_size;
extern uint64_t persistence_size;
extern int64_t iso_blocking_status;
extern size_t ubuffer_pos;
extern int dialog_showing, force_update, fs_type, boot_type, partition_type, target_type;
extern unsigned long syslinux_ldlinux_len[2];
extern char szFolderPath[MAX_PATH], app_dir[MAX_PATH], temp_dir[MAX_PATH], system_dir[MAX_PATH];
extern char sysnative_dir[MAX_PATH], app_data_dir[MAX_PATH], *image_path, *fido_url;



#if defined(_MSC_VER)
// Disable some VS Code Analysis warnings
#pragma warning(disable: 4996)		// Ignore deprecated
#pragma warning(disable: 6258)		// I know what I'm using TerminateThread for
#pragma warning(disable: 26451)		// Stop bugging me with casts already!
#pragma warning(disable: 28159)		// I'll keep using GetVersionEx(), thank you very much...
// Enable C11's _Static_assert (requires VS2015 or later)
#define _Static_assert static_assert
#endif


/* Convenient to have around */
#define KB                          1024LL
#define MB                          1048576LL
#define GB                          1073741824LL
#define TB                          1099511627776LL
#define PB                          1125899906842624LL




/*
 * Features not ready for prime time and that may *DESTROY* your data - USE AT YOUR OWN RISKS!
 */
//#define RUFUS_TEST

#define APPLICATION_NAME            "Rufus"
#if defined(_M_AMD64)
#define APPLICATION_ARCH            "x64"
#elif defined(_M_IX86)
#define APPLICATION_ARCH            "x86"
#elif defined(_M_ARM64)
#define APPLICATION_ARCH            "Arm64"
#elif defined(_M_ARM)
#define APPLICATION_ARCH            "Arm"
#else
#define APPLICATION_ARCH            "(Unknown Arch)"
#endif
#define COMPANY_NAME                "Akeo Consulting"
#define STR_NO_LABEL                "NO_LABEL"
// Yes, there exist characters between these seemingly empty quotes!
#define LEFT_TO_RIGHT_MARK          "‎"
#define RIGHT_TO_LEFT_MARK          "‏"
#define LEFT_TO_RIGHT_EMBEDDING     "‪"
#define RIGHT_TO_LEFT_EMBEDDING     "‫"
#define POP_DIRECTIONAL_FORMATTING  "‬"
#define LEFT_TO_RIGHT_OVERRIDE      "‭"
#define RIGHT_TO_LEFT_OVERRIDE      "‮"
#define DRIVE_ACCESS_TIMEOUT        15000		// How long we should retry drive access (in ms)
#define DRIVE_ACCESS_RETRIES        150			// How many times we should retry
#define DRIVE_INDEX_MIN             0x00000080
#define DRIVE_INDEX_MAX             0x000000C0
#define MIN_DRIVE_SIZE              (8 * MB)	// Minimum size a drive must have, to be formattable
#define MIN_EXTRA_PART_SIZE         (1 * MB)	// Minimum size of the extra partition, in bytes
#define MIN_EXT_SIZE                (256 * MB)	// Minimum size we allow for ext formatting
#define MAX_DRIVES                  (DRIVE_INDEX_MAX - DRIVE_INDEX_MIN)
#define MAX_TOOLTIPS                128
#define MAX_SIZE_SUFFIXES           6			// bytes, KB, MB, GB, TB, PB
#define MAX_CLUSTER_SIZES           18
#define MAX_PROGRESS                0xFFFF
#define PATCH_PROGRESS_TOTAL        207
#define MAX_LOG_SIZE                0x7FFFFFFE
#define MAX_REFRESH                 25			// How long we should wait to refresh UI elements (in ms)
#define MARQUEE_TIMER_REFRESH       10			// Time between progress bar marquee refreshes, in ms
#define MAX_GUID_STRING_LENGTH      40
#define MAX_PARTITIONS              16			// Maximum number of partitions we handle
#define MAX_ESP_TOGGLE              8			// Maximum number of entries we record to toggle GPT ESP back and forth
#define MAX_IGNORE_USB              8			// Maximum number of USB drives we want to ignore
#define MAX_ISO_TO_ESP_SIZE         (1 * GB)	// Maximum size we allow for the ISO → ESP option
#define MAX_DEFAULT_LIST_CARD_SIZE  (500 * GB)	// Size above which we don't list a card without enable HDD or Alt-F
#define MAX_SECTORS_TO_CLEAR        128			// nb sectors to zap when clearing the MBR/GPT (must be >34)
#define MAX_USERNAME_LENGTH         128			// Maximum size we'll accept for a WUE specified username
#define MAX_WININST                 4			// Max number of install[.wim|.esd] we can handle on an image
#define MBR_UEFI_MARKER             0x49464555	// 'U', 'E', 'F', 'I', as a 32 bit little endian longword
#define MORE_INFO_URL               0xFFFF
#define PROJECTED_SIZE_RATIO        110			// Percentage by which we inflate projected_size to prevent persistence overflow
#define STATUS_MSG_TIMEOUT          3500		// How long should cheat mode messages appear for on the status bar
#define WRITE_RETRIES               4
#define WRITE_TIMEOUT               5000		// How long we should wait between write retries (in ms)
#define SEARCH_PROCESS_TIMEOUT      5000		// How long we should wait to get the conflicting process data (in ms)
#define NET_SESSION_TIMEOUT         3500		// How long we should wait to connect, send or receive internet data (in ms)
#define FS_DEFAULT                  FS_FAT32
#define SINGLE_CLUSTERSIZE_DEFAULT  0x00000100
#define BADLOCKS_PATTERN_TYPES      5
#define BADBLOCK_PATTERN_COUNT      4
#define BADBLOCK_PATTERN_ONE_PASS   {0x55, 0x00, 0x00, 0x00}
#define BADBLOCK_PATTERN_TWO_PASSES {0x55, 0xaa, 0x00, 0x00}
#define BADBLOCK_PATTERN_SLC        {0x00, 0xff, 0x55, 0xaa}
#define BADCLOCK_PATTERN_MLC        {0x00, 0xff, 0x33, 0xcc}
#define BADBLOCK_PATTERN_TLC        {0x00, 0xff, 0x1c71c7, 0xe38e38}
#define BADBLOCK_BLOCK_SIZE         (512 * KB)
#define LARGE_FAT32_SIZE            (32 * GB)	// Size at which we need to use fat32format
#define UDF_FORMAT_SPEED            3.1f		// Speed estimate at which we expect UDF drives to be formatted (GB/s)
#define UDF_FORMAT_WARN             20			// Duration (in seconds) above which we warn about long UDF formatting times
#define MAX_FAT32_SIZE              (2 * TB)	// Threshold above which we disable FAT32 formatting
#define FAT32_CLUSTER_THRESHOLD     1.011f		// For FAT32, cluster size changes don't occur at power of 2 boundaries but slightly above
#define DD_BUFFER_SIZE              (32 * MB)	// Minimum size of buffer to use for DD operations
#define UBUFFER_SIZE                4096
#define ISO_BUFFER_SIZE             (64 * KB)	// Buffer size used for ISO data extraction
#define RSA_SIGNATURE_SIZE          256
#define CBN_SELCHANGE_INTERNAL      (CBN_SELCHANGE + 256)
#if defined(RUFUS_TEST)
#define RUFUS_URL                   "http://nas/~rufus"
#else
#define RUFUS_URL                   "https://rufus.ie"
#endif
#define DOWNLOAD_URL                RUFUS_URL "/downloads"
#define FILES_URL                   RUFUS_URL "/files"
#define FILES_DIR                   APPLICATION_NAME
#define FIDO_VERSION                "z1"
#define WPPRECORDER_MORE_INFO_URL   "https://github.com/pbatard/rufus/wiki/FAQ#bsods-with-windows-to-go-drives-created-from-windows-10-1809-isos"
#define SEVENZIP_URL                "https://7-zip.org/"
// Generated by following https://randomascii.wordpress.com/2013/03/09/symbols-the-microsoft-way/
#define DISKCOPY_URL                "https://msdl.microsoft.com/download/symbols/diskcopy.dll/54505118173000/diskcopy.dll"
#define DISKCOPY_SIZE               0x16ee00
#define DISKCOPY_IMAGE_OFFSET       0x66d8
#define DISKCOPY_IMAGE_SIZE         0x168000
#define SYMBOL_SERVER_USER_AGENT    "Microsoft-Symbol-Server/10.0.22621.755"
#define DEFAULT_ESP_MOUNT_POINT     "S:\\"
#define IS_POWER_OF_2(x)            ((x != 0) && (((x) & ((x) - 1)) == 0))
#define IGNORE_RETVAL(expr)         do { (void)(expr); } while(0)
#ifndef ARRAYSIZE
#define ARRAYSIZE(A)                (sizeof(A)/sizeof((A)[0]))
#endif
#ifndef STRINGIFY
#define STRINGIFY(x)                #x
#endif
#define PERCENTAGE(percent, value)  ((1ULL * (percent) * (value)) / 100ULL)
#define IsChecked(CheckBox_ID)      (IsDlgButtonChecked(hMainDialog, CheckBox_ID) == BST_CHECKED)
#define MB_IS_RTL                   (right_to_left_mode?MB_RTLREADING|MB_RIGHT:0)
#define CHECK_FOR_USER_CANCEL       if (IS_ERROR(ErrorStatus) && (SCODE_CODE(ErrorStatus) == ERROR_CANCELLED)) goto out
// Bit masks used for the display of additional image options in the UI
#define IMOP_WINTOGO                0x01
#define IMOP_PERSISTENCE            0x02

#define safe_free(p) do { free((void*)p); p = NULL; } while(0)
#define safe_mm_free(p) do { _mm_free((void*)p); p = NULL; } while(0)
static __inline void safe_strcp(char* dst, const size_t dst_max, const char* src, const size_t count) {
	memmove(dst, src, min(count, dst_max));
	dst[min(count, dst_max) - 1] = 0;
}
#define safe_strcpy(dst, dst_max, src) safe_strcp(dst, dst_max, src, safe_strlen(src) + 1)
#define static_strcpy(dst, src) safe_strcpy(dst, sizeof(dst), src)
#define safe_strcat(dst, dst_max, src) strncat_s(dst, dst_max, src, _TRUNCATE)
#define static_strcat(dst, src) safe_strcat(dst, sizeof(dst), src)
#define safe_strcmp(str1, str2) strcmp(((str1 == NULL) ? "<NULL>" : str1), ((str2 == NULL) ? "<NULL>" : str2))
#define safe_strstr(str1, str2) strstr(((str1 == NULL) ? "<NULL>" : str1), ((str2 == NULL) ? "<NULL>" : str2))
#define safe_stricmp(str1, str2) _stricmp(((str1 == NULL) ? "<NULL>" : str1), ((str2 == NULL) ? "<NULL>" : str2))
#define safe_strncmp(str1, str2, count) strncmp(((str1 == NULL) ? "<NULL>" : str1), ((str2 == NULL) ? "<NULL>" : str2), count)
#define safe_strnicmp(str1, str2, count) _strnicmp(((str1 == NULL) ? "<NULL>" : str1), ((str2 == NULL) ? "<NULL>" : str2), count)
#define safe_closehandle(h) do { if ((h != INVALID_HANDLE_VALUE) && (h != NULL)) { CloseHandle(h); h = INVALID_HANDLE_VALUE; } } while(0)
#define safe_release_dc(hDlg, hDC) do { if ((hDC != INVALID_HANDLE_VALUE) && (hDC != NULL)) { ReleaseDC(hDlg, hDC); hDC = NULL; } } while(0)
#define safe_sprintf(dst, count, ...) do { size_t _count = count; char* _dst = dst; _snprintf_s(_dst, _count, _TRUNCATE, __VA_ARGS__); \
	_dst[(_count) - 1] = 0; } while(0)
#define static_sprintf(dst, ...) safe_sprintf(dst, sizeof(dst), __VA_ARGS__)
#define safe_atoi(str) ((((char*)(str))==NULL) ? 0 : atoi(str))
#define safe_strlen(str) ((((char*)(str))==NULL) ? 0 : strlen(str))
#define safe_strdup(str) ((((char*)(str))==NULL) ? NULL : _strdup(str))



#if defined(_MSC_VER)
#define safe_vsnprintf(buf, size, format, arg) _vsnprintf_s(buf, size, _TRUNCATE, format, arg)
#else
#define safe_vsnprintf vsnprintf
#endif
#define safe_strtolower(str) do { if (str != NULL) CharLowerA(str); } while(0)
#define safe_strtoupper(str) do { if (str != NULL) CharUpperA(str); } while(0)
static __inline void static_repchr(char* p, char s, char r) {
	if (p != NULL) while (*p != 0) { if (*p == s) *p = r; p++; }
}
#define to_unix_path(str) static_repchr(str, '\\', '/')
#define to_windows_path(str) static_repchr(str, '/', '\\')
#define if_not_assert(cond) assert(cond); if (!(cond))


extern void uprintf(const char *format, ...);
extern void uprintfs(const char *str);
#define vuprintf(...) do { if (verbose) uprintf(__VA_ARGS__); } while(0)
#define vvuprintf(...) do { if (verbose > 1) uprintf(__VA_ARGS__); } while(0)
#define suprintf(...) do { if (!bSilent) uprintf(__VA_ARGS__); } while(0)
#define uuprintf(...) do { if (usb_debug) uprintf(__VA_ARGS__); } while(0)
#define ubprintf(...) do { safe_sprintf(&ubuffer[ubuffer_pos], UBUFFER_SIZE - ubuffer_pos - 4, __VA_ARGS__); \
	ubuffer_pos = strlen(ubuffer); ubuffer[ubuffer_pos++] = '\r'; ubuffer[ubuffer_pos++] = '\n'; \
	ubuffer[ubuffer_pos] = 0; } while(0)
#define ubflush() do { if (ubuffer_pos) uprintf("%s", ubuffer); ubuffer_pos = 0; } while(0)
#ifdef _DEBUG
#define duprintf uprintf
#else
#define duprintf(...)
#endif
