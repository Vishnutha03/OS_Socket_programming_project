server:
	gcc server.c -lpthread -o server
 
client:
	gcc client.c -o client

clean:
	rm client server
