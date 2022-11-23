# bconf

Build configurations, made easier.
GNU autotools are very powerful but extremely difficult to master
due to decades of enforced support for retrocompatibility.

The **bconf** utilities is a set of tools which purpose is to make
_source release of a software version_ easy to create and distribute.
In short, it provides the following functionalities:
- Makefile-easy and flexible build-system during development.
- Overridable set of rules and recipes for C software and libraries.
- Sane default set of rules and recipes for 21st century C software and libraries.
- Source release archive with a _standard_ build method: The infamous `./configure && make install`.
- Mimic the GNU `configure` script behaviour, not exactly, but enough not to traumatize package maintainers.
- Enhance components installation selection. eg: if a package maintainer only wants headers, or binaries.
- Support builds on the build machine.
- Support out of source tree builds.
- Support cross compilation.

## Developer

The software developer must describe:
- Products dependencies.
- Products recipes-specific details.
- Products installation directories.

## Code owner

The code owner, when releasing a source archive, must:
- Specify the package name, version and description.
- Generate the `configure` script for package maintainers.
- **TODO** Generate documentations in most trivial formats, such as groff-manpages, html or pdf.

## Package maintainer

The package maintainer, when building for the host system, must be able to:
- Extract the archive, patch, configure, build and install sources without installing non-POSIX utility.
- Change, enable or disable package features.
- Override tools used during builds.
- Install only a required set of products.
