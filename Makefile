
CC=gcc

CFLAGS= -Wall -pthread

deps=seadp.h thread_safe_quene.h

all: sea_server sea_client clean

sea_server: sea_server.o seadp.o thread_safe_quene.o
	$(CC) $^ -o $@ $(CFLAGS)

sea_server.o: sea_server.c $(dsps)
	$(CC) -c $^ -o $@ $(CFLAGS)

sea_client: sea_client.o seadp.o
	$(CC) $^ -o $@ $(CFLAGS)

sea_client.o: sea_client.c
	$(CC) -c $^ -o $@ $(CFLAGS)

seadp.o: seadp.c $(dsps)
	$(CC) -c $^ -o $@ $(CFLAGS)

thread_safe_quene.o: thread_safe_quene.c $(dsps)
	$(CC) -c $^ -o $@ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o