#include<Winsock2.h> 
#include<stdio.h>
#include<string.h>
#pragma comment(lib,"ws2_32")
#define SERVER_PORT 6666

int main(){
    WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2,1); 
	WSAStartup(wVersionRequested, &wsaData);

    char sendbuf[200],recvbuf[200];
    int serverSocket=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	//ȥ��6666�˿ڷ���������
    connect(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    while(1)
	{
		printf("������Ϣ��");
		scanf("%s", sendbuf);
		send(serverSocket, sendbuf, strlen(sendbuf), 0); //�����˷�����Ϣ
		if(strcmp(sendbuf, "quit") == 0) break;


		printf("���ܵ���Ϣ��");
		recvbuf[0]='\0';
		int len=recv(serverSocket, recvbuf, 200, 0); //���շ���˷�������Ϣ
		recvbuf[len]='\0';//����Ĳ�Ҫ��
		printf("%s\n", recvbuf);
	}
    closesocket(serverSocket);
	WSACleanup();
}