CC=g++

CFLAGS= -g -Wall -I ../../files -I ../../conf -I ../../misc

LDFLAGS= 

OBJS=	../../files/NuvieIO.o\
	../../files/NuvieIOFile.o\
	../../files/U6Lzw.o\
	../../files/U6Lib_n.o\
	unpack.o

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

all: unpack 

unpack: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 

clean: 
	rm -f $(OBJS) unpack 

reallyclean: clean

