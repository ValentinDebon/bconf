# Advanced guide

This section provides advanced examples of common operations,
and how to correctly perform them using **bconf** to avoid issues.

## Building executables

A binary executable is built in two steps:
- Compile the source code into object files.
- Link the object files into a binary executable.

The bconf rule to compile object file matches `%o: %.c`, so this rule is mostly used implicitly.
The link rule, on the other hands, matches all elements of the `host-bin` variable.
Thus, to declare a binary executable, define its binary executable in `host-bin`
and extend its dependencies to include all its object files:
```Makefile
hello: hello.o # hello depends on hello.o, which in turns depends on hello.c.

host-bin+=hello # Link as host binary executable.
```

## Building libraries

Like executables, libraries are built in two steps, compiling objects, and then linking them.
The compiled binary objects are the same kind of artifacts used in building executables.
The difference occurs during the link operation, where different rules are applied.

The link rule matches all elements in the `host-lib` variable, however, a different
rule is applied depending on whether the element matches `%.a` or matches `%.$(ld-so)`.
The `ld-so` is a variable defined in the `GNUmakefile` specifying the extension of shared
libraries (for now only `so`, even on *Darwin*-based systems). Overriding the value of
`ld-so` to `a` is an implicit way to deactivate shared libraries generation.

```Makefile
foo-libs:=libfoo.a # libfoo supports static linking.
ifneq ($(ld-so),a) # If dynamic linking supported, add shared libfoo.
foo-libs+=libfoo.$(ld-so)
endif

$(foo-libs): foo.o # For static libfoo, and maybe shared libfoo, define dependencies.

host-lib+=$(foo-libs) # Link all libfoo variations.
```

## Build flags, linking with a library

To specify build options, `bconf` relies on overrides
and extensions of variables for specific targets:
```Makefile
# Expanding on the previous example:

bar-objs:=bar.o baz.o # Define all object files for a target.

$(bar-objs): CPPFLAGS+=-I$(srcdir)/include # Add headers from the source directory.
$(bar-objs): CFLAGS+=-std=c11 # Override the C standard used by the compiler when building
# Note here we override for $(bar-objs) instead of bar, that's because
# `make bar.o` wouldn't propagate the overrides from the `bar` target.

ifneq ($(CONFIG_BAR_OPTION),)
bar.o: CPPFLAGS+=-DCONFIG_BAR_OPTION='$(CONFIG_BAR_OPTION)'
# Only bar.o will receive this option during compilation.
endif

bar: $(bar-objs) # bar target objects require for linking
bar: LDFLAGS+=-Wl,-rpath='$ORIGIN/../lib' # Passing an option to the linker through the compiler driver.
bar: LDLIBS+=libfoo.$(ld-so) # bar will link with either static or shared libfoo.

host-bin+=bar # Link as host binary executable.
```

## Cleaning artifacts

To avoid confusion between what can be cleaned and what cannot, `bconf` leaves
the PHONY `clean` target choice of artifacts entirely to the end user. All objects
specified in the `clean-up` variable will be removed on running the `clean` target.
```Makefile
# Expanding on the previous example:

clean-up+=$(bar-objs) $(host-bin) $(host-lib)
```

## Compiling assembler files

Assembler files are compiled from the rule matching
all `%.o: %.S` from the `host-$(host-arch)` variable:
```Makefile
# Pre-supposing that an x86_64 assembly file asm.S exists in either $(objdir) or $(srcdir):
host-x86_64+=asm.o
# Now if $(host-arch) is x86_64, asm.S will be compiled with the `CC` compiler driver.
```
The uppercase `.S` extension is linked to GCC (and other C compilers) interpreting
the file either as needing C preprocessing (with uppercase), or not (with a lowercase).
In the case of `bconf`, uppercase is chosen as the default as a non-preprocessed file
can be compiled with the preprocessor while the other way is not always valid.

## Custom rules

Just like any `Makefile`, you can write rules in the `bconf.mk`:
```Makefile
# Dependencies are resolved either from the current working directory,
# or from the $(srcdir) thanks to the $(VPATH) feature of GNU-make, always
# reference them by $^ or $< so GNU-make resolves them correctly.

.PHONY: install-mandir

# Install manpages
install-mandir: man/bar.1
	$(v-e) INSTALL $(notdir $^)
	$(v-a) $(INSTALL) -d -- "$(DESTDIR)$(mandir)"
	$(v-a) $(INSTALL-DATA) -- $^ "$(DESTDIR)$(mandir)"

# Extend the empty install-data PHONY target to install runtime documentation.
install-data: install-mandir
```
The `v-e` and `v-a` macros work differently depending on whether
the variable `V` is set to `1` or not, if `V` is equal to `1`,
`$(v-e)` prefixed directives are ignored and `$(v-a)` prefixed ones
are printed as usual. If not, `$(v-e)` echoes the following string,
and `$(v-a)` executes the command silently (verbose or fancy output).
