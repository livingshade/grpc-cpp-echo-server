
all: client server

client: client.cc
	g++ client.cc -pthread -O3 -o client

server: server.cc
	g++ server.cc -pthread -O3 -o server

clean: 
	rm -f client server