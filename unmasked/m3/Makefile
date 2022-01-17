.PHONY: all
all: tests

include mk/config.mk
include mk/crypto.mk
include mk/rules.mk
include mk/schemes.mk

.PHONY: tests

# Now, for all schemes, the template above is evaluated.
$(foreach scheme,$(KEM_SCHEMES), \
	$(eval $(call schemelib,$(scheme),$(call schemename,$(scheme)),kem)))

.PHONY: clean libclean

clean: libclean
	rm -rf elf/
	rm -rf bin/
	rm -rf obj/
	rm -rf testvectors/
	rm -rf benchmarks/

.SECONDARY:
