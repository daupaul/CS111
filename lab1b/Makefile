lab1b: client.c server.c
	@gcc -o client -g client.c -lpthread -lmcrypt
	@gcc -o server -g server.c -lpthread -lmcrypt

client: client.c
	@gcc -o client -g client.c -lpthread -lmcrypt

server: server.c
	@gcc -o server -g server.c -lpthread -lmcrypt

clean:

	@rm -rf client server *.o 
	@echo "Directory cleaned."

dist:
	@tar -czf lab1b-504451468.tar.gz README client.c server.c Makefile my.key
