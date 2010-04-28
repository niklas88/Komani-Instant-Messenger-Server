#include <iostream>
#include <unistd.h>
#include <string>
#include <fstream>
#include <list>
#include "TCPNet.h"
#include "askpass.h"
#include "Mutex.h"

namespace globals{
	Mutex locker;
	std::list<std::string> received;
	
	std::list<CSocket*> connections;
		
}



bool Login(CSocket& conn, std::string& Name, std::string& Passwort,UINT8 status =0){
return 	(
	conn.send((char*)&"LL",2,0)>0 &&
    SendStringWBLen(&conn,Name)>0  &&
	SendStringWBLen(&conn,Passwort)>0 &&
	conn.send((char*)&status,sizeof(status),0)>0
    );
}

bool SendMessage(CSocket& conn,unsigned int ID, std::string Message){
	
	unsigned short int length=Message.length();
	
	ID=htonl(ID);
	length=htons(length);
	return (conn.send((char*)&"MS",2,0) &&
			conn.send((char*)&ID,sizeof(ID),0)>0 &&
			conn.send((char*)&length,sizeof(length),0)>0 &&
			conn.send(Message.data(),ntohs(length),0)>0);//Be carfull for NetByteOrder
	
}

void* RecvHandler(void* Socket){

	CSocket* conn=(CSocket*) Socket;
	/*
    UINT8 data=0;
	for(;;){
		if(conn->recv((char*)&data,sizeof(data),MSG_WAITALL)<=0) return 0;
		std::ofstream oufile ("out.txt", std::ofstream::app);

		oufile.write((char*)&data,sizeof(data));
		oufile.close();
	}
	
	*/
	UINT16 Befehl=0; 
	while(!(conn->recv((char*)&Befehl,sizeof(Befehl),MSG_WAITALL)<=0)){
	//globals::locker.Lock();	
		if      (!strncmp((char*)&Befehl,"LO",2))  globals::received.push_back("Login OK");		
		else if (!strncmp((char*)&Befehl,"LP",2))  globals::received.push_back("Wrong Password");
		else if (!strncmp((char*)&Befehl,"LU",2))  globals::received.push_back("Username not found");
		else if (!strncmp((char*)&Befehl,"RM",2))  {	
			//globals::received.push_back("Message receiving");
			unsigned int ID;
			unsigned short int length;
			std::string Message;
			if(conn->recv((char*)&ID,sizeof(ID),MSG_WAITALL)<=0 ||
			   conn->recv((char*)&length,sizeof(length),MSG_WAITALL)<=0 ||
			   conn->recv(Message,ntohs(length))<=0) { //beware of NetByteOrder
				  globals::locker.Unlock();
				  return 0;
				  }
			globals::received.push_back("Message: "+Message);
		}
		else if (!strncmp((char*)&Befehl,"AC",2))  {
			
			signed char CET;
			unsigned int ID;
			std::string Nick;
			globals::received.push_back("receiving owninformation");
			
			if(RecvStringWBLen(conn,Nick)<=0) {
				  globals::locker.Unlock();
				  return 0;		
				  }
			globals::received.push_back("Nickname: "+Nick);	  
			if(conn->recv((char*)&CET,sizeof(CET),MSG_WAITALL)<=0){
				globals::locker.Unlock();
			}
			globals::received.push_back("CET received.");
			if(conn->recv((char*)&ID,sizeof(ID),MSG_WAITALL)<=0){
				globals::locker.Unlock();
			}
			ID=ntohl(ID);
			globals::received.push_back("OwnInfomation received");
		}
		else if (!strncmp((char*)&Befehl,"RS",2))  {
				char Status;
				unsigned int ID;
				std::string Username;
				if(conn->recv((char*)&ID,sizeof(ID),MSG_WAITALL)<=0 ||
				   conn->recv((char*)&Status,sizeof(Status),MSG_WAITALL)<=0) {
					globals::locker.Unlock();
					return 0;		
			        }
				ID=ntohl(ID);
			globals::received.push_back("StatusChange received");
		}
		else if (!strncmp((char*)&Befehl,"AL",2))  {
			unsigned int KontaktID;
			char Gruppe;
			std::string Username;
			std::string Nickname;
			char CET;
			char Status=0;	
					
			if(RecvStringWBLen(conn, Username)<=0 ||
			   RecvStringWBLen(conn, Nickname)<=0 ||
			   conn->recv((char*)&Status,sizeof(Status),MSG_WAITALL)<=0 ||
			   conn->recv((char*)&Gruppe,sizeof(Gruppe),MSG_WAITALL)<=0 ||
			   conn->recv(&CET,sizeof(CET),MSG_WAITALL)<=0 ||
			   conn->recv((char*)&KontaktID,sizeof(KontaktID),0)<=0){
				globals::locker.Unlock();
				return 0;
			}
			KontaktID=ntohl(KontaktID);
			globals::received.push_back("UserInformation received");
		}
		else globals::received.push_back("Befehl: "+std::string((char*)&Befehl,2));
	globals::locker.Unlock();
	}
	
	
	return 0;
}

