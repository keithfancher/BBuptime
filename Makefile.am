CPPFLAGS =	@CPPFLAGS@ \
		-DDEFAULT_CONF=\"$(datadir)/bbtools/bbuptime.bb\" \
		-DDEFAULT_NOBB=\"$(datadir)/bbtools/bbuptime.nobb\" \
    -DDEFAULT_LOCAL=\".bbtools/bbuptime.bb\" \
    -DLOCAL_NOBB=\".bbtools/bbuptime.nobb\"


SUBDIRS = data
bin_PROGRAMS = bbuptime
bbuptime_SOURCES = bbuptime.cc Image.cc LinkedList.cc \
		 bbuptime.hh Image.hh LinkedList.hh \
		 resource.cc resource.hh \
		 Baseresource.cc Baseresource.hh \
		 Basewindow.cc Basewindow.hh \
		 blackboxstyle.hh
EXTRA_DIST = BUGS
bbuptime_LDADD = @X_LIBS@
