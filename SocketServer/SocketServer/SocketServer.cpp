#include <iostream>
#include <winsock.h>
using namespace std;
#define PORT 9909

struct sockaddr_in srv;
fd_set fr, fw, fe;
int nMaxFd;
int nSocket;
int nArrClient[5];

void ProcessNewMessage(int nClientSocket)
{
    cout << endl << "Processing the new message for client socket : " << nClientSocket;
    char buff[256 + 1] = { 0, };
    cout << endl << "Receiving message on socket : " << nClientSocket;
    int nRet = recv(nClientSocket, buff, 256, 0);
    if (nRet < 0)
    {
        DWORD errorCode = GetLastError();
        cout << endl << "Receive failed with error code : " << errorCode ;
        cout << endl << "Closing the connection for client";
        closesocket(nClientSocket);
        for (int nIndex = 0; nIndex < 5; nIndex++)
        {
            if (nArrClient[nIndex] == nClientSocket)
            {
                nArrClient[nIndex] = 0;
                break;
            }
        }
    }
    else
    {
        cout << endl << "The message received from client is : " << buff;
        // Send the response to client
        send(nClientSocket, "Processed your Request", 23, 0);
        cout << endl << "************************************************";
    }
}

void ProcessTheNewRequest()
{
    if (FD_ISSET(nSocket, &fr))
    {
        // New connection request
        int nLen = sizeof(struct sockaddr);
        int nClientSocket = accept(nSocket, NULL, &nLen);
        if (nClientSocket > 0)
        {
            // Put it into the client fd set
            int nIndex;
            for (nIndex = 0; nIndex < 5; nIndex++)
            {
                if (nArrClient[nIndex] == 0)
                {
                    nArrClient[nIndex] = nClientSocket;
                    send(nClientSocket, "Got the connection done successfully", 37, 0);
                    break;
                }
            }
            if (nIndex == 5)
            {
                cout << endl << "No space for new connection";
            }
        }
    }
    else
    {
        for (int nIndex = 0; nIndex < 5; nIndex++)
        {
            if (FD_ISSET(nArrClient[nIndex], &fr))
            {
                // Fot the new message from the client
                // Just recv new message
                // Just queue that for new works of your server to fulfill the request
                ProcessNewMessage(nArrClient[nIndex]);
            }
        }
    }
}

int main()
{
    int nRet = 0;

    // Initialize the WSA variables
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) < 0)
    {
        cout << endl << "WSA Failed to initialize";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << endl << "WSA initialized";
    }

    // Initialize the socket
    nSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nSocket < 0)
    {
        cout << endl << "The socket not opened";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << endl << "The socket opened successfully : " << nSocket;
    }

    // Initialize the environment for sockaddr structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = INADDR_ANY;
    memset(&(srv.sin_zero), 0, 8);

    // setsockopt
    int nOptVal = 0;
    int nOptLen = sizeof(nOptVal);
    nRet = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOptVal, nOptLen);
    if (!nRet)
    {
        cout << endl << "The Setsockopt call successful.";
    }
    else
    {
        cout << endl << "The setsockopt call failed.";
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the local port
    nRet = bind(nSocket, (sockaddr*)&srv, sizeof(sockaddr));
    if (nRet < 0)
    {
        cout << endl << "Failed to bind to local port";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << endl << "Successfully bind to local port";
    }

    // Listen the request form client (queues the requests)
    nRet = listen(nSocket, 5);
    if (nRet < 0)
    {
        cout << endl << "Failed to start listen to local port";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << endl << "Started listening to local port";
    }

    nMaxFd = nSocket;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while(1)
    {
        FD_ZERO(&fr);
        FD_ZERO(&fw);
        FD_ZERO(&fe);
        
        FD_SET(nSocket, &fr);
        FD_SET(nSocket, &fe);

        for (int nIndex = 0; nIndex < 5; nIndex++)
        {
            if (nArrClient[nIndex] != 0)
            {
                FD_SET(nArrClient[nIndex], &fr);
                FD_SET(nArrClient[nIndex], &fe);
            }
        }
        
        // Keep waiting for new requests and proceed as per the request
        nRet = select(nMaxFd + 1, &fr, &fw, &fe, &tv);
        if (nRet > 0)
        {
            // When someone connects or communicates with a message over a dedicated connections

            // Please remember that the socket listening to new clients
            // and then communicating with the same client later are not the same.
            // After connection, you get one more socket to communicate with client.
            // You need to send/recv message over the network using that new socket
            cout << endl << "Data on port.... Processing now...";
            
            // Process the request.
            ProcessTheNewRequest();
        }
        else if (nRet == 0)
        {
            // No connection or any communication request made or you can say
            // that none of the socket descriptors are ready
        }
        else
        {
            cout << endl << "I failed";
            WSACleanup();
            exit(EXIT_FAILURE);
            //It failed and your application should show some useful message
        }
    }
    
}