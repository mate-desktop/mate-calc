bin_PROGRAMS = mate-calc mate-calc-cmd
noinst_PROGRAMS = test-mp test-mp-equation

TESTS = test-mp test-mp-equation

INCLUDES = \
	-DUI_DIR=\""$(datadir)/mate-calc"\" \
	-DVERSION=\""$(VERSION)"\" \
	-DLOCALE_DIR=\""$(localedir)"\" \
	-DGETTEXT_PACKAGE=\"$(GETTEXT_PACKAGE)\" \
	$(WARN_CFLAGS) \
	$(MATE_CALC_CFLAGS)

mate_calc_SOURCES = \
	mate-calc.c \
	currency.c \
	currency.h \
	currency-manager.c \
	currency-manager.h \
	math-buttons.c \
	math-buttons.h \
	math-converter.c \
	math-converter.h \
	math-display.c \
	math-display.h \
	math-equation.c \
	math-equation.h \
	math-preferences.c \
	math-preferences.h \
	math-variables.c \
	math-variables.h \
	math-variable-popup.c \
	math-variable-popup.h \
	math-window.c \
	math-window.h \
	mp.c \
	mp.h \
	mp-binary.c \
	mp-convert.c \
	mp-enums.c \
	mp-enums.h \
	mp-equation.c \
	mp-equation.h \
	mp-private.h \
	mp-serializer.c \
	mp-serializer.h \
	mp-trigonometric.c \
	financial.c \
	financial.h \
  	unit.c \
	unit.h \
	unit-category.c \
	unit-category.h \
	unit-manager.c \
	unit-manager.h \
	prelexer.c \
	prelexer.h \
	lexer.c \
	lexer.h \
	parserfunc.c \
	parserfunc.h \
	parser.c \
	parser.h

mate_calc_LDADD = \
	$(MATE_CALC_LIBS)        

mate_calc_cmd_SOURCES = \
	mate-calc-cmd.c \
	currency.c \
	currency.h \
	currency-manager.c \
	currency-manager.h \
	mp.c \
	mp-binary.c \
	mp-convert.c \
	mp-enums.c \
	mp-enums.h \
	mp-equation.c \
	mp-serializer.c \
	mp-serializer.h\
	mp-trigonometric.c \
	unit.c \
	unit.h \
	unit-category.c \
	unit-category.h \
	unit-manager.c \
	unit-manager.h \
	prelexer.c \
	prelexer.h \
	lexer.c \
	lexer.h \
	parserfunc.c \
	parserfunc.h \
	parser.c \
	parser.h

mate_calc_cmd_LDADD = \
	$(MATE_CALC_CMD_LIBS) \
	-lm

test_mp_SOURCES = \
	test-mp.c \
	mp.c \
	mp-binary.c \
	mp-convert.c \
	mp-enums.c \
	mp-enums.h \
	mp-serializer.c \
	mp-serializer.h \
	mp-trigonometric.c

test_mp_LDADD = \
	$(MATE_CALC_CMD_LIBS) \
	-lm

test_mp_equation_SOURCES = \
	test-mp-equation.c \
	currency.c \
	currency.h \
	currency-manager.c \
	currency-manager.h \
	mp.c \
	mp-convert.c \
	mp-binary.c \
	mp-enums.c \
	mp-enums.h \
	mp-equation.c \
	mp-serializer.c \
	mp-serializer.h \
	mp-trigonometric.c \
	unit.c \
	unit.h \
	unit-category.c \
	unit-category.h \
	unit-manager.c \
	unit-manager.h \
	prelexer.c \
	prelexer.h \
	lexer.c \
	lexer.h \
	parserfunc.c \
	parserfunc.h \
	parser.c \
	parser.h

test_mp_equation_LDADD = \
	$(MATE_CALC_CMD_LIBS) \
	-lm

CLEANFILES = \
	mp-enums.c \
	mp-enums.h

# Generate enum types
mp-enums.h: mp-enums.h.template mp-serializer.h
	$(AM_V_GEN)$(GLIB_MKENUMS) --template $(srcdir)/mp-enums.h.template $(srcdir)/mp-serializer.h > mp-enums.h

mp-enums.c: mp-enums.c.template mp-enums.h mp-serializer.h
	$(AM_V_GEN)$(GLIB_MKENUMS) --template $(srcdir)/mp-enums.c.template $(srcdir)/mp-serializer.h > mp-enums.c

# Fix dependencies
math-serializer.c: mp-enums.h
math-equation.c: mp-enums.h

# Install a symlink between mate-calc and mate-calculator
install-exec-hook:
	test -e "$(DESTDIR)$(bindir)/mate-calculator" \
	|| (cd "$(DESTDIR)$(bindir)" && ln -s mate-calc mate-calculator)

# Remove the symlink between mate-calc and mate-calculator
uninstall-local:
	test -h "$(DESTDIR)$(bindir)/mate-calculator" \
	&& rm -f "$(DESTDIR)$(bindir)/mate-calculator"

EXTRA_DIST = \
	mp-enums.c.template \
	mp-enums.h.template

DISTCLEANFILES = \
	Makefile.in

test: mate-calc
	./mate-calc -u
