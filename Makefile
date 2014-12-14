CFLAGS := -std=c99 \
	-Wall -Wextra -pedantic \
	-Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
	-D_GNU_SOURCE \
	${CFLAGS}

all: search
search: search.o boyermoore.o

clean:
	${RM} search *.o

.PHONY: clean
