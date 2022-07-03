server: application.o server.o
	g++ -pthread application.o server.o -o server

application.o: application.cpp
	g++ -c -O2 -pthread -std=c++17 application.cpp

server.o: server.cpp
	g++ -c -O2 -pthread -std=c++17 server.cpp
	
clean:
	rm *.o server
