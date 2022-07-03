# CNT4504 Concurrent Server/Client

## About
This project was an exercise in which a multi-threaded client sends many concurrent connections to a multi-threaded server for the purpose of stress-testing the capabilities of a MT server when it gets flooded with requests. The server and client must be compiled on a Linux-based system using G++. The requests sent by the client are enums for specific terminal command. Enums were used so remote code execution wouldn't be possible. The available Linux commands are date, uptime, free, netstat, and who. The round trip time of the requests is measured and recorded by the client for each thread so the average turnaround time can be used in later analysis.

## Features
There were some extra features implemented for the sake of learning and efficiency.

- Time-based cache was used so that unnecessary system calls and pipes wouldn't need to be opened for each incoming request.
- Thread pool implemented using unique_mutex and queues which decreases average turnaround time because the overhead of thread creation is only done once initial server runtime.

## How to use
Start by compiling the server and client using G++ and the given makefiles. Once compiled, the user can start the server and tell it which port to listen to. After the server starts listening on that port, the client can then be started. The client will ask the user for the IP address of the server (localhost if the server and client are on the same machine), which port it is listening on, the command which the server will be receiving, and how many concurrent connections to send to the server.
