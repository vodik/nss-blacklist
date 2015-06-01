CFLAGS := -std=c99 -fPIC \
	-Wall -Wextra -pedantic \
	-Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
	-D_GNU_SOURCE \
	${CFLAGS}

MODULE = libnss_blacklist.so.2

all: search $(MODULE)
search: search.o memory.o boyermoore.o
$(MODULE): nss.o search.o memory.o boyermoore.o
	${CC} -fPIC -Wall -shared -o $(MODULE) -Wl,-soname,$(MODULE) $^

clean:
	${RM} search *.o

.PHONY: clean
