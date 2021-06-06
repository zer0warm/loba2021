CFLAGS := -Wall -Wextra
CPPFLAGS := -std=c11

all: server test

server: main.c util.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o LB

test: client auth storage

client: test/client.c util.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o test/client

auth: test/auth.c util.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o test/auth

storage: test/storage.c util.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o test/storage
