
all: client server add

client: client.cc
	g++ client.cc -pthread -ldl -O3 -o client

server: server.cc
	g++ server.cc -pthread -ldl -O3 -o server

add: add.c
	gcc -shared -o libadd.so -fPIC add.c

clean: 
	rm -f client server libadd.so