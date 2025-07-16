#include <iostream>
#include <stdlib.h>
#include <winsock2.h>
#include <vector>

void ErrorHandling(std::string message);

int main(int argc, char *argv[]){
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;

    int szClntAddr;
    int sendSize;

    char messageUnit[] = "Hello World!";
    std::vector<char*> messages = { messageUnit, messageUnit};

    if(argc != 2){
        std::cout << "Usage : " << argv[0] << " <port>\n";
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    //소켓 생성
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    if(hServSock == INVALID_SOCKET){
        ErrorHandling("socket()");
    }

    servAddr = SOCKADDR_IN{};
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1]));
    
    //소켓에 IP 주소와 port 할당
    if(bind(hServSock, (SOCKADDR*) &servAddr, sizeof(servAddr)) == SOCKET_ERROR){
        ErrorHandling("bind()");
    }

    //소켓이 클라이언트의 연결 요청을 받을수 있는 상태로 만듦
    if(listen(hServSock, 5) == -1){
        ErrorHandling("listen()");
    }

    szClntAddr = sizeof(clntAddr);

    //클라이언트에서의 연결 요청을 수락
    hClntSock = accept(hServSock, (SOCKADDR*) &clntAddr, &szClntAddr);
    if(hClntSock == INVALID_SOCKET){
        ErrorHandling("accept()");
    }

    for(char* message : messages){
        sendSize = send(hClntSock, message, strlen(message), 0);
        std::cout << "Message size: " << sizeof(message) << "\n";
        std::cout << "Send message: " << message << ", Send bytes: " << sendSize << "\n";
    }

    //일반적으로 클라이언트 소켓을 먼저 닫은 후 리스닝 소켓을 닫는다.
    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(std::string message){
    std::cerr << message << " error\n";
    exit(1);
}