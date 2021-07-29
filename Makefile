client: client.cpp requests.cpp helpers.cpp buffer.cpp
	g++ client.cpp requests.cpp helpers.cpp buffer.cpp -o client

run: client
	./client

clean:
	rm -f *.o client
