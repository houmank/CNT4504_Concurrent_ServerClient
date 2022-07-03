client: application.o client.o timer.o
	g++ -O2 -pthread -std=c++17 application.o client.o timer.o -o client

application.o: application.cpp
	g++ -c -O2 -pthread -std=c++17 application.cpp

client.o: client.cpp
	g++ -c -O2 -std=c++17 client.cpp

timer.o: timer.cpp
	g++ -c -O2 -std=c++17 timer.cpp

clean:
	rm *.o client
