
IMPLEMENTATION_PATH = \
	crypto_kem/lightsaber/m4fspeed \
	crypto_kem/lightsaber/m4fstack \
	crypto_kem/saber/m4fspeed \
	crypto_kem/saber/m4fstack \
	crypto_kem/firesaber/m4fspeed \
	crypto_kem/firesaber/m4fstack

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
# Derives a namespace for the implementation (pqclean uses namespaced function
# names) from an implementation name.
namespace = $(shell echo $(if $(filter mupq_pqclean_%,$(1)),$(subst mupq_pqclean_crypto_$(2)_,pqclean_,$(1))_) | tr '[:lower:]' '[:upper:]' | tr -d '-')

# The default compilation rule.
define compiletest
	@echo "  CC      $@"
	$(Q)[ -d $(@D) ] || mkdir -p $(@D)
	$(Q)$(CC) $(filter-out --specs=%,$(CFLAGS)) $(LDFLAGS) -o $@ $(if $(AIO),$(filter %.c %.S %.s,$^),$<) -Wl,--start-group $(LDLIBS) -Wl,--end-group
endef

.SECONDEXPANSION:

# This template defines all the targets for a scheme: a library file containing
# all the compiled objects, and an elf file for each test.
define schemelib
obj/lib$(2).a: $(call objs,$(call schemesrc,$(1)))
libs: obj/lib$(2).a
elf/$(2)_%.elf: CPPFLAGS+=-I$(1)

# The {test,stack,speed,...}.c file is compiled directly into the elf file,
# since the code depends on the preprocessor definitions in the api.h file of
# the scheme.

ifeq ($(AIO),1)
# Compile all sources in one.
elf/$(2)_%.elf: crypto_$(3)/%.c $$(LINKDEPS) $(call schemesrc,$(1)) $$(CONFIG)
	$$(compiletest)
# Library target doesn't inherit these flags in AIO mode
obj/lib$(2).a: CPPFLAGS+=-I$(1)
obj/lib$(2).a: MUPQ_NAMESPACE=$(call namespace,$(2),$(3))
else
# Compile just the test and link against the library.
elf/$(2)_%.elf: LDLIBS+=-l$(2)
elf/$(2)_%.elf: crypto_$(3)/%.c obj/lib$(2).a $$(LINKDEPS) $$(CONFIG)
	$$(compiletest)
endif

# Add the elf,bin and hex files to the tests target.
tests: elf/$(2)_test.elf elf/$(2)_speed.elf elf/$(2)_stack.elf elf/$(2)_f_speed.elf
tests-bin: bin/$(2)_test.bin bin/$(2)_speed.bin bin/$(2)_stack.bin bin/$(2)_f_speed.bin
tests-hex: bin/$(2)_test.hex bin/$(2)_speed.hex bin/$(2)_stack.hex bin/$(2)_f_speed.hex


endef

# Now, for all schemes, the template above is evaluated.
$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call schemelib,$(scheme),$(call schemename,$(scheme)),kem)))

.PHONY: tests tests-bin tests-hex






