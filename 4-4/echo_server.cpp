#include <iostream>
#include <string>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(const char* message);

int main(int argc, char* argv[]){
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;
    int clntAddrSize;
    char message[BUF_SIZE];
    int strLen = 0;

    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <PORT>\n";
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    hServSock = socket(AF_INET, SOCK_STREAM, 0);
    if(hServSock == INVALID_SOCKET){
        ErrorHandling("socket()");
    }

    servAddr = SOCKADDR_IN{};
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[1]));

    if(bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr)) == SOCKET_ERROR){
        ErrorHandling("bind()");
    }

    if(listen(hServSock, 5) == SOCKET_ERROR){
        ErrorHandling("listen()");
    }

    clntAddrSize = sizeof(clntAddr);
    for(int i = 0; i < 5; i++){
        hClntSock = accept(hServSock, (SOCKADDR*) &clntAddr, &clntAddrSize);
        if(hClntSock == SOCKET_ERROR){
            ErrorHandling("accept()");
        }

        std::cout << "Connected server: " << i+1 << "\n";

        while((strLen = recv(hClntSock, message, BUF_SIZE, 0)) != 0){
            send(hClntSock, message, strLen, 0);
        }

        closesocket(hClntSock);
    }

    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* message){
    std::cerr << message << " error!" << std::endl;
    exit(1);
}