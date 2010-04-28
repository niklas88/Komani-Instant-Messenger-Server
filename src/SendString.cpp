#include "../include/TCPNet.h"

int  SendStringWBLen(CSocket* Socket,const std::string& String){
	unsigned char length=String.length();//htonl(String.length());
	std::cout<<"Entering SendStringWBLen"<<std::endl;
	int ret =  Socket->send((char*)&length,sizeof(length),0);
	std::cout<<"SendStringWBLen len="<<(unsigned int)length<<std::endl;
	ret     += Socket->send(String);
	std::cout<<"SendStringWBLen sent="<<ret<<std::endl;
	
	return ret;
}
