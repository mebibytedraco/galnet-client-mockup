OBJS = \
	  main.o \

LDLIBS = -lncursesw
CFLAGS = -g
LDFLAGS = -g

all: client-mockup

client-mockup: main.o
	cc $(LDFLAGS) -o client-mockup $(OBJS) $(LDLIBS)

%.o: %.c
	cc -c $(CFLAGS) $< -o $@

main.o: main.c

clean:
	rm -rf $(OBJS) client-mockup
