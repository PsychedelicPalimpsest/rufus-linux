noinst_LIBRARIES = libcommon.a

# TODO
libcommon_a_SOURCES = rufus.c hash.c stdio.c dos_locale.c stdfn.c
libcommon_a_CFLAGS = $(AM_CFLAGS) -I$(srcdir) -I$(srcdir)/.. -Wno-undef -Wno-strict-aliasing -Wno-shadow


if BUILD_LINUX
libcommon_a_CFLAGS += -I$(srcdir)/../linux_specific
endif
if BUILD_WINDOWS
libcommon_a_CFLAGS += -I$(srcdir)/../windows_specific
endif

