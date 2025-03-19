noinst_LIBRARIES = liblinux_specific.a

liblinux_specific_a_SOURCES = pseudo_windows.c locale.c winio.c wine_string.c unix_resources.c
liblinux_specific_a_CFLAGS = $(shell pkg-config --cflags gtk4 glib-2.0) $(AM_CFLAGS) -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../common -Wno-undef -Wno-strict-aliasing -Wno-shadow




EXTRA_DIST = ../unix_resources.xml

$(srcdir)/../unix_resources.xml:
	python $(srcdir)/../generate_unix_assets.py

unix_resources.c: $(srcdir)/../unix_resources.xml
	glib-compile-resources --target=unix_resources.c --generate-source $(srcdir)/../GResources.xml --sourcedir=$(srcdir)/../../res
