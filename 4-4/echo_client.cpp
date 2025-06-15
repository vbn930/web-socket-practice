#include <iostream>
#include <string>
#include <winsock2.h>

#define BUF_SIZE 1024
void ErrorHandling(const char* message);

int main(int argc, char* argv[]){
    WSADATA wsaData;
    SOCKET sock;
    SOCKADDR_IN sockAddr;

    char message[BUF_SIZE];
    int strLen = 0;

    if(argc != 3){
        std::cout << "Usage: " << argv[0] << " <IP> <PORT>\n";
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET){
        ErrorHandling("socket()");
    }

    sockAddr = SOCKADDR_IN{};
    sockAddr.sin_addr.s_addr = inet_addr(argv[1]);
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(atoi(argv[2]));

    if(connect(sock, (SOCKADDR*) &sockAddr, sizeof(sockAddr)) == SOCKET_ERROR){
        ErrorHandling("connect()");
    }

    while(true){
        std::cout << "Input message (Q to quit): ";
        std::cin >> message;
        
        if(!strcmp(message, "q") || !strcmp(message, "Q")){
            break;
        }

        send(sock, message, strlen(message), 0);
        strLen = recv(sock, message, BUF_SIZE-1, 0);
        message[strLen] = 0;
        std::cout << "Message from server: " << message << "\n";
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* message){
    std::cerr << message << " error!" << std::endl;
    exit(1);
}