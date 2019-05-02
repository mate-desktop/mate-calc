@INTLTOOL_XML_RULE@
appdatadir = $(datadir)/metainfo
appdata_in_files = mate-calc.appdata.xml.in
appdata_DATA = $(appdata_in_files:.xml.in=.xml)

gsettings_SCHEMAS = org.mate.calc.gschema.xml
@GSETTINGS_RULES@

Utilitiesdir = $(datadir)/applications
Utilities_in_files = mate-calc.desktop.in
Utilities_DATA = $(Utilities_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

man1_MANS = mate-calc.1 mate-calc-cmd.1

EXTRA_DIST = \
	org.mate.calc.gschema.xml.in \
	mate-calc.desktop.in \
	$(appdata_in_files) \
	$(man1_MANS)

DISTCLEANFILES = \
	Makefile.in \
	mate-calc.desktop \
	$(appdata_DATA) \
	$(gsettings_SCHEMAS)
