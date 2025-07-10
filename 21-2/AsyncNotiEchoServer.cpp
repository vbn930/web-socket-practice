#include <iostream>
#include <string.h>
#include <winsock2.h>

#define BUF_SIZE 100

void CompressSockets(SOCKET hSockAddr[], int idx, int total);
void CompressEvents(WSAEVENT hEventAddr[], int idx, int total);
void ErrorHandling(char* msg);

int main(int argc, char* argv[]){
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;

    // 생성된 소켓과 이벤트를 담기 위한 배열 생성 (최대 개수는 이벤트의 최대 개수를 따른다)
    // -> 소켓과 이벤트가 짝을 이루기 때문에 같은 위치에 담겨야함
    SOCKET hSockAddr[WSA_MAXIMUM_WAIT_EVENTS];
    WSAEVENT hEventAddr[WSA_MAXIMUM_WAIT_EVENTS];
    WSAEVENT newEvent; // 이벤트 저장을 위한 변수
    WSANETWORKEVENTS netEvents; // 발생한 이벤트의 정보를 담기위한 변수

    int numOfClntSock = 0; // 소켓, 이벤트의 총 개수
    int strLen;
    int posInfo, startIdx;
    int clntAdrLen;
    char msg[BUF_SIZE];

    if(argc != 2){
        std::cout << "Usage: " << argv[0] << " <PORT>\n";
        exit(1);
    }

    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        ErrorHandling("WSAStartup()");
    }

    hServSock = socket(PF_INET, SOCK_STREAM, 0);
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

    newEvent = WSACreateEvent(); // Server 소켓의 이벤트 초기화
    // 소켓에 연결되는 이벤트를 Accept 감지 모드로 초기화
    if(WSAEventSelect(hServSock, newEvent, FD_ACCEPT) == SOCKET_ERROR){
        ErrorHandling("WSAEventSelect()");
    }

    // 각각 소켓과 이벤트를 배열에 넣음
    hSockAddr[numOfClntSock] = hServSock;
    hEventAddr[numOfClntSock] = newEvent;
    numOfClntSock++;

    while(true){
        // 첫번째로 감지된 이벤트의 위치를 획득
        posInfo = WSAWaitForMultipleEvents(numOfClntSock, hEventAddr, FALSE, WSA_INFINITE, FALSE);

        //첫 이벤트의 위치정보를 받아서 인덱스를 추출
        startIdx = posInfo - WSA_WAIT_EVENT_0;

        for(int i = startIdx; i < numOfClntSock; i++){
            int sigEventIdx = WSAWaitForMultipleEvents(1, &hEventAddr[i], TRUE, 0, FALSE); // 해당 위치에 발생한 이벤트가 있는지 검사
            if((sigEventIdx == WSA_WAIT_FAILED || sigEventIdx == WSA_WAIT_TIMEOUT)){
                continue; // 이벤트가 발생하지 않았다면 넘김
            }else{
                sigEventIdx = i; // 가독성을 위해 이벤트가 발생한 인덱스 값을 sigEventIdx 값으로 복사 (i 변수를 그대로 사용해도 무관)
                WSAEnumNetworkEvents(hSockAddr[sigEventIdx], hEventAddr[sigEventIdx], &netEvents); // 발생한 이벤트 정보를 netEvents에 저장
                if(netEvents.lNetworkEvents & FD_ACCEPT){ // 발생한 이벤트가 서버 소켓이 클라이언트 소켓의 연결 요청인 경우
                    if(netEvents.iErrorCode[FD_ACCEPT_BIT] != 0){
                        puts("Accept error!");
                        break;
                    }
                    
                    // 클라이언트 소켓과 그에 대응하는 이벤트 초기화
                    clntAdrLen = sizeof(clntAddr);
                    hClntSock = accept(hServSock, (SOCKADDR*) &clntAddr, &clntAdrLen);
                    newEvent = WSACreateEvent();
                    WSAEventSelect(hClntSock, newEvent, FD_READ|FD_CLOSE);

                    hSockAddr[numOfClntSock] = hClntSock;
                    hEventAddr[numOfClntSock] = newEvent;
                    numOfClntSock++;
                    puts("connected new clinet.....");
                }

                if(netEvents.lNetworkEvents & FD_READ){ // 발생한 이벤트가 클라이언트 소켓이 입력 버퍼의 데이터를 읽어올수 있다는 이벤트인 경우
                    if(netEvents.iErrorCode[FD_READ_BIT] != 0){
                        puts("Read Error!");
                        break;
                    }
                    
                    // 입력 버퍼의 메세지를 읽어오고 메세지를 그대로 재전송
                    strLen = recv(hSockAddr[sigEventIdx], msg, BUF_SIZE, 0);
                    send(hSockAddr[sigEventIdx], msg, strLen, 0);
                }

                if(netEvents.lNetworkEvents & FD_CLOSE){ // 발생한 이벤트가 클라이언트 소켓의 종료 이벤트인 경우
                    if(netEvents.iErrorCode[FD_CLOSE_BIT] != 0){
                        puts("Close Error!");
                        break;
                    }

                    // 이벤트와 소켓을 닫는다
                    WSACloseEvent(hEventAddr[sigEventIdx]);
                    closesocket(hSockAddr[sigEventIdx]);

                    // 클라이언트와 소켓의 배열을 조정한다
                    numOfClntSock--;
                    CompressEvents(hEventAddr, sigEventIdx, numOfClntSock);
                    CompressSockets(hSockAddr, sigEventIdx, numOfClntSock);
                }
            }
        }
    }
    WSACleanup();
    return 0;
}

void CompressSockets(SOCKET hSockAddr[], int idx, int total){
    for(int i = idx; i < total; i++){
        hSockAddr[i] = hSockAddr[i+1];
    }
}
void CompressEvents(WSAEVENT hEventAddr[], int idx, int total){
    for(int i = idx; i < total; i++){
        hEventAddr[i] = hEventAddr[i+1];
    }
}

void ErrorHandling(char* msg){
    std::cerr << msg << " Error!\n";
    exit(1);
}