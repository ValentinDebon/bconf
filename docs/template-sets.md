# Template Sets

As explained in the [introduction](introduction.md), the `mkconf` command
might require extra arguments depending on the context and the project.

The `mkconf` generates one artifact, the `configure` script, a POSIX-shell script.
Yet indirectly generates two artifacts as the `configure` script in turns generate
the `GNUmakefile` which will be used when building the programs/artifacts.

A **template set** is a named pair of templates for both the `configure` and `GNUmakefile` files.
This document won't detail the internals of said templates nor how to write them, but the
features provided by the template sets provided by the default bconf distribution.

**NOTE: bconf is still in an experimental stage.
While basic features and functionalities work, syntax
and features are subject to future changes and bug fixes.**

## configure

The `configure` scripts are close to what you would find everywhere else,
they take arguments of the form `--${name}=${value}`, and if an argument
has the form `${name}=${value}`, evaluates it (useful for override, eg. `CC=gcc`).

While in the script, for a config entry, its name is lowercased and `_`s are changed into `-`s.
Three extra arguments are added to the `configure` script, where `${config}` is the transformed config name:
- `--enable-${config}`: The variable associated to `${config}` is set to `1`.
- `--with-${config}=${value}`: The variable associated to `${config}` is set to `${value}`.
- `--without-${config}`, `--disable-${config}`: The variable associated to `${config}` is unset.

The following variables are forwarded to the `GNUmakefile`:
- `AR`: Archiver for static libraries, if `host` is specified, default to `${host}-ar`, else to `ar`.
- `CC`: C compiler, if `host` is specified, default to `${host}-gcc`, else to `cc`.
- `CPP`: C preprocessor, if `host` is specified, default to `${host}-cpp`, else to `cpp`.
- `MKDIR`: Create directories of targets, defaults to `mkdir -p`.
- `INSTALL`: Install to use during the installations, defaults to `install`.
- `ARFLAGS`: Flags given to `AR`, defaults to `-rc`.
- `ASFLAGS`: Flags given to `CC` when compiling an assembler object file, default is empty.
- `CFLAGS`: Flags given to `CC` when compiling a C object file, defaults to `-fPIC -O2`.
- `CPPFLAGS`: Flags given to `CC` when compiling a C or assembler object file, default is empty.
- `LDFLAGS`: Flags given to `CC` when linking object files, default is empty.
- `LDLIBS`: Flags given to `CC` when linking object files, default is empty.

## GNUmakefile

The `GNUmakefile` exposes the `bconf` entries in the form `CONFIG_$(name)`
where `$(name)` is the name of the config entry as given in the `bconf` file.
All GNU-make default suffixes rules are removed and replaced by **bconf**'s rules
to avoid any conflicts. For more details, either see the associated
template set's informations or directly the generated `GNUmakefile` source.

The `GNUmakefile` heavily relies on the `VPATH` feature of GNU-make
to transparently serve out-of-source builds, it is recommended to follow
this guide's recommandations to avoid tricky issues and best serve this use-case.

The package's name and version given during invocation of `mkconf` are
available under the `package-name` and `package-version` variables respectively.

The absolute path to the `configure` script's directory is available in `srcdir`.
The absolute path to the `GNUmakefile` makefile's directory is available in `objdir`.

The target triple of `CC` is available in the `host` variable, while the
architecture of said triple should be extracted in the `host-arch` variable.

Following the GNU guidelines, default installation directories are:
- `prefix`: Defaults to `/usr/local`.
- `exec_prefix`: Defaults to `$(prefix)`.
- `bindir`: Defaults to `$(exec_prefix)/bin`.
- `sbindir`: Defaults to `$(exec_prefix)/sbin`.
- `libexecdir`: Defaults to `$(exec_prefix)/libexec`.
- `libdir`: Defaults to `$(exec_prefix)/lib`.
- `datarootdir`: Defaults to `$(prefix)/share`.
- `datadir`: Defaults to `$(datarootdir)/$(package-name)`.
- `sysconfdir`: Defaults to `$(prefix)/etc`.
- `sharedstatedir`: Defaults to `$(prefix)/com`.
- `localstatedir`: Defaults to `$(prefix)/var`.
- `runstatedir`: Defaults to `$(localstatedir)/run`.
- `includedir`: Defaults to `$(prefix)/include`.
- `docdir`: Defaults to `$(datarootdir)/doc/$(package-name)`.
- `infodir`: Defaults to `$(datarootdir)/info`.
- `mandir`: Defaults to `$(datarootdir)/man`.
- `htmldir`: Defaults to `$(docdir)/$(package-name)`.
- `pdfdir`: Defaults to `$(docdir)/$(package-name)`.

The `GNUmakefile` always present the following PHONY rules:
- `all`: Run `host-bin`, `host-sbin`, `host-libexec`, `host-lib`.
- `clean`: `$(RM)` everything in the `clean-up` variable.
- `install-data`: Does nothing out-of-the-box. Should be augmented for docs, manpages, etc...
- `install-devel`: Installs static libraries. Should be augmented for headers, pkg-configs, etc...
- `install-exec`: Installs binaries from `host-bin`, `host-sbin`, `host-libexec` and shared libraries in their associated installation directories.
- `install-exec-strip`: Same as `install-exec` except `INSTALL` is augmented with the `-s` option.
- `install`: Performs `install-data`, `install-devel` and `install-exec`.
- `install-strip`: Performs `install-data`, `install-devel` and `install-exec-strip`.
