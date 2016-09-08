

CC = gcc

CFILES = $(notdir $(wildcard *.c))

OFILES = $(patsubst %.c,%.o,$(CFILES))


ALL:FTPC


FTPC:$(OFILES)
	$(CC) $(OFILES) -o ftpc

%.o:%.c
	$(CC) $< -c -o $@	

clean:
	rm $(OFILES) ftpc
