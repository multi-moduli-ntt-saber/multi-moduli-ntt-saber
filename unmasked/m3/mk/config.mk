
ifndef _CONFIG

_CONFIG :=

SRCDIR := $(CURDIR)

PLATFORM=nucleo-f207zg

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

Q ?= @

# The platform file should set all necessary CC/CXX/AR/LD variables
include mk/$(PLATFORM).mk

RETAINED_VARS += PLATFORM

Q ?= @

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


