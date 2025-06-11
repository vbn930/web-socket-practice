#include <iostream>
#include <stdlib.h>
#include <winsock2.h>

void ErrorHandling(std::string message);

int main(int argc, char *argv[]){
    WSADATA wsaData;
    SOCKET hSock;
    SOCKADDR_IN servAddr;

    int strLen;
    char message[30];

    if(argc != 3){
        std::cout << "Usage : " << argv[0] << " <IP> <port>\n";
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    //소켓 생성
    hSock = socket(PF_INET, SOCK_STREAM, 0);
    if(hSock == INVALID_SOCKET){
        ErrorHandling("socket()");
    }

    servAddr = SOCKADDR_IN{};
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(argv[1]);
    servAddr.sin_port = htons(atoi(argv[2]));
    
    //생성된 소켓을 이용해 서버에 연결을 요청하고있다.
    if(connect(hSock, (SOCKADDR*) &servAddr, sizeof(servAddr)) == SOCKET_ERROR){
        ErrorHandling("connect()");
    }

    strLen = recv(hSock, message, sizeof(message)-1, 0);
    if(strLen == -1){
        ErrorHandling("recv()");
    }

    std::cout << "Message from server : " << message << "\n";

    //일반적으로 클라이언트 소켓을 먼저 닫은 후 리스닝 소켓을 닫는다.
    closesocket(hSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(std::string message){
    std::cerr << message << " error\n";
    exit(1);
}