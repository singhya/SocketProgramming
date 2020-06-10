#include <iostream>
#include <winsock.h>
using namespace std;
#define PORT 9909

int nClientSocket;
struct sockaddr_in srv;


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
    nClientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (nClientSocket < 0)
    {
        cout << endl << "The socket not opened";
        WSACleanup();
        exit(EXIT_FAILURE);
    }
    else
    {
        cout << endl << "The socket opened successfully : " << nClientSocket;
    }

    // Initialize the environment for sockaddr structure
    srv.sin_family = AF_INET;
    srv.sin_port = htons(PORT);
    srv.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(&(srv.sin_zero), 0, 8);

    nRet = connect(nClientSocket, (struct sockaddr*)&srv, sizeof(srv));
    if (nRet < 0)
    {
        cout << endl << "connect failed";
        WSACleanup();
        return (EXIT_FAILURE);
    }
    else
    {
        cout << endl << "Connected to the server";
        char buff[255] = { 0, };
        recv(nClientSocket, buff, 255, 0);
        cout << endl << "Just press any key on keyboard to see the message received from the server.";
        getchar();
        cout << endl << buff;

        // Actual communication
        cout << endl << "Now send your messages to server:";
        while (1)
        {
            fgets(buff, 256, stdin);
            cout << endl << "Sending message on socket : "<< nClientSocket;
            send(nClientSocket, buff, 256, 0);
            cout << endl << "Press any key to get the response from server...";
            getchar();
            recv(nClientSocket, buff, 256, 0);
            cout << endl << buff << endl << "Now send next message:";
        }
    }
}