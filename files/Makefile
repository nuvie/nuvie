CC=gcc

CFLAGS= -g -Wall

LDFLAGS= 

OBJS=dict.o\
     lzw.o\
     stack.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

all: u6decomp

u6decomp: $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@ 

clean:
	rm -f $(OBJS) u6decomp 
