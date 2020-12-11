#include"tool.h"
#include<Winsock2.h>
#pragma comment(lib,"ws2_32")
using namespace std;
#define SERVER_PORT 6666
#define CLIENT_PORT 6665
SOCKET localSocket;
struct sockaddr_in serverAddr,clientAddr;//���ն˵�ip�Ͷ˿ں���Ϣ 
DWORD WINAPI handlerRequest(LPVOID lpParam);

bool begin_recv=false;//���Կ�ʼ���� 
bool waiting=false; //�ȴ����� 
string temp;

int seq_num=1;//���


//���ͱ���
void send_to(char *message){
	udp_message u=udp_message(SERVER_PORT,seq_num,bufferSize-10,0,message);//�����udp 
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


//�����ļ�
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


bool begin_timer=false;
double dt;
clock_t start,end;
double overtime=1000;//��ʱʱ�� 
//�Ȱ��ļ�����洢��v�У�������Сһ��
void send_file_2(string path){
	FILE *fin=fopen(path.c_str(),"rb");
	char message[4096];
	while(!feof(fin))
	{
		end=clock();
		dt = (double)(end - start);
		if(waiting==true&&dt>=overtime){
			cout<<"timeout="<<dt<<endl; 
			send_to(message);//��ʱ�ش�
			cout<<seq_num<<' '<<strlen(message)<<endl;
			start=clock(); 
			continue;
		}
		//�����ڲ���ͣ��״̬ʱ�������ͱ��� 
		if(waiting==false)
		{
			fread(message,1,sizeof(message)-10,fin);
			
			seq_num=1-seq_num; //�µı��ģ���ű仯
			 
			send_to(message);
			cout<<seq_num<<' '<<strlen(message)<<endl;
			start=clock();  
			waiting=true;
		}
		begin_recv=true;//***���Կ�ʼ���շ���������Ϣ��***
	}
	cout<<"finish"<<endl;
	fclose(fin);
} 
double space=1859130;
void send_test(){
 	send_file_2("C:\\Users\\Mika\\Desktop\\���������\\����ҵ3\\����1�����ļ�\\1.jpg");
 //   send_file_2("C:\\Users\\Mika\\Desktop\\���������\\����ҵ3\\����1�����ļ�\\2.jpg");
  //  send_file_2("C:\\Users\\Mika\\Desktop\\���������\\����ҵ3\\����1�����ļ�\\3.jpg");
  // send_file_2("C:\\Users\\Mika\\Desktop\\���������\\����ҵ3\\����1�����ļ�\\helloworld.txt");
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
	
	clock_t all_start=clock();
	send_test();
	clock_t all_end=clock();
	double all_pass=(double)(all_end - all_start);
	cout<<"����ʱ�䣺"<<all_pass<<"ms"<<endl; 
	cout<<"�����ʣ�"<<space/all_pass<<"KB/s"<<endl;
	Sleep(10000);//��������ACK 
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
		
//		udp_message m=udp_message(server_port,seq_num_2,length,check_sum,message);
//		m.print();
		if(GET_WHOLE(msg[2],msg[3])==seq_num){//У�����ȷ�������ȷ�����Է�����һ��
			waiting=false;
			cout<<">>>";
		}
		else{//�����ȴ���ȷACK������ʱ�ش�
			waiting=true; 
		}
	} 
}


