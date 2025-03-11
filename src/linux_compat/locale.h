/*
 * Rufus: The Reliable USB Formatting Utility
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


#ifndef _WIN32

INT MultiByteToWideChar( UINT codepage, DWORD flags, const char *src, INT srclen,
                                                  WCHAR *dst, INT dstlen );

INT WideCharToMultiByte( UINT codepage, DWORD flags, LPCWSTR src, INT srclen,
                                                  LPSTR dst, INT dstlen, LPCSTR defchar, BOOL *used );

// Random constants that might be used (from wine):
#define LOCALE_NAME_USER_DEFAULT    NULL

#define NORM_IGNORECASE            0x00000001
#define NORM_IGNORENONSPACE        0x00000002
#define NORM_IGNORESYMBOLS         0x00000004
#define SORT_DIGITSASNUMBERS       0x00000008
#define LINGUISTIC_IGNORECASE      0x00000010
#define LINGUISTIC_IGNOREDIACRITIC 0x00000020
#define SORT_STRINGSORT            0x00001000 /* Take punctuation into account */
#define NORM_IGNOREKANATYPE        0x00010000
#define NORM_IGNOREWIDTH           0x00020000
#define NORM_LINGUISTIC_CASING     0x08000000
#define FIND_STARTSWITH            0x00100000
#define FIND_ENDSWITH              0x00200000
#define FIND_FROMSTART             0x00400000
#define FIND_FROMEND               0x00800000

#define CP_ACP        0
#define CP_OEMCP      1
#define CP_MACCP      2
#define CP_THREAD_ACP 3
#define CP_SYMBOL     42
#define CP_UTF7       65000
#define CP_UTF8       65001

#define CP_UNIXCP     65010 /* Wine extension */

#define CP_INSTALLED 0x1
#define CP_SUPPORTED 0x2

#define HIGH_SURROGATE_START 0xd800
#define HIGH_SURROGATE_END   0xdbff
#define LOW_SURROGATE_START  0xdc00
#define LOW_SURROGATE_END    0xdfff

#define IS_HIGH_SURROGATE(ch)       ((ch) >= HIGH_SURROGATE_START && (ch) <= HIGH_SURROGATE_END)
#define IS_LOW_SURROGATE(ch)        ((ch) >= LOW_SURROGATE_START  && (ch) <= LOW_SURROGATE_END)
#define IS_SURROGATE_PAIR(high,low) (IS_HIGH_SURROGATE(high) && IS_LOW_SURROGATE(low))

#define WC_DISCARDNS         0x0010
#define WC_SEPCHARS          0x0020
#define WC_DEFAULTCHAR       0x0040
#define WC_ERR_INVALID_CHARS 0x0080
#define WC_COMPOSITECHECK    0x0200
#define WC_NO_BEST_FIT_CHARS 0x0400

#define MAP_FOLDCZONE        0x0010
#define MAP_PRECOMPOSED      0x0020
#define MAP_COMPOSITE        0x0040
#define MAP_FOLDDIGITS       0x0080
#define MAP_EXPAND_LIGATURES 0x2000


/* String mapping flags */
#define LCMAP_LOWERCASE  0x00000100 /* Make lower-case */
#define LCMAP_UPPERCASE  0x00000200 /* Make upper-case */
#define LCMAP_TITLECASE  0x00000300 /* Make title-case */
#define LCMAP_SORTKEY    0x00000400 /* Create a sort key */
#define LCMAP_BYTEREV    0x00000800 /* Reverse the result */
#define LCMAP_HASH       0x00040000
#define LCMAP_HIRAGANA   0x00100000 /* Transform Japanese katakana into hiragana */
#define LCMAP_KATAKANA   0x00200000 /* Transform Japanese hiragana into katakana */
#define LCMAP_HALFWIDTH  0x00400000 /* Use single byte chars in output */
#define LCMAP_FULLWIDTH  0x00800000 /* Use double byte chars in output */
#define LCMAP_LINGUISTIC_CASING   0x01000000 /* Change case by using language context */
#define LCMAP_SIMPLIFIED_CHINESE  0x02000000 /* Transform Chinese traditional into simplified */
#define LCMAP_TRADITIONAL_CHINESE 0x04000000 /* Transform Chinese simplified into traditional */
#define LCMAP_SORTHANDLE 0x20000000

