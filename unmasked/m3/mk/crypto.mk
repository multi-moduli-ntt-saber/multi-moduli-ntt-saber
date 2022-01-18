SYMCRYPTO_SRC = \
	common/fips202.c \
	common/keccakf1600.S \


obj/libsymcrypto.a: $(call objs,$(SYMCRYPTO_SRC))

obj/libsymcrypto-hashprof.a: CPPFLAGS+=-DPROFILE_HASHING
obj/libsymcrypto-hashprof.a: $(call hashprofobjs,$(SYMCRYPTO_SRC))

ifeq ($(AIO),1)
LDLIBS +=
LIBDEPS += $(SYMCRYPTO_SRC)
CPPFLAGS+=$(if $(PROFILE_HASHING),-DPROFILE_HASHING)
else
LDLIBS += -lsymcrypto$(if $(PROFILE_HASHING),-hashprof)
LIBDEPS += obj/libsymcrypto$$(if $$(PROFILE_HASHING),-hashprof).a
endif
