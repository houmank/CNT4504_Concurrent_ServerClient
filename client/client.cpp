#include "client.hpp"

Client::Client(const serverInfo& servInfo)
    : m_ServerInfo(servInfo) 
{
    // empty message buffer
    m_MsgBuffer.fill('\0');

    // Create the socket for us to bind to later
    m_ServerID = socket(AF_INET, SOCK_STREAM, 0);
    CHK_ERR(m_ServerID, "Creating a socket")

    // Get the proper address resolved and attempts to open a connection
    OpenConnection();

    // Now we are ready to send and receive data
    // Each client will hold their own Timer object and will be timing the Turn-around time.
    m_Timer.StartTimer();
    SendAndRecv();
    m_Timer.StopTimer();

    sscout << "Thread ID: " << std::this_thread::get_id() << '\n'
           << "Server: \n"
           << &m_MsgBuffer[0] << '\n'
           << "\nThe turn-around time was: " << m_Timer.GetDurationMicro().count() * 0.001 << " milliseconds." 
           << "\n------------------------------------------------------------------------------\n" << std::endl;

    // Close the socket fd
    close(m_ServerID);    
}

Client::~Client()
{}

Timer Client::GetTimer() { return m_Timer; }

void Client::OpenConnection()
{
    // Clear the uninitialized memory
    memset(&m_ServerAddress, 0, sizeof(m_ServerAddress));

    // Set the sockaddr fields
    m_ServerAddress.sin_family = AF_INET;
    m_ServerAddress.sin_port = htons(m_ServerInfo.portNumber);

    // call gethostbyname to get the correct hint to attach to sockaddr_in
    hostent* hostQuery = gethostbyname(m_ServerInfo.serverAddress.c_str());
    if(!hostQuery)
    {
        std::cerr << "Error getting host";
        exit(0);
    }
    
    // copy the data over to the sockaddr
    memcpy(&m_ServerAddress.sin_addr.s_addr, hostQuery->h_addr_list[0], hostQuery->h_length);

    // Attempt to connect to the server
    CHK_ERR(connect(m_ServerID, (sockaddr*)&m_ServerAddress,
     sizeof(m_ServerAddress)), "Connecting to server")
}

void Client::SendAndRecv()
{
    // Declare and initialize local variables
    int totalBytes = 0, expectedBytes = 0;

    // Send client request code to server
    m_NumBytes = write(m_ServerID, &m_ServerInfo.userSelection, sizeof(m_ServerInfo.userSelection));
    CHK_ERR(m_NumBytes, "Sending a message to server")
    const int sent = m_NumBytes;

    /// Read the number of bytes that the client should expect to recv
    m_NumBytes = read(m_ServerID, &expectedBytes, sizeof(expectedBytes));
    CHK_ERR(m_NumBytes, "Receiving data size")

    do
    {
        // Get m_NumBytes from server via TCP buffer
        m_NumBytes = read(m_ServerID, &m_MsgBuffer[totalBytes], m_MsgBuffer.size() -  totalBytes - 1);
        CHK_ERR(m_NumBytes, "Receiving a message to server")
        if(m_NumBytes == 0) // Done reading data from server
        {
            break;
        }
        else
        {
            totalBytes += m_NumBytes; // Still data left in the buffer
        }
    } while(totalBytes < expectedBytes); // Check to see if we recieved all data after one recv() call
    
    sscout << "Bytes sent: " << m_NumBytes << '\n'
           << "Bytes recieved: " << totalBytes << '\n';
}