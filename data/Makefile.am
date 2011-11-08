uidir = $(datadir)/mate-calc
ui_DATA = \
	preferences.ui \
	buttons-basic.ui \
	buttons-advanced.ui \
	buttons-financial.ui \
	buttons-programming.ui

gsettings_SCHEMAS = org.mate.mate-calc.gschema.xml
@INTLTOOL_XML_NOMERGE_RULE@
@GSETTINGS_RULES@

Utilitiesdir = $(datadir)/applications
Utilities_in_files = mate-calc.desktop.in
Utilities_DATA = $(Utilities_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

man1_MANS = mate-calc.1

EXTRA_DIST = $(ui_DATA) org.mate.mate-calc.gschema.xml.in mate-calc.desktop.in $(man1_MANS)

DISTCLEANFILES = \
	Makefile.in \
	mate-calc.desktop \
	$(gsettings_SCHEMAS)
