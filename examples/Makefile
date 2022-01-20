LDSCRIPT   = stm32f4discovery.ld
LIBNAME    = opencm3_stm32f4
ARCH_FLAGS = -mthumb -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
DEFINES    = -DSTM32F4 -DSTM32F407VG
OBJS       = hal-opencm3.o

HEADER = tools.h naive_mult.h NTT_params.h gen_table.h ntt_c.h NTT.h SABER_params.h
SRC = tools.c naive_mult.c gen_table.c ntt_c.c
ASM_I = macros.i CT_butterflies.i schoolbooks.i
ASM = NTT.S iNTT.S auxiliary.S mul.S CRTm4.S

all: test.bin

test.elf: $(HEADER) $(SRC) $(ASM_I) $(ASM) test.o $(OBJS) $(LDSCRIPT)
		$(LD) -o $@ test.o $(SRC) $(ASM) $(OBJS) $(LDFLAGS) -l$(LIBNAME)

include make.mk