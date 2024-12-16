mkconf-objs:=mkconf.o y.tab.o lex.yy.o

$(mkconf-objs): CPPFLAGS+=-D_GNU_SOURCE -I$(objdir) -I$(srcdir)

mkconf.o: CPPFLAGS+= \
	-DCONFIG_DEFAULT_TEMPLATE_SET='"$(CONFIG_DEFAULT_TEMPLATE_SET)"' \
	-DCONFIG_DEFAULT_OUTPUT='"$(CONFIG_DEFAULT_OUTPUT)"' \
	-DCONFIG_DEFAULT_BCONF='"$(CONFIG_DEFAULT_BCONF)"' \
	-DCONFIG_DATADIR='"$(datadir)"'
mkconf.o: y.tab.h

y.tab.h: y.tab.c
y.tab.c: bconf.y
	$(v-e) YACC $@
	$(v-a) $(YACC) $(YFLAGS) -d -- $<

lex.yy.c: bconf.l y.tab.h
	$(v-e) LEX $@
	$(v-a) $(LEX) $(LFLAGS) -- $<

mkconf: $(mkconf-objs)

host-bin+=mkconf
clean-up+=$(host-bin) $(mkconf-objs) y.tab.c y.tab.h lex.yy.c

template-sets:=host-only build-aware

host-data:=$(patsubst %,data/%/configure.in,$(template-sets)) \
	$(patsubst %,data/%/GNUmakefile.in,$(template-sets))

define generate-install-data-rule
	$(v-a) $(INSTALL-DATA) -- $(filter %/$(1)/configure.in %/$(1)/GNUmakefile.in,$(2)) $(DESTDIR)$(datadir)/$(1)

endef

install-data: $(host-data)
	$(v-e) INSTALL $(host-data)
	$(v-a) $(INSTALL) -d -- $(patsubst %,$(DESTDIR)$(datadir)/%,$(template-sets))
	$(foreach d,$(template-sets),$(call generate-install-data-rule,$(d),$^))

uninstall-data:
	$(v-e) UNINSTALL $(host-data)
	$(v-a) $(RM) -- \
		 $(patsubst %,$(DESTDIR)$(datadir)/%/configure.in,$(template-sets)) \
		 $(patsubst %,$(DESTDIR)$(datadir)/%/GNUmakefile.in,$(template-sets))

ifneq ($(shell command -v mdbook),)
.PHONY: book

book: book.toml
	$(v-e) MDBOOK
	$(v-a) mdbook build $(<D)
endif
