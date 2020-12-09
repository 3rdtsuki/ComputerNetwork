#include"tool.h"
#include<Winsock2.h>
#include<time.h> 
#pragma comment(lib,"ws2_32")
using namespace std;
#define SERVER_PORT 6666

SOCKET localSocket;
struct sockaddr_in serverAddr,clientAddr;//���ն˵�ip�Ͷ˿ں���Ϣ 
DWORD WINAPI handlerRequest(LPVOID lpParam);
DWORD WINAPI handlerTimer(LPVOID lpParam);

bool begin_recv=false;//���Կ�ʼ���� 
bool waiting=false; //�ȴ����� 
string sendbuf="";
string temp;

int seq_num=1;//���
clock_t start;
bool begin_timer=false;
 
//���ͱ��� ������[���1λ�����ģ�У���16λ] 
void send_to(string sendbuf){
	udp_message message=udp_message(SERVER_PORT,seq_num,sendbuf.size()*8,0,sendbuf);//�����udp 
	string real_message=message.real_message();
	const char *buffer=real_message.c_str();
	//��������
	sendto(localSocket,buffer,real_message.size()*8,0,(SOCKADDR*)&serverAddr,sizeof(SOCKADDR));
	//��ʼ��ʱ
	start=clock(); 
	begin_timer=true;
}


//���� 
string recv_from(){
	char recvbuf[200];
	int size=sizeof(serverAddr);
	recvfrom(localSocket,recvbuf,200,0,(SOCKADDR*)&serverAddr,&size);
	//cout<<"�������˿ںţ�"<<ntohs(serverAddr.sin_port); 
	string s=recvbuf;
	return s;
}

int main(){
	//��ʼ��
    WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD(2,1); 
	WSAStartup(wVersionRequested, &wsaData);

	//ȥ���ӷ�������socket
    localSocket=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    
    //��������ip�Ͷ˿ں� 
    serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(6666);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	clientAddr.sin_family = AF_INET;
	clientAddr.sin_port = htons(6665);
    clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	HANDLE hThread=CreateThread(NULL, 0, handlerRequest,LPVOID(), 0,NULL);
	HANDLE hThread2=CreateThread(NULL, 0, handlerTimer,LPVOID(), 0,NULL);
	
	
	cout<<">>>";
	
	
	while(1) {
		cin>>sendbuf;
		Sleep(10);//�ȴ�waiting���� 
		//�����ڲ���ͣ��״̬ʱ�������ͱ��� 
		if(waiting==false){
			seq_num=!seq_num; //�µı��ģ���ű仯 
			send_to(sendbuf);//����
						
			temp=sendbuf;//���棬�����ش� 
			waiting=true;
		}
		begin_recv=true;//***���Կ�ʼ���շ���������Ϣ��***
		if(sendbuf=="quit"){
			begin_recv=false;
			break;
		}
	}
	CloseHandle(hThread);
	closesocket(localSocket);
	WSACleanup();
}

//������յ��߳� 
DWORD WINAPI handlerRequest(LPVOID lpParam){
	while(1){
		Sleep(10);//�ı�begin_recv����Ҫ��һ���ٽ��գ���Ȼ�ᶪ����һ��ACK 
		string s=recv_from();
		if(begin_recv==false)continue;//�����ܽ��� 
		
		begin_timer=false;//�յ�ack��ֹͣ��ʱ 
		
		int server_port=bin2dec(s.substr(0,16));
		int seq_num_2=bin2dec(s.substr(16,16));
		int length=bin2dec(s.substr(32,16));
		int check_sum=bin2dec(s.substr(48,16));
		string message_asc=s.substr(64,length);
		string message="";
		for(int i=0;i<length;i+=8){
			message+=bin2dec(message_asc.substr(i,8));
		}
		udp_message m=udp_message(server_port,seq_num_2,length,check_sum,message);
		m.print();

		int sum=0;
		for(int i=0;i<s.size();i+=16){
			sum+=bin2dec(s.substr(i,16));
			sum=(sum>>16)+(sum&0xffff);
		}
		if(sum==0xffff&&seq_num_2==seq_num){//У�����ȷ�������ȷ�����Է�����һ��
			waiting=false;
			cout<<">>>";
		}
		else{//�����ȴ���ȷACK������ʱ�ش�
			waiting=true; 
		}
	} 
}
DWORD WINAPI handlerTimer(LPVOID lpParam){
	double dt;
    clock_t end;
    while(1){
    	end = clock();
    	dt = (double)(end - start);
    	if(begin_timer==true && dt==300 ){
		//�������300���룬���������==����������dt>1000���쳣��� 
			cout<<"timeout="<<dt<<",resend"<<endl; 
			begin_timer=false;
			send_to(temp);//��ʱ�ش�
		}
	}
}