int main(unsigned int argc, char* args[]){
/*	
	if(argc<2){
		std::cout<<"Error: Too few arguments"<<std::endl;
		return 0;
	}
*/	
	std::string Name="";
	std::string Passwort="";
	
	for(unsigned int i=0;i<argc;i++){
		//std::cout<<"args["<<i<<"] "<<args[i]<<std::endl;	
	 if (!strncmp(args[i],"-n",2)) {
	 	 //Does not need to be evaluated	
		 i++;
		 Name=args[i];
		
	 }
	 else if(!strncmp(args[i],"-p",2)) {
 		//Does not need to be evaluated		
		 i++;
	 	Passwort=args[i];
	 }

	}	
	if (Name=="" ) {
		std::cout<<"Name: ";
		std::cin>>Name;
		std::cout<<std::endl;
	}
	if (Passwort=="" ) {
		
		char* buf=new char[256];
		askpass::tty_readpass("Password: ",buf,256);
		Passwort.append(buf,strchr(buf,'\n'));//newline entfernen POSSIBLE BUG HERE
		delete[] buf;
	}

	
	CSocket conn("sceneproject.org",6969);
/*	
	for(unsigned int num=0;num<=100; num++ ){
		globals::connections.push_back(new CSocket("sceneproject.org",6969));		
	}
*/
	Login(conn,Name,Passwort);
	
	pthread_t ThreadID=0;
	pthread_create(&ThreadID, 0, &RecvHandler, (void*)&conn);
	//sleep(10);
	
	
  
std::string input="";	
	while(true){
		std::cout<<"kimc: ";
		std::cin>>input;
		std::string command=input.substr(0,input.find(" ",0));

		if (command=="end" || command=="quit" || command=="bye" || command=="exit") {
			break;			
		}
		else if(command=="test") std::cout<<"Weeeh\n";	
		else if(command=="msg") {
			std::string Message="";
			unsigned int ID;
			std::cout<<"ID: ";std::cin>>ID;std::cout<<std::endl;
			std::cout<<"Message: ";std::cin>>Message;std::cout<<std::endl;
			SendMessage(conn,ID,Message);
		 

		}
		else if(command=="show"){
			globals::locker.Lock();
			std::list<std::string>::iterator it;
			for ( it=globals::received.begin();it!=globals::received.end();++it) {
				std::cout<<(*it)<<std::endl;
				//globals::received.erase(it); 
			}
			globals::locker.Unlock();
		}
			
		
	}
/*	
	pthread_cancel(ThreadID);
	std::list<CSocket*>::iterator it;
	for ( it=globals::connections.begin();it!=globals::connections.end();++it) {
		delete *it;
		globals::connections.erase(it); 
	}

*/
return 0;
}

