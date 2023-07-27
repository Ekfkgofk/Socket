//201912107 배수지
//Linux에서 실행이 됩니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h> // sigaction 구조체 정의
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SIZE 100
void error(char* message) //에러 처리 함수
{
    fputs(message, stderr);
    exit(1);
}
void read_child(int sig)
{
    pid_t pid; // 프로세스 번호(pid)를 저장하는 타입(t)
    int state;
    pid = waitpid(-1, &state, WNOHANG);
    printf("removed process ID: %d \n", pid); // 이동된 process ID
}

int main(int argc, char* argv[]) // 인자의 개수, 문자열로 된 인자들이 저장된 포인터 배열
{
    int serversocket, clientsocket;
    struct sockaddr_in servaddr, clntaddr; // 서버의 소켓주소, 클라이언트의 소켓주소 

    pid_t pid;            // 프로세스(pid) 번호를 저장하는 타입(t)
    struct sigaction sig; // 신호 처리 함수
    socklen_t addr_size;  // 소켓 관련 매개 변수로 길이 및 크기에 대한 정의
    int length, s;
    char message1[SIZE];  // 클라이언트로 부터 받는 message를 저장할 곳

    if (argc != 2) {      // 명령어를 제외하고, 인자의 개수가 1개(port번호)가 아닐경우 아래 문장을 출력하고 종료
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    sig.sa_handler = read_child; // 부모 프로세스는 자식 프로세스 종료시 반환되는 값을 전달 받을 수 있도록 신호처리
    sigemptyset(&sig.sa_mask);   // 시그널 핸들러 함수가 실행되는 동안 블럭되어야 하는 시그널 마스크
    sig.sa_flags = 0;
    s = sigaction(SIGCHLD, &sig, 0); //자식프로세스가 종료되었을때의 신호

    serversocket = socket(PF_INET, SOCK_STREAM, 0);// IPv4프로토콜 사용, TCP 패킷을  받는 소켓
    memset(&servaddr, 0, sizeof(servaddr));        // 메모리 초기화

    servaddr.sin_family = AF_INET;                 // 서버주소.패밀리용
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);  // 서버주소.ip주소
    servaddr.sin_port = htons(atoi(argv[1]));      // 서버주소.port번호

    // ↓서버에서 사용하는 함수로 serversocket에 서버의 소켓 주소(ip주소, port번호)를 bind해준다
    if (bind(serversocket, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
        error("bind() error");

    // ↓서버에서 호출하는 함수로 임의의 클라이언트가 접속하기를 기다리는 함수
    // ↓두번째 값은 동시접속이 가능한 클라이언트의 수 이다
    if (listen(serversocket, 3) == -1)
        error("listen() error");

    while (1)
    {
        addr_size = sizeof(clntaddr);

        // ↓서버는 listen 상태에서 새로운 클라이언트가 들어오면, 클라이언트를 수용하기 위해 accept()함수 호출
        // ↓클라이언트-서버 TCP연결이 되면 클라이언트 소켓주소와 새로운 소켓 번호 반환
        clientsocket = accept(serversocket, (struct sockaddr*)&clntaddr, &addr_size);
        if (clientsocket == -1)
            continue;
        else
            puts("새로운 client가 연결되었습니다..");

        pid = fork(); // 새로운 클라이언트가 들어오면 새로운 프로세스 생성
        // ↓fork()함수의 반환값에 따라 부모프로세스와 자식프로세스의 프로그램 구분
        if (pid == -1)
        {
            close(clientsocket);
            continue;
        }
        if (pid == 0) //자식 프로세스
        {
            close(serversocket);
            while ((length = read(clientsocket, message1, SIZE)) != 0) //클라이언트에서 온 메세지를 읽고 길이가 0이 아니면
                write(clientsocket, message1, length);                 //온 메세지를 다시 보냄

            close(clientsocket);
            puts("client의 연결이 종료되었습니다..");
            return 0;
        }
        else
            close(clientsocket);
    }
    close(serversocket);
    return 0;
}

