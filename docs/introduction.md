# Introduction

**bconf** is meant to be used by different kind of actors.
The main one is the *software developer*, who writes the rules
because well, he is the one writing the software.
The second role is the *code owner*, it designates the person
who creates the source code release, and uses bconf to generate
the configure script that will ship to the end-user.
The last role is the *package maintainer*, which is a wide term
for the person using the source code release created by the
previous *code owner*. The *package maintainer* uses bconf when
he builds the source code on his local machine
(or a more complex automated infrastructure).

## Software developer

The *software developer* is the role who will spend the most
time with bconf, because he is the one responsible of describing
how the source code is built, and what are the configuration options.
He will use **bconf** like a *code owner* in the sense that he will
use the `mkconf` tool to perform local builds and iterate.

This user guide is mainly aimed at software developers.

The *software developer* must:
- Describe the build configuration, using the `bconf` file.
- Describe the GNUmakefile targets and rules, using the `bconf.mk` file.

## Code owner

The *code owner* is the one person (or automated infrastructure)
who performs the operation of creating a source release tarball
from the [*SCM*](https://en.wikipedia.org/wiki/Version_control).
In a manner reminiscent of `autoreconf`, he must perform
the `mkconf` command to create the `configure` script
which will ship with the source release tarball.

The `mkconf` command cannot just be typed randomly.
The *code owner* must choose a **template set** compatible with the source code.
See the chapter dedicated to template sets for more informations.
He may also specify the name and the version of the package, which
will be forwarded to `bconf.mk` through the variable `$(package-name)` and `$(package-version)`.
Note the *software developer* must also perform the previous tasks for local builds,
even though the name is usually inferred from the source directory name, and the version is unused.

The *code owner* must:
- Specify the package name, version and description.
- Generate the `configure` script for package maintainers.

## Package maintainer

The *package maintainer* is the one person (or automated infrastructure)
who uses the source code release created by the *code owner*.
Basically, the *package maintainer* shouldn't have to install anything
apart from GNU make. He will interact with the `configure` script,
specifying options and relaying environment variables.
The `configure` script in turn generates the `GNUmakefile`
which will rely on the `bconf.mk` definitions and allow
simple usage of the command `make` to compile, test, install, etc...

The *package maintainer* must:
- Extract the archive, patch, configure, build and install sources without installing non-POSIX utility.
- Change, enable or disable package features when calling the `configure` script.
