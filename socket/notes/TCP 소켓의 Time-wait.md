# TCP 소켓의 통신에서의 Time-wait 상태
## 4-Way Handshake
4-Way Handshake란, TCP 연결을 종료하는 과정에서, 4단계의 종료 과정을 거치며 신뢰성 있는 연결 종료를 위해 사용되는 방법이다. 4-Way Handshake는 다음과 같은 순서로 진행된다.
1. 호스트 A가 종료를 원하는 FIN 패킷을 B에게 전송한다.
2. B는 A의 FIN 패킷에 대한 응답은 ACK 패킷을 A에게 전송한다.
3. B가 종료를 위해서 FIN 패킷을 A에게 전송한다.
4. A는 B에서 전달받은 FIN 패킷에 대한 응답으로 ACK 패킷을 전송한다. 이때, A는 ACK 패킷을 전송 후 Time-wait 상태로 진입하여 네트워크 문제로 ACK 패킷이 전달 되지 않았을때를 위해서 대기한다.

TCP 소켓은 위와 같은 과정으로 TCP 연결을 종료한다. 이때, 클라이언트가 먼저 연결을 종료한다면, PORT의 재사용에 문제가 없지만 (클라이언트 소켓의 포트는 임의로 할당됨) 서버가 먼저 연결을 종료 하였을때, 서버가 Time-wait 상태로 진입하면, 신뢰성 있는 연결 종료를 위해서 서버가 사용했던 PORT 번호를 일정 시간동안 사용하지 못하게 된다.

## TIME_WAIT
먼저 FIN 요청을 보낸 소켓이 TIME_WAIT 상태가 되는 이유는, 네트워크의 문제 혹은 ACK 패킷의 유실로 인해 잘못된 정보가 상대 호스트로 전달 되었을때를 대비하여 소켓이 대기 상태로 놓이게 되는 것이다.

## SO_REUSEADDR
소켓의 옵션을 변경하여, time-wait 상태에 놓인 소켓의 PORT 번호를 사용하도록 할 수 있다.
```cpp
int option = TRUE;
int optlen = sizeof(option);
setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (void*)&option, optlen);
```
위와 같이, 소켓의 SO_REUSEADDR 옵션을 true로 설정하면 해당 소켓이 time-wait 상태일때도 해당 소켓의 PORT 번호를 재활용 할 수 있다.

SO_REUSEADDR 옵션을 사용하여 TIME_WAIT 상태의 소켓의 PORT 재할당을 허용 한다고 하더라도, 소켓의 TIME_WAIT 상태가 끝나는것이 아니라, 대기 상태는 지속되는 채로 PORT의 바인딩만 허용하는 상태가 되는것이다.

따라서 SO_REUSEADDR 옵션은 TIME_WAIT 상태 소켓 PORT의 연결 불가 문제를 근본적으로 해결 하는 것이 아니라, 단순히 PORT의 바인딩을 가능하게만 해 주는 옵션이다.