#include <iostream>
#include <string>
#include <limits>
#include <vector>
#include <future>
#include <thread>
#include <fstream>

#include "client.hpp"


// Function declaration

/**
 * The getUserInput function accepts an int as an argument and returns a serverInfo struct.
 * The function is responsible for asking the user for the necessary information to start the client(s)
 * and will error check the input before returning the data. It will return the number of client to create via the arguments
 * and will return a serverInfo struct with the server address, port number and selection.
 * 
 * @param int&
 * @return serverInfo
 */
serverInfo getUserInput(int& numClients);



int main()
{
    // Initialize variables
    int numberOfClients;
    double totalTime = 0;
    serverInfo server;
    std::vector<double> dataPoints;
    std::vector<std::future<double>> futures;

    // Ask user for input
    server = getUserInput(numberOfClients);

    // Reserve space for async
    futures.reserve(numberOfClients);

    // Spawn tasks
    sscout << "\nMain Thread ID: " << std::this_thread::get_id() << '\n';
    for (int i = 0; i < numberOfClients; i++)
    {
        futures.emplace_back(std::async(std::launch::async, [&server]() -> double
        {
            Client client(server);
            return client.GetTimer().GetDurationMicro().count() * 0.001;
        }));
    }
    sscout << "\n\nFinished launching threads.\n";

    // Wait for all tasks to finish and get total turn-around time
    for(int i = 0; i < futures.size(); i++)
    {
        dataPoints.push_back(futures[i].get());
        totalTime += dataPoints[i];
    }

    sscout << "\n------------------------------------------------------------------------------\n"
              << "The total turn-around time: " << totalTime << " ms\n"
              << "The average turn-around time: " << totalTime/numberOfClients << " ms\n" << std::endl;

    // Append data to file 
    std::ofstream fileOut;
    fileOut.open("data_output.txt", std::ofstream::out | std::ofstream::app);
    fileOut << "\nNew data group, " << numberOfClients << " Threads, " << server.userSelection << " Selection. Here are the data points in ms: \n";
    for(auto d : dataPoints)
    {
        fileOut << d << ", ";
    }
    fileOut << totalTime << ", " << totalTime/numberOfClients << "\n";
    fileOut.close();

    return 0;
}

// Function definitions
serverInfo getUserInput(int& numClients)
{
    serverInfo info;
    
    // Get server address
    std::cout << "Please input the server address: ";
    std::cin >> info.serverAddress;

    // Get port number
    std::cout << "Please enter the port which the server is listening on: ";
    std::cin >> info.portNumber;

    // Get user selection
    std::cout << "Here are the services that can be requested from the server: \n" 
              << "1.) Date and Time\n"
              << "2.) Uptime\n"
              << "3.) Memory Usage\n"
              << "4.) List of Network Connections\n"
              << "5.) Current Users\n"
              << "6.) Running Processes\n"
              << "Please input your selection: ";
    std::cin >> info.userSelection;
    
    // Error check the user selection
    while(info.userSelection < 1 || info.userSelection > 6 )
    {
        std::cout << "[ERROR]: Selection was not valid.\n"
                  << "Please enter a valid number[1 - 6]: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> info.userSelection;
    }

    // Get number of clients to connect to server
    std::cout << "Please enter number of clients to generate[1, 5, 10, 15, 20, 25, or 100]: ";
    std::cin >> numClients;

    // Error check number of clients to generate
    while(!( ( (numClients == 1 || numClients % 5 == 0) && (numClients > 0 && numClients < 26) ) || numClients == 100) )
    {
        std::cout << "[ERROR]: Number of clients not valid.\n"
                  << "Please enter a valid number [1, 5, 10, 15, 20, 25]: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin >> numClients;
    }

    return info;
}