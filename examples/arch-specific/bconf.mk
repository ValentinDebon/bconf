hello-x86_64:=hello.o
ifneq ($(hello-$(host-arch)),)
hello: CPPFLAGS:=-nostdinc
hello: LDFLAGS:=-nostdlib
hello: $(hello-$(host-arch))
host-$(host-arch)+=$(hello-$(host-arch))
host-bin+=hello
endif