/* Date and time formatting flags */
#define DATE_SHORTDATE          0x01  /* Short date format */
#define DATE_LONGDATE           0x02  /* Long date format */
#define DATE_USE_ALT_CALENDAR   0x04  /* Use an Alternate calendar */
#define DATE_YEARMONTH          0x08  /* Year/month format */
#define DATE_LTRREADING         0x10  /* Add LTR reading marks */
#define DATE_RTLREADING         0x20  /* Add RTL reading marks */
#define DATE_AUTOLAYOUT         0x40  /* Add LTR or RTL reading marks automatically */
#define DATE_MONTHDAY           0x80  /* Month/day format */

#define TIME_FORCE24HOURFORMAT  0x08  /* Always use 24 hour clock */
#define TIME_NOTIMEMARKER       0x04  /* show no AM/PM */
#define TIME_NOSECONDS          0x02  /* show no seconds */
#define TIME_NOMINUTESORSECONDS 0x01  /* show no minutes either */

/* Unicode char type flags */
#define CT_CTYPE1       0x0001  /* usual ctype */
#define CT_CTYPE2       0x0002  /* bidirectional layout info */
#define CT_CTYPE3       0x0004  /* textprocessing info */

/* Type 1 flags */
#define C1_UPPER        0x0001
#define C1_LOWER        0x0002
#define C1_DIGIT        0x0004
#define C1_SPACE        0x0008
#define C1_PUNCT        0x0010
#define C1_CNTRL        0x0020
#define C1_BLANK        0x0040
#define C1_XDIGIT       0x0080
#define C1_ALPHA        0x0100
#define C1_DEFINED      0x0200

/* Type 2 flags */
#define C2_LEFTTORIGHT      0x0001
#define C2_RIGHTTOLEFT      0x0002
#define C2_EUROPENUMBER     0x0003
#define C2_EUROPESEPARATOR  0x0004
#define C2_EUROPETERMINATOR 0x0005
#define C2_ARABICNUMBER     0x0006
#define C2_COMMONSEPARATOR  0x0007
#define C2_BLOCKSEPARATOR   0x0008
#define C2_SEGMENTSEPARATOR 0x0009
#define C2_WHITESPACE       0x000A
#define C2_OTHERNEUTRAL     0x000B
#define C2_NOTAPPLICABLE    0x0000

/* Type 3 flags */
#define C3_NONSPACING       0x0001
#define C3_DIACRITIC        0x0002
#define C3_VOWELMARK        0x0004
#define C3_SYMBOL       0x0008
#define C3_KATAKANA     0x0010
#define C3_HIRAGANA     0x0020
#define C3_HALFWIDTH        0x0040
#define C3_FULLWIDTH        0x0080
#define C3_IDEOGRAPH        0x0100
#define C3_KASHIDA      0x0200
#define C3_LEXICAL      0x0400
#define C3_HIGHSURROGATE    0x0800
#define C3_LOWSURROGATE     0x1000
#define C3_ALPHA        0x8000
#define C3_NOTAPPLICABLE    0x0000

/* Code page information.
 */
#define MAX_LEADBYTES     12
#define MAX_DEFAULTCHAR   2

/* Defines for calendar handling */
#define CAL_NOUSEROVERRIDE        LOCALE_NOUSEROVERRIDE
#define CAL_USE_CP_ACP            LOCALE_USE_CP_ACP
#define CAL_RETURN_NUMBER         LOCALE_RETURN_NUMBER
#define CAL_RETURN_GENITIVE_NAMES LOCALE_RETURN_GENITIVE_NAMES

