#include <iostream>
#include <winsock2.h>
#include <stdlib.h>

#define BUF_SIZE 1024

void ErrorHandling(const char* message);

int main(int argc, char* argv[]){
    WSADATA wsaData;
    SOCKET hSock;
    SOCKADDR_IN sockAddr;
    char message[BUF_SIZE];
    int strLen, readLen;

    if(argc != 3){
        std::cout << "Usage: " << argv[0] << "<IP> <PORT>" << std::endl;
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    hSock = socket(PF_INET, SOCK_STREAM, 0); // 클라이언트는 하나의 서버와 연결되기 때문에 기본적인 블록킹 소켓을 사용한다
    if(hSock == INVALID_SOCKET){
        ErrorHandling("socket()");
    }

    sockAddr = {};
    sockAddr.sin_addr.s_addr = inet_addr(argv[1]);
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_port = htons(atoi(argv[2]));

    if(connect(hSock, (SOCKADDR*) &sockAddr, sizeof(sockAddr)) == SOCKET_ERROR){
        ErrorHandling("connection()");
    }else{
        puts("Connected.....");
    }

    while(true){
        std::cout << "Input message(Q to quit): ";
        std::cin >> message;
        
        if(!strcmp(message, "q") || !strcmp(message, "Q")){
            break;
        }

        strLen = strlen(message);
        send(hSock, message, strLen, 0);
        readLen = 0;

        while(true){
            readLen += recv(hSock, message, BUF_SIZE, 0); // 서버에서 발신한 메세지가 모두 수신될때 까지 반복
            if(readLen >= strLen){
                break;
            }
        }

        message[strLen] = 0;
        std::cout << "Message from server: " << message << std::endl;
    }

    closesocket(hSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(const char* message){
    std::cerr << message << " error!" << "\n";
}