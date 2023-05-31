CPPFLAGS=-D_GNU_SOURCE \
	-DCONFIG_DEFAULT_MAKEFILE_TEMPLATE=\"GNUmakefile.in\" \
	-DCONFIG_DEFAULT_OUTPUT_TEMPLATE=\"configure.in\" \
	-DCONFIG_DEFAULT_OUTPUT=\"configure\" \
	-DCONFIG_DEFAULT_BCONF=\"bconf\"

mkconf: y.tab.o lex.yy.o

mkconf.o: y.tab.h

y.tab.h: y.tab.c
y.tab.c: bconf.y
	$(YACC) $(YFLAGS) -d -- $<

lex.yy.c: bconf.l y.tab.h
	$(LEX) $(LFLAGS) -- $<
