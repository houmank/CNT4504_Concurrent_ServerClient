#include "server.hpp"

Server::Server(int port)
    : m_PortNumber(port)
{
    const int maxThreads = std::thread::hardware_concurrency();
    m_ThreadPool.reserve(maxThreads);

    // initialize threadpool
    for(int i = 0; i < maxThreads; i++)
    {
        m_ThreadPool.emplace_back(&Server::GetJobs, this);
    }

    // Create the socket
    m_ServerID = socket(AF_INET, SOCK_STREAM, 0);
    CHK_ERR(m_ServerID, "Creating the socket")

    // Bind ip and port to socket
    memset(&m_ServerAddress, 0, sizeof(m_ServerAddress));
    m_ServerAddress.sin_family = AF_INET; // IPv4
    m_ServerAddress.sin_port = htons(m_PortNumber); // Fix endianness
    m_ServerAddress.sin_addr.s_addr = INADDR_ANY; // Sets to address of machine

    CHK_ERR(bind(m_ServerID, (sockaddr*)&m_ServerAddress, sizeof(m_ServerAddress)),
            "Binding of address to socket")


    // Listen to the socket for connections
    CHK_ERR(listen(m_ServerID, SOMAXCONN), "Setting the socket to listen")
}

void Server::AcceptCons()
{
    // clear the array initially
    m_MsgBuffer.fill('\0');

    // main loop
    while(true)
    {
        // Clear the buffers and sockaddrs
        memset(&m_ClientAddress, 0, sizeof(m_ClientAddress));

        // Accept connections
        m_ClientAddrLength = sizeof(m_ClientAddress);
        std::cout << "Listening for connections...\n";
        int clientID = accept(m_ServerID, (sockaddr*)&m_ClientAddress, &m_ClientAddrLength);
        std::cout << "Connection Accepted.\n";
        CHK_ERR(clientID, "accepting a connection")

        // add new connection to thread pool
        AddJobs(std::bind(&Server::HandleConn, this, clientID));
    }
}

void Server::AddJobs(std::function<void()> f)
{
    {
        // add job to job queue
        std::unique_lock<std::mutex> queueLock(m_QueueLock);
        m_JobQueue.push(std::move(f));
    }
    // let threads know there is a new connection
    m_JobCondition.notify_one();
}

void Server::GetJobs()
{
    while(m_Running)
    {
        {
            std::unique_lock<std::mutex> queueLock(m_QueueLock);
            std::function<void()> job;
            if(!m_JobQueue.empty())
            {
                job = std::move(m_JobQueue.front());
                m_JobQueue.pop();
                job();
            }
            else
            {
                std::function<bool()> pred = [&](){return !m_JobQueue.empty();};
                m_JobCondition.wait(queueLock, pred);
            }       
        }
    }
}

void Server::HandleConn(int clientID)
{ 
    int selection;
    int numBytes = read(clientID, &selection, sizeof(int));
    CHK_ERR(numBytes, "Reading data from client")
    SelectCommand(m_MsgBuffer, selection);
    // std::cout << "Bytes Read: " << numBytes << '\n';

    // send initial size incase it neads to be read in chunks
    {
        std::lock_guard<std::mutex> lock(m_MsgLock);
        int msgLen = strlen(m_MsgBuffer.begin());
        numBytes = write(clientID, &msgLen, sizeof(msgLen));

        numBytes = write(clientID, &m_MsgBuffer[0], msgLen); 
        CHK_ERR(numBytes, "Writing data to client")
       
        // printf("Response: \n%s\nClosing Connection.\n", m_MsgBuffer.begin());
    }
    std::cout << "Bytes Sent: " << numBytes << "\nConnection closed.\n";
}

void Server::ShutDown()
{
    close(m_ServerID);

    // thread clean up
    std::lock_guard<std::mutex> lock(m_PoolLock);
    m_Running = false;
    m_JobCondition.notify_all(); // let all threads know to stop waiting on jobs

    for(auto& t : m_ThreadPool)
        t.join(); // ensure all threads are finished before destroying server
}

void Server::SelectCommand(std::array<char, 1024 * 32>& msgBuffer, int userSelection)
{
    {
        std::lock_guard<std::mutex> lockTime(m_TimePointLock);

        // Check cache
        if(userSelection == m_LastSelection)
        {
            // check if it has been one minute
            auto endTimePoint = std::chrono::steady_clock::now();
            auto end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTimePoint);
            auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_TimePoint);

            auto diff = end - start;
            
            // if last cache was made less than one second ago, use cache
            if(diff.count() < 1000)
                return;
        }

        // else we update user selection and time stamp new cache
        m_LastSelection = userSelection;
        m_TimePoint = std::chrono::steady_clock::now();
    }

    switch (userSelection)
    {
    case 1:
        HandleCommand(msgBuffer, "date");
        break;
    case 2:
         HandleCommand(msgBuffer, "uptime");
        break;
    case 3:
        HandleCommand(msgBuffer, "free");
        break;
    case 4:
         HandleCommand(msgBuffer, "netstat");
        break;
    case 5:
         HandleCommand(msgBuffer, "who");
        break;
    case 6:
         HandleCommand(msgBuffer, "ps");
        break;
    default:
        {
            std::lock_guard<std::mutex> lock(m_MsgLock);
            snprintf(&msgBuffer[0], msgBuffer.size() - 1, "ERROR: invalid selection.");
            break;
        }
    }
}

void Server::HandleCommand(std::array<char, 1024 * 32>& msgBuffer, std::string command)
{
    // get the buffer ready for output
    {
        std::lock_guard<std::mutex> lock (m_MsgLock);
        snprintf(&msgBuffer[0], msgBuffer.size() - 1, command.c_str());
        memset(&msgBuffer[0] + command.size() + 1, '\0', msgBuffer.size() - command.size() - 1);


        GetCommandOutput(msgBuffer);
    }
}

void Server::GetCommandOutput(std::array<char, 1024 * 32>& msgBuffer)
{
    FILE* fp;
    {
        char* start = &msgBuffer[0];
        fp = popen(start, "r");
        if(!fp)
        {
            snprintf(start, msgBuffer.size(), "ERROR: could not execuite command");
            return;  
        }

        fread(start, sizeof(char), msgBuffer.size() - 1,  fp);
    }
    pclose(fp); 
}