#define CAL_ICALINTVALUE       0x01
#define CAL_SCALNAME           0x02
#define CAL_IYEAROFFSETRANGE   0x03
#define CAL_SERASTRING         0x04
#define CAL_SSHORTDATE         0x05
#define CAL_SLONGDATE          0x06
#define CAL_SDAYNAME1          0x07
#define CAL_SDAYNAME2          0x08
#define CAL_SDAYNAME3          0x09
#define CAL_SDAYNAME4          0x0a
#define CAL_SDAYNAME5          0x0b
#define CAL_SDAYNAME6          0x0c
#define CAL_SDAYNAME7          0x0d
#define CAL_SABBREVDAYNAME1    0x0e
#define CAL_SABBREVDAYNAME2    0x0f
#define CAL_SABBREVDAYNAME3    0x10
#define CAL_SABBREVDAYNAME4    0x11
#define CAL_SABBREVDAYNAME5    0x12
#define CAL_SABBREVDAYNAME6    0x13
#define CAL_SABBREVDAYNAME7    0x14
#define CAL_SMONTHNAME1        0x15
#define CAL_SMONTHNAME2        0x16
#define CAL_SMONTHNAME3        0x17
#define CAL_SMONTHNAME4        0x18
#define CAL_SMONTHNAME5        0x19
#define CAL_SMONTHNAME6        0x1a
#define CAL_SMONTHNAME7        0x1b
#define CAL_SMONTHNAME8        0x1c
#define CAL_SMONTHNAME9        0x1d
#define CAL_SMONTHNAME10       0x1e
#define CAL_SMONTHNAME11       0x1f
#define CAL_SMONTHNAME12       0x20
#define CAL_SMONTHNAME13       0x21
#define CAL_SABBREVMONTHNAME1  0x22
#define CAL_SABBREVMONTHNAME2  0x23
#define CAL_SABBREVMONTHNAME3  0x24
#define CAL_SABBREVMONTHNAME4  0x25
#define CAL_SABBREVMONTHNAME5  0x26
#define CAL_SABBREVMONTHNAME6  0x27
#define CAL_SABBREVMONTHNAME7  0x28
#define CAL_SABBREVMONTHNAME8  0x29
#define CAL_SABBREVMONTHNAME9  0x2a
#define CAL_SABBREVMONTHNAME10 0x2b
#define CAL_SABBREVMONTHNAME11 0x2c
#define CAL_SABBREVMONTHNAME12 0x2d
#define CAL_SABBREVMONTHNAME13 0x2e
#define CAL_SYEARMONTH         0x2f
#define CAL_ITWODIGITYEARMAX   0x30
#define CAL_SSHORTESTDAYNAME1  0x31
#define CAL_SSHORTESTDAYNAME2  0x32
#define CAL_SSHORTESTDAYNAME3  0x33
#define CAL_SSHORTESTDAYNAME4  0x34
#define CAL_SSHORTESTDAYNAME5  0x35
#define CAL_SSHORTESTDAYNAME6  0x36
#define CAL_SSHORTESTDAYNAME7  0x37
#define CAL_SMONTHDAY          0x38
#define CAL_SABBREVERASTRING   0x39
#define CAL_SRELATIVELONGDATE  0x3a
#define CAL_SENGLISHERANAME    0x3b
#define CAL_SENGLISHABBREVERANAME 0x3c

/* Calendar types */
#define CAL_GREGORIAN              1
#define CAL_GREGORIAN_US           2
#define CAL_JAPAN                  3
#define CAL_TAIWAN                 4
#define CAL_KOREA                  5
#define CAL_HIJRI                  6
#define CAL_THAI                   7
#define CAL_HEBREW                 8
#define CAL_GREGORIAN_ME_FRENCH    9
#define CAL_GREGORIAN_ARABIC       10
#define CAL_GREGORIAN_XLIT_ENGLISH 11
#define CAL_GREGORIAN_XLIT_FRENCH  12
#define CAL_PERSIAN                22
#define CAL_UMALQURA               23

