
CFLAGS += -Wall -Wextra -Wmissing-prototypes -Wredundant-decls \
          -Wshadow -mtune=native -O3

all: test.c
	gcc $(CFLAGS) NTT_params.h tools.h tools.c ntt_c.h ntt_c.c gen_table.h gen_table.c test.c

.PHONY: clean
clean:
	rm -f *.h.gch
	rm -f a.out

