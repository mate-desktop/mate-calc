SUBDIRS = po src data help

# Temporary fix for JHBuild, see https://bugzilla.gnome.org/show_bug.cgi?id=641652
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}

DISTCLEANFILES = \
	Makefile.in \
	aclocal.m4 \
	config.h.in \
	configure \
	depcomp \
	install-sh \
	missing \
	mkinstalldirs \
	omf.make \
	xmldocs.make

EXTRA_DIST = \
	autogen.sh


# Build ChangeLog from GIT  history
ChangeLog:
	$(AM_V_GEN) if test -d $(top_srcdir)/.git; then \
		GIT_DIR="$(top_srcdir)/.git" git log --stat > $@; \
	fi

dist: ChangeLog

.PHONY: ChangeLog
