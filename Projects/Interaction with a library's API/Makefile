CFLAGS = -Wall -g -std=c++11
SOURCES = client.cpp helpers.cpp buffer.cpp requests.cpp warnings.cpp user_interaction.cpp response_parsers.cpp queries.cpp

build:
	g++  $(CFLAGS) $(SOURCES) -o client

run_client:
	./client

clean:
	rm -f client
