.PHONY: all
all: tests

# include mupq/mk/config.mk

ifndef _CONFIG
_CONFIG :=

SRCDIR := $(CURDIR)

##############################
# Include retained variables #
##############################

RETAINED_VARS :=

CONFIG := obj/.config.mk

-include $(CONFIG)

$(CONFIG):
	@echo "  GEN     $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	@echo "# These variables are retained and can't be changed without a clean" > $@
	@$(foreach var,$(RETAINED_VARS),echo "$(var) := $($(var))" >> $@; echo "LAST_$(var) := $($(var))" >> $@;)

###############
# Some Macros #
###############
objs = $(addprefix obj/,$(addsuffix .o,$(1)))
hashprofobjs = $(addprefix obj/hashprof/,$(addsuffix .o,$(1)))
hostobjs = $(addprefix obj-host/,$(addsuffix .o,$(1)))

Q ?= @

PLATFORM=nucleo-f207zg

# The platform file should set all necessary CC/CXX/AR/LD variables
include mk/$(PLATFORM).mk

RETAINED_VARS += PLATFORM

Q ?= @

HOST_CC := cc
HOST_AR := ar
HOST_LD := $(HOST_CC)

################
# Common Flags #
################

SYSROOT ?= $(shell $(CC) --print-sysroot)

CFLAGS += \
	--sysroot=$(SYSROOT) \
	-I$(SRCDIR)/common \

DEBUG ?=
OPT_SIZE ?=
LTO ?=
ITERATIONS ?= 1

RETAINED_VARS += DEBUG OPT_SIZE LTO AIO ITERATIONS

CFLAGS += -O3 -g3

ifeq ($(LTO),1)
CFLAGS += -flto
LDFLAGS += -flto
endif

CPPFLAGS += -DITERATIONS=$(ITERATIONS)

ifeq ($(STACK), 1)
CFLAGS += -DSTACK
endif

CFLAGS += \
	-Wall -Wextra -Wshadow \
	-MMD \
	-fno-common \
	-ffunction-sections \
	-fdata-sections \
	$(CPPFLAGS)

LDFLAGS += \
	-Lobj \
	-Wl,--gc-sections

LDLIBS += -lm

HOST_CFLAGS += \
	-O2 -g3 \
	-I$(SRCDIR)/common \
	-Wall -Wextra -Wshadow \
	-MMD \
	-fno-common \

HOST_LDFLAGS += \
	-Lobj-host

# Check if the retained variables have been changed
define VAR_CHECK
ifneq ($$(origin LAST_$(1)),undefined)
ifneq "$$($(1))" "$$(LAST_$(1))"
$$(info Variable $(1) changed, forcing rebuild!)
.PHONY: $(CONFIG)
endif
endif
endef

$(foreach VAR,$(RETAINED_VARS),$(eval $(call VAR_CHECK,$(VAR))))
endif

# include mk/crypto.mk

SYMCRYPTO_SRC = \
	common/fips202.c \
	common/keccakf1600.S

obj/libsymcrypto.a: $(call objs,$(SYMCRYPTO_SRC))

obj/libsymcrypto-hashprof.a: CPPFLAGS+=-DPROFILE_HASHING
obj/libsymcrypto-hashprof.a: $(call hashprofobjs,$(SYMCRYPTO_SRC))

LDLIBS += -lsymcrypto$(if $(PROFILE_HASHING),-hashprof)
LIBDEPS += obj/libsymcrypto.a obj/libsymcrypto-hashprof.a

# include mupq/mk/host-crypto.mk

HOST_SYMCRYPTO_SRC = \
	common/fips202.c \
	common/keccakf1600.c

obj-host/libsymcrypto.a: $(call hostobjs,$(HOST_SYMCRYPTO_SRC))

HOST_LDLIBS += -lsymcrypto
HOST_LIBDEPS += obj-host/libsymcrypto.a

# include mupq/mk/rules.mk

ELFNAME = $*

obj/_ELFNAME_%.o: $(CONFIG)
	@echo "  GEN     $@"
	$(Q)echo "const char _elf_name[] = \"$(ELFNAME)\";" | \
		$(CC) -x c -c -o $@ $(filter-out -g3,$(CFLAGS)) -

elf/%.elf: obj/_ELFNAME_%.elf.o $(LINKDEPS) $(CONFIG)
	@echo "  LD      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(LD) $(LDFLAGS) -o $@ $(filter %.o,$^) -Wl,--start-group $(LDLIBS) -Wl,--end-group

obj/%.a: $(CONFIG)
	@echo "  AR      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(AR) rcs $@ $(filter %.o,$^)

bin/%.bin: elf/%.elf
	@echo "  OBJCOPY $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(OBJCOPY) -Obinary $< $@

bin/%.hex: elf/%.elf
	@echo "  OBJCOPY $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(OBJCOPY) -Oihex $< $@

obj/%.c.o: %.c $(CONFIG)
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

obj/%.c.S: %.c $(CONFIG)
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -S -o $@ $(CFLAGS) $<

