CFLAGS = -O2 -Wall -pthread
OBJS = slave.o reader.o writer.o bus.o libser.o crc.o master.o
CC = g++

.c.o: 
	$(CC) $(FLAGS) -c $<

all: slave master

slave: $(OBJS)
	$(CC) $(CFLAGS) reader.o writer.o crc.o bus.o slave.o libser.o -o slave	
	
master: $(OBJS)
	$(CC) $(CFLAGS) reader.o writer.o bus.o master.o libser.o crc.o -o master

clean:
	rm -f *.o
