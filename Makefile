CFLAGS := -std=c99 -fPIC \
	-Wall -Wextra -pedantic \
	-Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
	-D_GNU_SOURCE \
	${CFLAGS}

all: search libnss-blacklist.so.0
search: search.o memory.o boyermoore.o
libnss_blacklist.so.0: nss.o search.o memory.o boyermoore.o
	$(LINK.o) -shared $^ $(LOADLIBES) $(LDLIBS) -o $@

clean:
	${RM} search *.o

.PHONY: clean
