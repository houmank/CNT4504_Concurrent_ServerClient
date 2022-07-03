#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/socket.h> // socket(), listen()
#include <sys/types.h> // Recommended by man page
#include <netinet/in.h> // sockaddr_in
#include <stdlib.h> // exit()
#include <stdio.h> // fgets()
#include <unistd.h> // read()
#include <string.h> // memset(), memcpy()
#include <errno.h> // errno error code
#include <thread> // multithreading
#include <vector> // threadpool
#include <chrono> // timer
#include <mutex> // locks
#include <queue> // job queue
#include <functional> // function pointers
#include <condition_variable> // conditional vars for yielding threads

#include <iostream>
#include <array>

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

class Server 
{
public:
    // Constructor and destructor
    /**
     * The Server constructor is responsible for setting up the socket and listening to said socket.
     * The moment that the server object gets created and constructor is called is the when the server will attempt
     * to open socket, bind ip, listen on socket.
     * 
     * @param port
     */
    Server(int port);

    /**
     * The Server destructor is a default destructor and accepts no arguments.
     * 
     * @param void
     */
    ~Server() = default;

    // Public methods
    /**
     * The AcceptCons method will loop and accept incoming connections to the open port 
     * that the server is listening to. It will run indefinitely until closed manually.
     * The method accepts no arguments and returns nothing.
     *
     * @param void 
     * @return void
     */
    void AcceptCons();

    /**
     * AddJobs is a method that will add a job (new connection) to the job queue.
     * The method will accept a function pointer of type void(int) and will add an incoming
     * connection to the thread pool's job queue. The method returns nothing.
     * Code was inspired by the thread pool implementation from Anthony William's "C++ Concurreny in Action"
     * 
     * @param  std::function<void()> f
     * @return void
     */
    void AddJobs(std::function<void()> f);

    /**
     * GetJobs is the method that will loop the threads until they get assigned a job.
     * The thread pool's thread use this to check if there are jobs in the job queue.
     * The function accepts no arguments and returns nothing.
     * Code was inspired by the thread pool implementation from Anthony William's "C++ Concurreny in Action"
     * 
     * @param  void
     * @return void
     */
    void GetJobs();

    /**
     * The HandleConn method will be used when calling a new thread. It will handle a new connection
     * and call the necessary functions so that it functions as intended. The method will terminate the connection
     * and the thread once it has finished the job.
     *
     * @param  clientID  -  The file descriptor of the newly opened connection.
     * @return void
     */
    void HandleConn(int clientID);

    /**
     * The ShutDown method will close the open fds and handle any memory cleanup
     * The method accepts no arguments and returns nothing.
     * 
     * @param void
     * @return void
     */
    void ShutDown();

private:
    // Private member methods
    /**
     * The SelectCommand method will be responsible for determining the request and calling the appropriate
     * bash command. The function will accept a message buffer, and user's selection as arguments and returns nothing.
     * 
     * @param msgBuffer
     * @param userSelection
     * @return void
     */
    void SelectCommand(std::array<char, 1024 * 32>& msgBuffer, int userSelection);

    /**
     * The HandleCommand method will check to see if the given command is already cached,
     * if not it will get the correct output and push it to the cache before returning.
     * The method accepts two arguments, a message buffer and a command. The method
     * return nothing.
     * 
     * @param  msgBuffer
     * @param  command
     * @return void
     */
    void HandleCommand(std::array<char, 1024 * 32>& msgBuffer, std::string command);

    /**
     * The GetCommandOutput method will be calling the popen function to invoke a terminal command and get
     * the output back. It accepts a message buffer as an argument and returns nothing. It will return the ouput via
     * the referenced message buffer.
     * 
     * @param msgBuffer
     * @return void
     */
    void GetCommandOutput(std::array<char, 1024 * 32>& msgBuffer);


private:
    // Private member variables //

    // Socket and msgBuffer
    int m_PortNumber;

    int m_ServerID, m_NumBytes;
    socklen_t m_ClientAddrLength;
    sockaddr_in m_ServerAddress, m_ClientAddress;
    std::array<char, 1024 * 32> m_MsgBuffer;

    // thread locks and cache timings
    int m_LastSelection = 0;
    std::mutex m_MsgLock;
    std::mutex m_TimePointLock;
    std::chrono::time_point<std::chrono::steady_clock> m_TimePoint;

    // Thread pool 
    bool m_Running = true;
    std::mutex m_QueueLock;
    std::mutex m_PoolLock;
    std::condition_variable m_JobCondition;
    std::vector<std::thread> m_ThreadPool;
    std::queue<std::function<void()>> m_JobQueue;

};

#endif // SERVER_HPP



