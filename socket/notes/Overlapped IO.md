# 윈도우 환경의 비동기 입출력 처리
윈도우 환경에서 소켓의 입출력을 비동기로 처리할수 있는 방식을 Overlapped IO라고 한다. 말 그대로, 입출력 처리를 동시에 (실제로 병렬적으로 작업을 처리하지는 않는다) 겹친것 같이 처리하는 것을 의미한다.

## Overlapped IO와 논블록킹 소켓
Overlapped IO는 논블록킹 소켓을 통해 구현된다. 왜냐하면, 비동기 처리를 위해서는 반드시 논블록킹으로 입출력이 처리되어야 하기 때문이다. 

## Overlapped IO 구현 방식
대표적인 Overlapped IO의 구현 방식은 총 세가지가 존재한다.

### Event Object
이 방식은 이벤트 시그널을 통해 Overlapped IO를 구현하는 방식이다. OVERLAPPED 구조체의 hEvent에 이벤트를 할당 한 후, 해당 이벤트가 WaitForSingleObject() 혹은 WaitForSingleObjects()를 통해 감지되면, GetOverlappedResult()를 통해 입출력의 결과를 확인 하는 방식이다.

### Completion Routine
이 방식은 워커 스레드를 alertable wait 상태, 즉, 스레드가 운영체제가 전달하는 메세지의 수신을 대기하는 상태일때 IO 가 완료되면, WSASend, WSAReceive 함수등에 전달된 Completion Routine 함수가 실행되어, 입출력 완료 처리를 하는 방식이다.

스레드가 alertable wait 상태일때만 해당 작업을 처리하는 이유는, 만약 스레드가 중요한 작업을 하고있는데, IO 완료 이벤트가 발생하여 처리해야한다면, 프로그램의 흐름이 망가질 수 있기 때문이다.

### IOCP (Input Output Completion Port)
IOCP는 윈도우 환경에서 제일 고성능의 Overlapped IO 완료 처리 방식이다. IOCP는 구현 방법도 복잡하고 어렵기 때문에 다음 포스트에 따로 다루겠다.