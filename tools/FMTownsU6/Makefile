CC=g++

CFLAGS= -g -Wall -I ../.. -I ../../files -I ../../conf -I ../../misc

LDFLAGS= 

# Makefile.common defines unpack_OBJS
include Makefile.common

OBJS=$(unpack_OBJS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: unpack 

unpack: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 

clean: 
	rm -f $(OBJS) unpack 

reallyclean: clean