/* EnumCalendarInfo Flags */
#define ENUM_ALL_CALENDARS 0xffffffff /* Enumerate all calendars within a locale */

/* CompareString results */
#define CSTR_LESS_THAN    1
#define CSTR_EQUAL        2
#define CSTR_GREATER_THAN 3

/*
 * Language Group IDs.
 * Resources in kernel32 are LGRPID_xxx+0x2000 because low values were used by LOCALE_xxx
 * This is done because resources in win2k kernel32 / winxp kernel32 are not even
 * stored the same way.
 */
#define LGRPID_WESTERN_EUROPE      0x01 /* Includes US and Africa */
#define LGRPID_CENTRAL_EUROPE      0x02
#define LGRPID_BALTIC              0x03
#define LGRPID_GREEK               0x04
#define LGRPID_CYRILLIC            0x05
#define LGRPID_TURKISH             0x06
#define LGRPID_JAPANESE            0x07
#define LGRPID_KOREAN              0x08
#define LGRPID_TRADITIONAL_CHINESE 0x09
#define LGRPID_SIMPLIFIED_CHINESE  0x0A
#define LGRPID_THAI                0x0B
#define LGRPID_HEBREW              0x0C
#define LGRPID_ARABIC              0x0D
#define LGRPID_VIETNAMESE          0x0E
#define LGRPID_INDIC               0x0F
#define LGRPID_GEORGIAN            0x10
#define LGRPID_ARMENIAN            0x11

/* IDN defines. */
#define IDN_ALLOW_UNASSIGNED        0x1
#define IDN_USE_STD3_ASCII_RULES    0x2

/* MUI defines. */
#define MUI_LANGUAGE_ID                     0x04
#define MUI_LANGUAGE_NAME                   0x08
#define MUI_MERGE_SYSTEM_FALLBACK           0x10
#define MUI_MERGE_USER_FALLBACK             0x20
#define MUI_UI_FALLBACK                     MUI_MERGE_SYSTEM_FALLBACK | MUI_MERGE_USER_FALLBACK
#define MUI_THREAD_LANGUAGES                0x40
#define MUI_CONSOLE_FILTER                  0x100
#define MUI_COMPLEX_SCRIPT_FILTER           0x200
#define MUI_RESET_FILTERS                   0x001
#define MUI_USER_PREFERRED_UI_LANGUAGES     0x10
#define MUI_USE_INSTALLED_LANGUAGES         0x20
#define MUI_USE_SEARCH_ALL_LANGUAGES        0x40
#define MUI_LANG_NEUTRAL_PE_FILE            0x100
#define MUI_NON_LANG_NEUTRAL_FILE           0x200
#define MUI_MACHINE_LANGUAGE_SETTINGS       0x400
#define MUI_FILETYPE_NOT_LANGUAGE_NEUTRAL   0x001
#define MUI_FILETYPE_LANGUAGE_NEUTRAL_MAIN  0x002
#define MUI_FILETYPE_LANGUAGE_NEUTRAL_MUI   0x004
#define MUI_QUERY_TYPE                      0x001
#define MUI_QUERY_CHECKSUM                  0x002
#define MUI_QUERY_LANGUAGE_NAME             0x004
#define MUI_QUERY_RESOURCE_TYPES            0x008
#define MUI_FILEINFO_VERSION                0x001
#define MUI_FULL_LANGUAGE                   0x01
#define MUI_PARTIAL_LANGUAGE                0x02
#define MUI_LIP_LANGUAGE                    0x04
#define MUI_LANGUAGE_INSTALLED              0x20
#define MUI_LANGUAGE_LICENSED               0x40



#endif