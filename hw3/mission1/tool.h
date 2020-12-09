#include<stdio.h>
#include<string.h>
#include<iostream>
#include<vector>
#include<sstream> 
#include<algorithm>
using namespace std;
const int bufferSize=4096;

//С���ߣ�������βλ�û�ȡ�Ӵ� 
string get_substr(string s,int begin,int end){
	return s.substr(begin,end-begin+1);
}
//����תstring 
string to_string(unsigned long num){
	stringstream ss;
	string str;
	ss<<num;
	ss>>str;
	return str;
}

string int2bin(int num,int bit_num){//����ת������
	string s="";
	while(num>0){
		s+=to_string(num%2);
		num/=2;
	}
	reverse(s.begin(), s.end());
	int n=s.size();
	//��0
	for(int i=0;i<bit_num-n;++i){
		s="0"+s;
	}
	return s;
}
string str2bin(string s){
	string r="";
	for(int i=0;i<s.size();++i){
		unsigned char c=s[i];
		r+=int2bin((int)c,8);//��charתΪ8λ��������asc
	}
	return r;
}
int bin2dec(string b){//16λ��������תΪ10����
	int n=b.size();
	int d=0;
	for(int i=0;i<n;++i){
		d=d*2+b[i]-'0';
	}
	//cout<<d<<endl;
	return d;
}
struct udp_message{
	int server_port;//�˿ں�
	int seq_num;//���
	int length;//���Ķζ����Ƴ���
	int check_sum;//У���
	string message;//���Ķ�
	udp_message(){}
	udp_message(int server_port,int seq_num,int length,int check_sum,string message):
		server_port(server_port),
		seq_num(seq_num),
		length(length),
		check_sum(check_sum),
		message(message){}
	void print(){
		printf("server_port:%d\nseq_num:%d\nlength:%d\ncheck_sum:%d\nmessage:%s\n",
			server_port,seq_num,length,check_sum,message.c_str());
	}
	//�����ı��ģ������ƴ�
	string real_message(){
		string s_p=int2bin(server_port,16);
		//ע�⣺�����ԣ����Ҳ������16λ������ʹ��16λ��У��Ͳ����п������Ͳ���ȫΪ1��
		string s_n=int2bin(seq_num,16);
		string l=int2bin(length,16);
		string c_s=int2bin(check_sum,16);
		string m=str2bin(message);
		string need0="";

		string result=s_p+s_n+l+c_s+m;
		int need_zero=16-result.size()%16;
		for(int i=0;i<need_zero;++i){
			need0+='0';
		}
		result+=need0;

		c_s=checksum(result);
		result.replace(48,16,c_s);//��У����������
		//cout<<s_p<<"/"<<s_n<<"/"<<l<<"/"<<c_s<<"/"<<m<<endl;
		return result;
	}
	//����У��ͣ�ÿ16λתΪ10���ƣ�Ȼ�����ȡ��
	string checksum(string s){
		unsigned long sum=0;
		int n=s.size();
		for(int i=0;i<n;i+=16){
			sum+=bin2dec(s.substr(i,16));
			sum=(sum>>16)+(sum&0xffff);
			//��16λ�����Լ��Ľ�λ���֣��൱�ڻؾ�
			//�ö����ƾ�������Ϊ�����Ϊ111+111=1110=1111-1�����Լ��Ͻ�λ��϶������ٽ�λ�� 
		}
		string sum_bin=int2bin(sum,16);//��У���תΪ������
		//��λȡ��
		string result="";
		for(int i=0;i<16;++i){
			if(sum_bin[i]=='0'){
				result+="1";
			}
			else{
				result+="0";
			}
		}
		return result;
	} 
};
