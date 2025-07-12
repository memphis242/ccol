// I will host a socket server here that will provide information on the state
// of the static array arena that a client program can use to, say, produce
// a visual of the memory. Plan at the moment will be to write a Python client
// script to do such a visualization.
// TODO: Array Arena Viz

#include <pthread.h> // Use separate thread to handle server socketing

// TODO: Figure out how to include sockets stuff..
// NOTE: Below is the suggestion by GPT 4.1 regarding cross-platform socketing.
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
    #define CLOSESOCKET(s) closesocket(s)
    typedef SOCKET socket_t;
#else
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #define CLOSESOCKET(s) close(s)
    typedef int socket_t;
#endif

#ifdef _WIN32
    // NOTE: Before using the socket API in Win32, do the following:
    // WSADATA wsa;
    // WSAStartup(MAKEWORD(2,2), &wsa);
#endif

// Use socket(), connect(), send(), recv()...

CLOSESOCKET(sock);

#ifdef _WIN32
    WSACleanup();
#endif

