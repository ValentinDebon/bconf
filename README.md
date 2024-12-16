# bconf

Build configurations, easier.
**bconf** is a tool which purpose is to make
_release of a software sources_ easy to create and distribute.

It generates a `configure` shell script,
which in turn creates a `GNUmakefile` for GNU Make.
The generated configure script should be POSIX-compliant
while the configure's generated Makefile targets GNU Make,
which is available on most UNIX-like systems.

It aims to provide the following functionalities:
- Makefile-based and flexible build-system during development.
- Overridable set of rules and recipes for C software and libraries.
- Sane default set of rules and recipes for 21st century C software and libraries.
- Source release archive with a _standard_ build method: The infamous `./configure && make install`.
- Mimic the GNU `configure` script behaviour, not exactly, but enough not to traumatize package maintainers.
- Enhance components installation selection. eg: if a package maintainer only wants headers, or binaries.
- Support builds on the build machine.
- Support out of source tree builds.
- Support cross compilation.

## Why?

bconf is a small project born from the frustration of other alternatives.

As of the time of writing (2024), the C build ecosystem is still a gigantic mess.
The current serious and portable alternatives are autotools, meson or cmake.

GNU autotools are very powerful but extremely difficult to master.
Learning resources are scarce. And even if you finally tame the beast, you're now
a part of a small group of programmers who are able to maintain these scripts/projects.
Its main benefit is that generated artifacts are extremely portable,
and thus building a source code release is always really simple.

CMake is a ninja or Makefile generator frontend. CMake is extremely
configurable. Just like autotools, mastering it requires arcanes behind
most humans patience. However, its main issue is lacking the ability to
easily build a program on the build machine when cross compiling.

Meson is a ninja generator frontend. I won't go into details, but the benefits
of ninja are only seen on huge projects when you have a stupidly powerful build machine.
Meson features are as restrictive as its syntax. For simple workflows, where you just
build C files, it is ok, great even, as it is one of the only alternative to support
cross-compilation transparently. My first issue with meson is that when you stray from
the path, creating a custom rule and having no functions in the language to generalize
and factorize the build infrastructure quickly becomes a pain. My second issue is its
run-time dependency on python. Meson versions need specific versions of python on
the build machine to work, and we all know how managing python versions goes...

So basically, I desired the flexibility of *Makefile*s with the portability
of autotools-generated artifacts. And that's exactly what bconf is,
a configuration script with a small GNU Make infrastructure.
A small and simple autotools.

Now, bconf is not a silver bullet. It is a way better and simpler
alternative for small projects and sources you want to distribute.
It's not meant to have optimized *Makefile* rules, and if the
build time becomes a problem, you may want to look at alternatives.
The goal of bconf is simplicity to use during development,
source code releases, and source code configuration/build/install.

## Compiling

bconf being built with bconf, if you build from SCM, you must first
bootstrap it from the source directory, and generate the configure script.
You'll need, lex, yacc and a working C compiler to do so:
```sh
make
./mkconf
```

Then, configure the build and remove the bootstrap artifacts.
You can now build a clean version of bconf:
```sh
./configure
make clean
make
```

## Copying

bconf is released under a BSD-3-Clause license, see the
LICENSE file which should also have been redistributed with the sources.

configure.in and GNUmakefile.in templates are also redistributed under the previous license.
Exception made for bconf's **generated** configuration/GNUmakefile files, which are redistributed under the CC0 license.
