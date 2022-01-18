
SYMCRYPTO_SRC = \
	common/fips202.c \
	common/keccakf1600.S

obj/libsymcrypto.a: $(call objs,$(SYMCRYPTO_SRC))

LDLIBS += -lsymcrypto
LIBDEPS += obj/libsymcrypto.a
