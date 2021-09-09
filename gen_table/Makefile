
CFLAGS += -Wall -Wextra -Wmissing-prototypes -Wredundant-decls \
          -Wshadow -mtune=native -O3

HEADERS = NTT_params.h tools.h gen_table.h
SRCs = tools.c gen_table.c

all: $(HEADERS) $(SRCs) test.c
	gcc $(CFLAGS) $(SRCs) test.c

.PHONY: clean
clean:
	rm -f *.h.gch
	rm -f a.out

