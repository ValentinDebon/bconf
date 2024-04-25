CPPFLAGS=-D_GNU_SOURCE \
	-DCONFIG_DEFAULT_TEMPLATE_SET=\"host-only\" \
	-DCONFIG_DEFAULT_OUTPUT=\"configure\" \
	-DCONFIG_DEFAULT_BCONF=\"bconf\" \
	-DCONFIG_DATADIR=\"data\" \

mkconf: y.tab.o lex.yy.o

mkconf.o: y.tab.h

y.tab.h: y.tab.c
y.tab.c: bconf.y
	$(YACC) $(YFLAGS) -d -- $<

lex.yy.c: bconf.l y.tab.h
	$(LEX) $(LFLAGS) -- $<
