# Example

In this chapter, we will create an example project and showcase
how the different roles introduced in the previous chapter come in.

## Hello World

First, create a source code with our desired hello world program, `hello.c`:
```C
#include <stdio.h>

int
main(void) {

	puts("Hello, world!");

	return 0;
}
```

Next, we need to define the targets, in `bconf.mk`:
```Makefile
hello: hello.o

host-bin+=hello
clean-up+=$(host-bin) hello.o
```

Now, create the `configure` script:
In a shell, in the same directory as our `bconf.mk` and `hello.c` files:
```sh
mkconf
```

Finally, you can configure, compile and run the example:
```sh
./configure
make
./hello
```
This last step can be performed outside of the source code directory tree!

## Configuration options

Now, let's introduce the reason why bconf exists: build configuration.

In the source tree, create a new file called `bconf`:
```
config GREETING
	"String to print for the hello program"
	defaults "Hello, world!"
```

Modify the source code `hello.c`:
```C
#include <stdio.h>

int
main(void) {

	puts(CONFIG_GREETING);

	return 0;
}
```

And modify the `bconf.mk`:
```Makefile
hello: hello.o

hello.o: CPPFLAGS+=-DCONFIG_GREETING='"$(CONFIG_GREETING)"'

host-bin+=hello
clean-up+=$(host-bin) hello.o
```

Don't forget to re-generate the `configure` script:
```sh
mkconf
```

In your build directory, don't forget to clean the previous artifacts,
and run `configure` once again to generate an up-to-date `GNUmakefile`:
```sh
make clean
./configure --with-greeting="Hello, ${LOGNAME}\!"
make
./hello
```

You should now have a custom hello message due to our build configuration.
