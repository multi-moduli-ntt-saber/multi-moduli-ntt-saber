.PHONY: all
all: tests tests-bin

include mk/config.mk
include mk/crypto.mk
include mk/rules.mk
include mk/schemes.mk

.PHONY: clean libclean

clean: libclean
	rm -rf elf/
	rm -rf bin/
	rm -rf obj/
	rm -rf benchmarks/

.SECONDARY:
