#ifndef CLIENT_H
#define CLIENT_H

#include <unistd.h> // read()
#include <errno.h> // errno code
#include <stdlib.h> // exit() - maybe
#include <sys/socket.h> // socket(), connect()
#include <sys/types.h> // recommended by man
#include <netinet/in.h> // struct sockaddr_in
#include <string.h> // memset(), memcpy()
#include <netdb.h> // hostent

#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <thread>

#include "timer.hpp"
#include "asyncstream.h"

/**
 * The serverInfo struct is a struct that contains  different variables used in
 * the identification and creation of the client. Specifically the information necessary to conn
 * the socket to the server.
 * 
 * It also contains user information, the service the user has requested from the server.
 */
struct serverInfo {

    std::string serverAddress;
    int portNumber;
    int userSelection;
};

/**
 * The CHK_ERR macro is used to use preprocessor to write the socket error checking code by 
 * wrapping the first argument up in an if and second argument to output error message.
 * The macro accepts two arguments, the variable and a string.
 * 
 * @param var
 * @param str
 * @return void
 */
#define CHK_ERR(var, str) if(var < 0){\
    std::cerr << "ERROR #" << errno << ": " << str <<" failed.\n" << __FILE__ << ", Line: " <<  __LINE__ << std::endl;\
    exit(0);\
}\

/**
 * The Client class is the abstraction around the code needed to create a socket which will connect to the server
 * address and send and recv information. It contains its own data and needs a serverInfo struct to be constructed.
 */
class Client 
{
public:
    // Public constructor and destructor
    /**
     * The Client constructor accepts a serverInfo structure that contains the necessary data to
     * create a socket and Open a connection to the server. The moment the object gets created is the moment
     * the client attempts to open a connection. Once its open a connection it will time the send and recv 
     * turn around time.
     * 
     * @param servInfo
     */
    Client(const serverInfo& servInfo);

    /**
     * The Client destructor simply frees allocated memory.
     * 
     * @param void
     */
    ~Client();

    /**
     * The GetTimer public member function that accepts zero arguments and returns a Timer object which holds the 
     * duration of the Turn-around time of the client requesting data from the server.
     *
     * @param void
     * @return Timer
     */
    Timer GetTimer();

private:
    // private methods

    /**
     * OpenConnection is a private member function that accepts zero arguments and returns nothing.
     * The function will try to access databases to resolve the proper sockaddr and then attempt to 
     * connect to the server.
     * 
     * @param void
     * @return void
     * @see connect() in socket.h
     */
    void OpenConnection();

    /**
     * SendAndRecv is a private member function that will hold the code necessary to send and recieve data
     * from the open connection. The function accepts zero arguments and returns nothing.
     * 
     * @param void
     * @return void
     * @see OpenConnection
     */
    void SendAndRecv();


private:
    // Private member variables
    serverInfo m_ServerInfo;
    Timer m_Timer;

    int m_ServerID;
    int m_NumBytes;
    sockaddr_in m_ServerAddress;
    std::array<char, 1024 * 24> m_MsgBuffer;
};

#endif //CLIENT_H
