#include"tool.h"
#include<Winsock2.h>
#include<vector>
#pragma comment(lib,"ws2_32")
using namespace std;
#define SERVER_PORT 6666
#define CLIENT_PORT 6665
#define WINDOW_SIZE 4	//���ڴ�С

SOCKET localSocket;
struct sockaddr_in serverAddr,clientAddr;//���ն˵�ip�Ͷ˿ں���Ϣ 
DWORD WINAPI handlerRequest(LPVOID lpParam);

bool begin_recv=false;//���Կ�ʼ���� 
bool waiting=false; //�ȴ����� 
string temp;

int seq_num=0;//��ţ�����nxtseqnum


//���ͱ���
void send_to(char *message){
	udp_message u=udp_message(SERVER_PORT,seq_num,1024-10,0,message);//�����udp 
	//�����ı���msg
	char msg[bufferSize];
	u.real_message(msg);
	//��������
	sendto(localSocket,msg,sizeof(msg),0,(SOCKADDR*)&serverAddr,sizeof(SOCKADDR));//��С����Ҫ
}


//���� 
void recv_from(char *message){
	char msg[bufferSize];
	int size=sizeof(serverAddr);
	recvfrom(localSocket,msg,sizeof(msg),0,(SOCKADDR*)&serverAddr,&size);
	analyse(msg,message);
}


//�����ļ�����У���
 void send_file(string path){
 	FILE *fin=fopen(path.c_str(),"rb");
 	char buffer[4096];
 	while(!feof(fin))
 	{
 		fread(buffer,1,sizeof(buffer),fin);
 		cout<<strlen(buffer)<<endl;
 		sendto(localSocket,buffer,sizeof(buffer),0,(SOCKADDR*)&serverAddr,sizeof(SOCKADDR));
 		memset(buffer,0,sizeof(buffer));
 	}
 	cout<<"finish"<<endl;
 	fclose(fin);
 } 


double dt;
clock_t start,end;
double overtime=1000;//��ʱʱ�� 
//���ļ��ֿ�洢��v��
vector<char*>v;
char block[2048][1024];


int base=0;
void send_file_2(string path){
	//�ȱ��浽������
	FILE *fin=fopen(path.c_str(),"rb");
	int i=0;
	while(!feof(fin)){
		fread(block[i],1,sizeof(block)[i]-10,fin);
		v.push_back(block[i++]);//��������
	}
	fclose(fin);
	
	char *message;
	int n=v.size();
	while(seq_num<n)
	{
		end=clock();
		dt = (double)(end - start);
		//�����ʱ��δȷ�ϵ�ȫ���ش�
		if(dt>=overtime){
			cout<<"timeout="<<dt<<endl; 
			seq_num=base; 
			
		}
		//�������пռ䣬�Ҳ���ͣ��״̬ʱ�������ͱ��� 
		if(seq_num<base+WINDOW_SIZE)
		{
			message=v[seq_num];
			send_to(message);
			start=clock(); 
			begin_recv=true;//***���Կ�ʼ���շ���������Ϣ��***
			cout<<seq_num<<' '<<strlen(message)<<endl;
			seq_num++; //�µı��ģ���ű仯
			if(base==seq_num){//��base=nxtʱ����ʼ��ʱ�����൱�ڳ�ʼ����
				start=clock(); 
			}
		}
	}
	cout<<"finish"<<endl;
} 
double space=1859130;
void send_test(){
 	send_file_2("C:\\Users\\Mika\\Desktop\\���������\\����ҵ3\\����1�����ļ�\\1.jpg");
 //   send_file_2("C:\\Users\\Mika\\Desktop\\���������\\����ҵ3\\����1�����ļ�\\2.jpg");
  //  send_file_2("C:\\Users\\Mika\\Desktop\\���������\\����ҵ3\\����1�����ļ�\\3.jpg");
 //  send_file_2("C:\\Users\\Mika\\Desktop\\���������\\����ҵ3\\����1�����ļ�\\helloworld.txt");
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

	cout<<">>>";
	string sendbuf;
	cin>>sendbuf;
	send_test();

	
	Sleep(10000);//һ��Ҫ��һ�ᣬ�����߳������ص���û����ӡ��� 
	//CloseHandle(hThread);
	closesocket(localSocket);
	WSACleanup();
}

//������յ��߳� 
DWORD WINAPI handlerRequest(LPVOID lpParam){
	while(1){
		Sleep(10);//�ı�begin_recv����Ҫ��һ���ٽ��գ���Ȼ�ᶪ����һ��ACK 
		char msg[bufferSize],message[bufferSize];
		int size=sizeof(serverAddr);
		if(begin_recv==false){
			continue;
		}
		recvfrom(localSocket,msg,sizeof(msg),0,(SOCKADDR*)&serverAddr,&size);

		if(analyse(msg,message)){
			cout<<"ACK"<<GET_WHOLE(msg[2],msg[3])<<endl; 
			base=GET_WHOLE(msg[2],msg[3])+1;
			//���յ�һ��ack������һ������ʼ��ʱ
			start=clock();
		}
	} 
}
