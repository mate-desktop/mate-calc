@YELP_HELP_RULES@
HELP_ID = mate-calc
HELP_FILES = absolute.page \
             base.page \
             boolean.page \
             complex.page \
             conv-base.page \
             conv-character.page \
             conv-currency.page \
             conv-length.page \
             conv-time.page \
             conv-weight.page \
             equation.page \
             factorial.page \
             factorize.page \
             financial.page \
             functions.page \
             index.page \
             keyboard.page \
             legal.xml \
             logarithm.page \
             modulus.page \
             mouse.page \
             number-display.page \
             percentage.page \
             power.page \
             scientific.page \
             superscript.page \
             trigonometry.page \
             variables.page

# Add linguas to be ignored, e.g. IGNORE_HELP_LINGUAS = ca de es fr
IGNORE_HELP_LINGUAS =
HELP_LINGUAS = $(if $(IGNORE_HELP_LINGUAS), \
	$(filter-out $(IGNORE_HELP_LINGUAS),$(subst /,,$(dir $(wildcard */*.po)))), \
	$(subst /,,$(dir $(wildcard */*.po))) )

DISTCLEANFILES = \
	Makefile.in

-include $(top_srcdir)/git.mk
