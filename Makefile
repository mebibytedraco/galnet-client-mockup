OBJS = \
	  main.o \

LDLIBS = -lncursesw

all: client-mockup

client-mockup: main.o
	cc $(LDFLAGS) -o client-mockup $(OBJS) $(LDLIBS)

%.o: %.c
	cc -c $(CFLAGS) $< -o $@

main.o: main.c

clean:
	rm -rf $(OBJS) client-mockup
