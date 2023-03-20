mkconf-objs:=mkconf.o y.tab.o lex.yy.o

$(mkconf-objs): CPPFLAGS+=-D_GNU_SOURCE -I$(objdir) -I$(srcdir)

mkconf.o: CPPFLAGS+= \
	-DCONFIG_DEFAULT_OUTPUT_TEMPLATE='"$(CONFIG_DEFAULT_OUTPUT_TEMPLATE)"' \
	-DCONFIG_DEFAULT_MAKEFILE_TEMPLATE='"$(CONFIG_DEFAULT_MAKEFILE_TEMPLATE)"' \
	-DCONFIG_DEFAULT_OUTPUT='"$(CONFIG_DEFAULT_OUTPUT)"' \
	-DCONFIG_DEFAULT_BCONF='"$(CONFIG_DEFAULT_BCONF)"'
mkconf.o: y.tab.h

y.tab.h: y.tab.c
y.tab.c: bconf.y
	$(v-e) YACC $@
	$(v-a) $(YACC) $(YFLAGS) -o y.tab.c -d $<

lex.yy.c: bconf.l y.tab.h
	$(v-e) LEX $@
	$(v-a) $(LEX) $(LFLAGS) -o $@ $<

mkconf: $(mkconf-objs)

host-bin+=mkconf
host-data+=configure.in Makefile.in
clean-up+=$(host-bin) $(mkconf-objs) y.tab.c y.tab.h lex.yy.c
