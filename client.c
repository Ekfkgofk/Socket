//201912107 배수지
//Linux에서 실행이 됩니다.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SIZE 100

int main(int argc, char* argv[]) // 인자의 개수, 문자열로 된 인자들이 저장된 포인터 배열
{
    int sock;
    char message[SIZE]; // 서버에 보낼 message를 배열로 선언
    int length;         // 문장을 대입할 변수
    struct sockaddr_in servaddr; // 서버의 소켓주소

    if (argc != 3) { // 명령어를 포함해서 인자의 개수가 3개가 아니면, 즉, 인자의 개수가 2개(ip주소와 port번호)가 아닐경우 아래 문장을 출력하고 종료
        printf("입력 방식 : %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0); // IPv4프로토콜 사용, TCP 패킷을 >받는 소켓

    memset(&servaddr, 0, sizeof(servaddr)); //메모리 초기화

    servaddr.sin_family = AF_INET;                //서버주소.패밀리용
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);//서버주소.ip주소
    servaddr.sin_port = htons(atoi(argv[2]));     //서버주소.port번호

    connect(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)); // 클라이언트에서 호출하는 connect함수, 두번째 값에 서버의 소켓주소가 들어감

    puts("연결되었습니다..");
    while (1)
    {
        fputs("메세지 입력(종료=q입력): ", stdout);
        fgets(message, SIZE, stdin);

        if (!strcmp(message, "q\n")) //q입력시 연결 종료
            break;

        write(sock, message, strlen(message));  // 데이터 송신을 위한 3개의 값
        length = read(sock, message, SIZE - 1); // 문장 단위로 데이터 송신
        message[length] = 0;                    // 문장 초기화
        printf("서버로부터 온 메세지: %s", message);
    }
    close(sock);
    return 0;
}
