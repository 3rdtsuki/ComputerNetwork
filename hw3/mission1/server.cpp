#include"tool.h"
#include<Winsock2.h> 
#pragma comment(lib,"ws2_32")
using namespace std;
#define SERVER_PORT 6666
#define CLIENT_PORT 6665
SOCKET localSocket; 
//���͵�ip�Ͷ˿ں���Ϣ 
struct sockaddr_in clientAddr,serverAddr;


int send_to(string s){
	const char *c=s.c_str();
	//�������ݵ���ַserverAddr
	return sendto(localSocket,c,200,0,(SOCKADDR*)&clientAddr,sizeof(SOCKADDR));
}
//����udp���ģ����ر��Ķ�
string analyse(string s){
	int server_port=bin2dec(s.substr(0,16));
	int seq_num=bin2dec(s.substr(16,16));
	int length=bin2dec(s.substr(32,16));
	int check_sum=bin2dec(s.substr(48,16));
	string message_asc=s.substr(64,length);
	string message="";
	for(int i=0;i<length;i+=8){
		message+=bin2dec(message_asc.substr(i,8));
	}
	udp_message m=udp_message(server_port,seq_num,length,check_sum,message);
	m.print();

	int sum=0;
	for(int i=0;i<s.size();i+=16){
		sum+=bin2dec(s.substr(i,16));
		sum=(sum>>16)+(sum&0xffff);
	}
	//Sleep(1000);
	if(sum==0xffff){//У�����ȷ���ظ�ACK
		cout<<"У�����ȷ"<<endl;
		udp_message m=udp_message(CLIENT_PORT,seq_num,24,0,"ACK");
		string message=m.real_message();
		send_to(message);
	}
	else{
		cout<<"У��ʹ���"<<sum<<endl;
		udp_message m=udp_message(CLIENT_PORT,1-seq_num,24,0,"ACK");
		string message=m.real_message();
		send_to(message);
	}
	return message;
}

string recv_from(){
	char recvbuf[200];
	int size=sizeof(clientAddr);
	recvfrom(localSocket,recvbuf,200,0,(SOCKADDR*)&clientAddr,&size);
	string s=recvbuf;
	return analyse(s);
}

int main(){
    WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2,1); 
	WSAStartup(wVersionRequested, &wsaData);
	
	//����socket��ֻ������� 
    localSocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    //��������ip�Ͷ˿ں� 
    serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(6666);//htons��unsigned short���ʹ�������ת����������
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//�󶨱���socket�Ͷ˿ں� 
	bind(localSocket,(SOCKADDR*)&serverAddr,sizeof(SOCKADDR));
	cout<<"���ض˿ڣ�"<<ntohs(serverAddr.sin_port)<<endl;//ntohs��unsigned short���ʹ�������ת����������
	int recv_status=0;
	while(1) {
		string recvbuf=recv_from();
		cout<<recvbuf<<endl;
	}
	closesocket(localSocket);
	WSACleanup();
}
