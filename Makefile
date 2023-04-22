PROG = lab07
OBJS = lab07.o
TMP = $(PROG) $(OBJS)

%.o: %.c
	gcc -c -g -o $@ $<

$(PROG): $(OBJS)
	gcc -g -o $@ $^

clean:
	rm -rf $(TMP)
