#include<stdio.h>
#include<string.h>
#include<iostream> 
#include<algorithm>
#include<time.h>
#include<cstdlib>
using namespace std;

#define GET_HIGH(number) ((int)number&0xFF00)>>8
#define GET_LOW(number) ((int)number&0x00FF)
#define GET_WHOLE(h,l) ((((int)h<<8)&0xff00)|((int)l&0xff))
const int bufferSize=4096;

struct udp_message{
	int server_port;//�˿ں�
	int seq_num;//���
	int length;//���Ķζ����Ƴ���
	int check_sum;//У���
	char *message;//���Ķ�
	udp_message(){}
	udp_message(int server_port,int seq_num,int length,int check_sum,char *message):
		server_port(server_port),
		seq_num(seq_num),
		length(length),
		check_sum(check_sum),
		message(message){}
	void print(){
		printf("server_port:%d\nseq_num:%d\nlength:%d\ncheck_sum:%d\nmessage:%s\n",
			server_port,seq_num,length,check_sum,message);
	}
	//�����ı��ģ������ƴ������浽msg�� 
	void real_message(char *msg){
		//��Ĳ���ÿһλ�����ǰ�8λ��Ϊһ���ֽڣ��浽char��һ����λ�
		msg[0]=GET_HIGH(server_port);
		msg[1]=GET_LOW(server_port);
		msg[2]=GET_HIGH(seq_num);
		msg[3]=GET_LOW(seq_num);
		msg[4]=GET_HIGH(length);
		msg[5]=GET_LOW(length);
		msg[6]=0;
		msg[7]=0;
		for(int i=0;i<length;++i){
			msg[8+i]=message[i];
		}

		int a=checksum(msg);
		msg[6]=GET_HIGH(a);
		msg[7]=GET_LOW(a);
	}
	//����У��ͣ�ÿ16λתΪ10���ƣ�Ȼ�����ȡ��
	int checksum(char *msg){
		unsigned long sum=0;
		for(int i=0;i<8+length;i+=2){//���ﲻ����strlen(msg)����Ϊ����м���0�������Ͳ�������
			sum+=GET_WHOLE(msg[i],msg[i+1]);
			sum=(sum>>16)+(sum&0xffff);
			//��16λ�����Լ��Ľ�λ���֣��൱�ڻؾ�
			//�ö����ƾ�������Ϊ�����Ϊ111+111=1110=1111-1�����Լ��Ͻ�λ��϶������ٽ�λ�� 
		}
		return (~sum)&0xffff;
	} 
};

//����udp����msg�����ر��ĶΣ�д��message
bool analyse(char *msg,char *message){
	int server_port=GET_WHOLE(msg[0],msg[1]);
	int seq_num=GET_WHOLE(msg[2],msg[3]);
	int length=GET_WHOLE(msg[4],msg[5]);
	int check_sum=GET_WHOLE(msg[6],msg[7]);
	for(int i=0;i<length;i++){
		message[i]=msg[8+i];
	}
//	udp_message m=udp_message(server_port,seq_num,length,check_sum,message);
//	m.print();

	int sum=0;
	for(int i=0;i<length+8;i+=2){
		sum+=GET_WHOLE(msg[i],msg[i+1]);
		sum=(sum>>16)+(sum&0xffff);
	}
	if(sum==0xffff){//У�����ȷ���ظ�ACK
		cout<<"yes"<<endl;
		return true;
	}
	else{
		cout<<"no"<<endl;
		return false;
	}
}

bool random_loss(){
	srand((int)time(0));
	int r=rand()%100;
	if(r==99){
		return false;
	}
	return true;
}
