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

#include "rufus.h"



BOOL is_x86_64, use_own_c32[NB_OLD_C32] = { FALSE, FALSE }, mbr_selected_by_user = FALSE, lock_drive = TRUE;
BOOL op_in_progress = TRUE, right_to_left_mode = FALSE, has_uefi_csm = FALSE, its_a_me_mario = FALSE;
BOOL enable_HDDs = FALSE, enable_VHDs = TRUE, enable_ntfs_compression = FALSE, no_confirmation_on_cancel = FALSE;
BOOL advanced_mode_device, advanced_mode_format, allow_dual_uefi_bios, detect_fakes, enable_vmdk, force_large_fat32;
BOOL usb_debug, use_fake_units, preserve_timestamps = FALSE, fast_zeroing = FALSE, app_changed_size = FALSE;
BOOL zero_drive = FALSE, list_non_usb_removable_drives = FALSE, enable_file_indexing, large_drive = FALSE;
BOOL write_as_image = FALSE, write_as_esp = FALSE, use_vds = FALSE, ignore_boot_marker = FALSE;
BOOL appstore_version = FALSE, is_vds_available = TRUE, persistent_log = FALSE, has_ffu_support = FALSE;
BOOL expert_mode = FALSE, use_rufus_mbr = TRUE;