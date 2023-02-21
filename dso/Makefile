
all: client server add

client: client.cc
	g++ client.cc -pthread -ldl -O3 -o client

server: server.cc interceptor.hh
	g++ server.cc -pthread -ldl -O3 -o server

add: add.cc interceptor.hh
	g++ -c -fPIC -o add.o add.cc 
	gcc -shared -o libadd.so  add.o

clean: 
	rm -f client server libadd.so