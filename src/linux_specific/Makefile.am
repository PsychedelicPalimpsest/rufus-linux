noinst_LIBRARIES = liblinux_specific.a

liblinux_specific_a_SOURCES = pseudo_windows.c locale.c winio.c wine_string.c
liblinux_specific_a_CFLAGS = $(AM_CFLAGS) -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../common -Wno-undef -Wno-strict-aliasing -Wno-shadow
