appdatadir = $(datadir)/metainfo
appdata_in_files = mate-calc.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)
$(appdata_DATA): $(appdata_in_files)
	$(AM_V_GEN) $(MSGFMT) --xml --template $< -d $(top_srcdir)/po -o $@

gsettings_SCHEMAS = org.mate.calc.gschema.xml
@GSETTINGS_RULES@

Utilitiesdir = $(datadir)/applications
Utilities_in_files = mate-calc.desktop.in
Utilities_DATA = $(Utilities_in_files:.desktop.in=.desktop)
$(Utilities_DATA): $(Utilities_in_files)
	$(AM_V_GEN) $(MSGFMT) --desktop --keyword= --keyword=Name --keyword=GenericName --keyword=Comment --keyword=Keywords --template $< -d $(top_srcdir)/po -o $@

man1_MANS = mate-calc.1 mate-calc-cmd.1

EXTRA_DIST = \
	mate-calc.desktop.in \
	$(appdata_in_files) \
	$(gsettings_SCHEMAS) \
	$(man1_MANS)

DISTCLEANFILES = \
	Makefile.in \
	mate-calc.desktop \
	$(appdata_DATA)