obj/%.S.o: %.S $(CONFIG)
	@echo "  AS      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

obj/%.s.o: %.s $(CONFIG)
	@echo "  AS      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

obj/hashprof/%.c.o: %.c $(CONFIG)
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

obj/hashprof/%.S.o: %.S $(CONFIG)
	@echo "  AS      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

obj/hashprof/%.s.o: %.s $(CONFIG)
	@echo "  AS      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) -c -o $@ $(CFLAGS) $<

bin-host/%: $(HOST_LIBDEPS) $(CONFIG)
	@echo "  HOST-LD $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(HOST_LD) $(HOST_LDFLAGS) -o $@ $(filter %.o,$^) $(HOST_LDLIBS)

obj-host/%.a: $(CONFIG)
	@echo "  HOST-AR $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(HOST_AR) rcs $@ $(filter %.o,$^)

obj-host/%.c.o: %.c $(CONFIG)
	@echo "  HOST-CC $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(HOST_CC) -c -o $@ $(HOST_CFLAGS) $<

obj-host/%.c.S: %.c $(CONFIG)
	@echo "  HOST-CC $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(HOST_CC) -S -o $@ $(HOST_CFLAGS) $<

obj-host/%.S.o: %.S $(CONFIG)
	@echo "  HOST-AS $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(HOST_CC) -c -o $@ $(HOST_CFLAGS) $<

# include mupq/mk/schemes.mk

IMPLEMENTATION_PATH = \
	crypto_kem/saber/m3speed \
	crypto_kem/saber/m3speedstack \
	crypto_kem/saber/m3stack \
	crypto_kem/saber/m3_32bit \
	crypto_kem/lightsaber/m3speed \
	crypto_kem/lightsaber/m3speedstack \
	crypto_kem/lightsaber/m3stack \
	crypto_kem/lightsaber/m3_32bit \
	crypto_kem/firesaber/m3speed \
	crypto_kem/firesaber/m3speedstack \
	crypto_kem/firesaber/m3stack \
	crypto_kem/firesaber/m3_32bit

# If, however, the IMPLEMENTAION_PATH is defined, only the path it points to
# will be added to the {KEM,SIGN}_SCHEMES lists. Since the python scripts for
# automatic test running will call make with this, remaking the list is skipped,
# saving a bit of time.
KEM_SCHEMES := $(if $(findstring crypto_kem,$(IMPLEMENTATION_PATH)),$(IMPLEMENTATION_PATH))

# These are small macros to be called with the $(call) mechanism of make
# Derives a name for a scheme from its path.
schemename = $(subst /,_,$(1))
# Derives the list of source files from a path.
schemesrc = $(wildcard $(1)/*.c) $(wildcard $(1)/*.s) $(wildcard $(1)/*.S)

# The default compilation rule.
define compiletest
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) $(filter-out --specs=%,$(CFLAGS)) $(LDFLAGS) -o $@ $(if $(AIO),$(filter %.c %.S %.s,$^),$<) -Wl,--start-group $(LDLIBS) -Wl,--end-group
endef

define hostcompiletest
	@echo "  HOST-LD $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(HOST_CC) $(filter-out --specs=%,$(HOST_CFLAGS)) $(HOST_LDFLAGS) -o $@ $(filter %.c,$^) $(HOST_LDLIBS)
endef

HOST_IMPLEMENTATIONS = %_ref %_opt %opt-ct

# This template defines all the targets for a scheme: a library file containing
# all the compiled objects, and an elf file for each test.
define schemelib
obj/lib$(2).a: $(call objs,$(call schemesrc,$(1)))
libs: obj/lib$(2).a
elf/$(2)_%.elf: CPPFLAGS+=-I$(1)

# The {test,stack,speed,...}.c file is compiled directly into the elf file,
# since the code depends on the preprocessor definitions in the api.h file of
# the scheme.

# Compile just the test and link against the library.
elf/$(2)_%.elf: LDLIBS+=-l$(2)
elf/$(2)_%.elf: crypto_$(3)/%.c obj/lib$(2).a $$(LINKDEPS) $$(CONFIG)
	$$(compiletest)

# Add the elf,bin and hex files to the tests target.
tests: elf/$(2)_test.elf elf/$(2)_speed.elf elf/$(2)_stack.elf elf/$(2)_f_speed.elf
tests-bin: bin/$(2)_test.bin bin/$(2)_speed.bin bin/$(2)_stack.bin bin/$(2)_f_speed.bin
tests-hex: bin/$(2)_test.hex bin/$(2)_speed.hex bin/$(2)_stack.hex bin/$(2)_f_speed.hex

endef

.PHONY: tests tests-bin tests-hex

# Now, for all schemes, the template above is evaluated.
$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call schemelib,$(scheme),$(call schemename,$(scheme)),kem)))

.PHONY: clean libclean

clean: libclean
	rm -rf elf/
	rm -rf bin/
	rm -rf bin-host/
	rm -rf obj/
	rm -rf testvectors/
	rm -rf benchmarks/

.SECONDARY:
