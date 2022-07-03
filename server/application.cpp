#include <iostream>
#include <limits>

#include "server.hpp"

/**
 * getPortNumber is a function that asks the user for a port number and returns it as an int.
 * The function does not accept any arguments and returns int.
 * @param void
 * @return int
 */
int getPortNumber();

int main()
{
    // Get the port number from the user
    int portNumber = getPortNumber();

    // Create a server socket and listen for connections
    Server server(portNumber);
    server.AcceptCons();

    // Clean up
    server.ShutDown();

    return 0;
}

int getPortNumber()
{
    int portNum = 0;

    // Prompt the user to input the port number
    std::cout << "Please enter which port to use for the socket [1024 - 4996]: ";
    std::cin >> portNum;

    // Error check invalid port range
    while(portNum < 1024 || portNum > 4996)
    {
        std::cout << "[ERROR]: Port number is out of range.\n"
                  << "Please enter a valid port number [1024 - 4996]: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> portNum;
    }

    return portNum;
}