#include<Winsock2.h> 
#include<stdio.h>
#include<string.h>
#include<iostream>
#pragma comment(lib,"ws2_32")
#define SERVER_PORT 6666
using namespace std;
int main(){
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2,1); 
	WSAStartup(wVersionRequested, &wsaData);
	
	SOCKET sockSrv=socket(AF_INET,SOCK_STREAM,0);//������������socket
	struct sockaddr_in server_addr;//�������˵�ַ
	
	char buffer[200];
	int addrlen=sizeof(server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);//�������˿ںŶ�Ϊ6666
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockSrv,(SOCKADDR*)&server_addr,sizeof(SOCKADDR));//�׽��ְ󶨷������˵�ַ
	listen(sockSrv,5);//���е���󳤶�Ϊ5
	
	while(1){
		printf("�����˿ڣ�%d\n",SERVER_PORT);
		struct sockaddr_in addrClient;//�ͻ��˵�ַ
		SOCKET client = accept(sockSrv, (SOCKADDR*)&addrClient, &addrlen);//�������׽������ӿͻ�
		cout<<"��ǰ�ͻ��Ķ˿ںţ�"<<addrClient.sin_port<<endl;//�����ǰ�ͻ��Ķ˿ں�
		while(1){
			//buffer[0]='\0';//ÿ�ζ�Ҫ��ʼ��
			int len=recv(client, buffer, 50, 0);
			buffer[len]='\0';//����Ĳ�Ҫ��
			if(strcmp(buffer,"quit")==0)break;
			printf("���յ���Ϣ��%s",buffer);
			printf("\n");	

			printf("������Ϣ��");
			scanf("%s",buffer);
			send(client, buffer, strlen(buffer), 0);
			if(strcmp(buffer,"quit")==0)break;
		}
	}
	closesocket(sockSrv);
	WSACleanup();

}
//g++ server.cpp -o server.exe -lwsock32
