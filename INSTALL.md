# Installing bconf

## Bootstrap (from SCM only)

The repository comes with a bootstrap Makefile.
It should only be required for development, as a
configure script should be embedded with source releases.
It can be easily tweak if required so.
```sh
make # Create the bootstrap 'mkconf' in the current directory if required
./mkconf # Create the configuration script
```
Note that if you configure the build and try to build, the `mkconf`
would be considered up-to-date as all its dependencies are too.
To really take into consideration the newly created configuration:
```sh
./configure
make clean
make
```

## Configuration and build

Once a configuration script available, you can do the usual:
```sh
./configure
# Generated Makefile is a GNUmakefile, use gmake if on *BSDs
make
make install
```

See README for more informations on bconf and bconf templates' capabilites and limitations.
