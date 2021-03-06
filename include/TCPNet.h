/*Simple Netzwerkklassen (nahezu Platformunabhaengig)
  Copyright 2006 Niklas Schnelle und KoMaNi Software
   -Nicht ohne Erlaubins oben genannter fuer kommerzielle Projekte
    zugelassen.
  Anmerkung des Autors:
            Dies ist mein erster sinvoller C++ Code also is vielleicht
            nicht �berall Perfekt. 
*/

//
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include "mbuffer.h"

#ifdef _WIN32
/* Headerfiles f�r Windows */
#include <winsock.h>
#include <io.h>
#include <windows.h>

#else
/* Headerfiles f�r Unix/Linux */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
 #include <arpa/inet.h>
 #include <pthread.h>
 #include <errno.h>
#define closesocket(s) close(s)
#endif

/* http requests werden normalerweise auf Port 80 
 * vom Server entgegengenommen */


/****************** MAIN *********************/

class CSocket{ 
public:
	explicit CSocket(const char* Address,short Port, unsigned int BufferLength =8192);
	explicit CSocket(int neusock, sockaddr_in& Addresse, unsigned int BufferLength =8192);
	virtual ~CSocket();
	
	      //send muss schnell sein daher inline
          inline int  send(const char* Buff,unsigned int length, int flag ) const;
          int  send(const MBuffer& Buff)const;
          int  send(const std::string& String) const;
		  inline int  recv(char* Buffer,unsigned int length, int flag =0);		
          int  recv(MBuffer& ,unsigned int);
          int  recv(std::string& String,unsigned int DataLength);
private:   
			  
		//Variablen fuer den Socket
		sockaddr_in  server;
		int sock;          	  
        
		unsigned int MaxBufferLength;
        char* Buffer;
        
		 

};

inline int  CSocket::send(const char* Buff,unsigned int length, int flag ) const{
int _b=::send( sock, Buff,length, flag); 
if(_b<0) std::cout<<"raw send returned:"<<_b<<std::endl;
return _b;
} 
 
inline int CSocket::recv(char* Buff,unsigned int length, int flag ){
return ::recv( sock, Buff, length, flag);
 }

 inline bool RecvStringWBLen(CSocket* Socket,std::string& Out){
    unsigned char length;	
    bool _b;
   
	_b= Socket->recv((char*)&length,sizeof(length),MSG_WAITALL)>0;
	return Socket->recv(Out, length)>0 && _b;
			
}

int  SendStringWBLen(CSocket* Socket,const std::string& String);

class SSocket {
public:
         SSocket(short,bool (*func)(CSocket*));
         //SSocket(short);
         virtual     ~SSocket();                 
         //muss leider public sein da sonst nich Threadbar     
         int                 myAcceptThread();   
          
protected:               
         pthread_t               ThreadID;
         //Pointer zur Loginfunktion (sie wird gecalled sobald jemand connected)
         bool                (*loginfunc)(CSocket*); 
                
         //Die Variablen fuer den Socket
         sockaddr_in         server;
         //Der ServerSocket
         int                 sock;    

                       
};
