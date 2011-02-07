/*# Komani Instant Messenger Server
# Copyright (C) 2010 Niklas Schnelle
# Copyright (C) 2010 Konstantin Weitz
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/
#include <string>
#include <utility>
#include "../include/user.h"
#include "../include/str.h"
#define Debug

typedef unsigned char	UINT8;
typedef signed char		INT8;
typedef unsigned short	UINT16;
typedef signed short	INT16;
typedef unsigned int	UINT32;
typedef signed int		INT32;
typedef unsigned long long UINT64;
typedef signed long long INT64;
typedef unsigned char	BYTE;

pthread_attr_t tattr; //Thread attribute to use everywhere

/****************** Kleiner Anwendungstest mit unseren Socket Klassen*********************/
inline bool RegisterAntwort(CSocket* Socket,char flag){
return (Socket->send((char*)&"AR",2,0)>=0&&
Socket->send(&flag,sizeof(flag),0)>=0) ;
}

inline bool LoginAntwort(CSocket* Socket,char flag){
return (Socket->send((char*)&"L",1,0)>=0&&
Socket->send(&flag,sizeof(flag),0)>=0) ;
}


inline unsigned int AddUserToDB(Query& q,CSocket* Socket,std::string& Name,std::string& Nickname,std::string& Passwort,signed char CET){
			if(!q.execute("INSERT INTO  user (Name, Nick, Passwort,Zeitzone) VALUES('"+q.safestr(Name)+"','"+q.safestr(Nickname)+"','"+q.safestr(Passwort)+"','"+	 str( CET,  10 ) +"')")) {
				RegisterAntwort(Socket,'S');
				std::cout<< "User bereits in DB!"<<q.GetError()<<"\n";
				return 0;	
			    }
return q.get_count("select ID from user where Name='"+q.safestr(Name)+"'");
}

inline unsigned int IsUserInDB(Query& q,CSocket* Socket,std::string& Name, std::string& Passwort){
	
	unsigned int ID=q.get_count("select ID from user where Name='"+q.safestr(Name)+"'");
	if(ID<=0) {
		LoginAntwort(Socket,'U');
#ifdef Debug
		std::cout<<"Login failed User not in DB\n";
		std::cout<<"Username: ("<<q.safestr(Name)<<")"<<std::endl;		
#endif			
		std::cout<< q.GetError()<<"\n";
		return 0;
	}
	else {
		if (!q.get_count("Select ID from user where ID='"+str(ID,10)+"' and Passwort='"+q.safestr(Passwort)+"'")) {
			LoginAntwort(Socket,'P');

#ifdef Debug
			std::cout<<"Login failed User wrong Password\n";
			std::cout<<"Username: ("<<q.safestr(Name)<<")"<<std::endl;		
			std::cout<<"Wrong Password: ("<<  q.safestr(Passwort)<<")"<<std::endl;
#endif
			return 0;
		}
		
	}
	return ID;
}


User*  Auth(CSocket* Socket){	
	//Get the Message
	UINT16 Befehl;
	Socket->recv((char*)&Befehl,sizeof(Befehl));
	Befehl=ntohs(Befehl);
	
	switch(Befehl){
		case LOGIN:
		{
#ifdef Debug			
		  std::cout<<"Login\n";
#endif		 
		  UINT8 Status;	
		  std::string Name="";
		  std::string Passwort="";	
			
		    //Empfangen
			if(!RecvStringWBLen(Socket,Name)|
			!RecvStringWBLen(Socket,Passwort)||			
			Socket->recv((char*)&Status,sizeof(Status),MSG_WAITALL)<=0) 
			{
				std::cout<<"Empfangsfehler beim einloggen"<<std::endl;
				return 0;
			}		
		
		  //Jetzt Das Datenbankzeug	
          Query q(Backend::db);  
          unsigned int ID=IsUserInDB(q,Socket,Name,Passwort);
          if(!ID) return 0;			
		
	  char CET=q.get_count("select Zeitzone from user where ID='"+str(ID,10)+"'");
	  //Login bestŠtigen	
	  LoginAntwort(Socket,'O');
	  return  new User(Name, q.get_string("select Nick from user where ID='"+str(ID,10)+"'"),ID, Socket,Status,CET);
		} 
		case REGISTER:	
		{
#ifdef Debug
			std::cout<<"Register\n";
#endif
			INT8 CET;		
			std::string Name="";
			std::string Nickname="";
			std::string Passwort="";
			
			if(!RecvStringWBLen(Socket,Name)||!RecvStringWBLen(Socket,Nickname)||!RecvStringWBLen(Socket,Passwort))
			
			{
				std::cout<<"Empfangsfehler beim registrieren"<<std::endl;
				return 0;
			}
			//Datenbankzeug
			Query q(Backend::db);		
			UINT32 ID=AddUserToDB(q,Socket, Name,Nickname,Passwort, CET);	
            	if(!ID)
					return 0;
			
			//Register bestŠtigen
        	RegisterAntwort(Socket,'O'); 			
			std::cout<<"ID: "<<ID<<endl;
	        return new  User(Name,Nickname,ID,Socket,1,CET);
		}
		default:
			return 0;
	}
     
  return 0;
}

inline bool SendChangeStatus(unsigned int EmpfaengerID,unsigned int ChangerID,char Status)
{
	bool bRet;
	User* Client=GetFromList(EmpfaengerID);
	bRet = (Client != NULL);
	if(bRet)
	{
		ChangerID=htonl(ChangerID);
		
		Client->Lock();            
		bRet= (Client->send((char*)&"RS",2,0)>0||
			   Client->send((char*)&ChangerID,sizeof(ChangerID),0)>0||    
			   Client->send((char*)&Status,sizeof(Status),0)>0) ;
		Client->Unlock();
	}
	return bRet;
}

void  SendStatus(User* ListenBesitzer, char Status){
	Query q(Backend::db);	
	unsigned int ID=ListenBesitzer->GetID();
	q.get_result("select USER, from kontaktlisten where EINTRAG='"+str(ID,10)+"'");
	while (q.fetch_row()){
	 //Bei jedem User bei dem Client geadded ist Clients Status melden	
     SendChangeStatus(q.getval(),ID,Status);
	}
	q.free_result();
}

inline bool SendContactInformation(User* Client,unsigned int KontaktID,char Gruppe){	
std::string Username;
std::string Nickname;
char CET;
char Status=0;		

User* Kontakt=GetFromList(KontaktID);
     if(Kontakt) {
		 Kontakt->Lock();
		 	Status=Kontakt->GetStatus();	
		 	Username=Kontakt->GetName();
		 	Nickname=Kontakt->GetNick();
         	CET=Kontakt->GetCET();	
		 Kontakt->Unlock();
		 }
	 else{	
	     Query q(Backend::db);
	     q.get_result("select Name,Nick,Zeitzone from user where ID='"+str(KontaktID,10)+"'");
	     if(q.fetch_row()){
	      Username=q.getstr();	
	      Nickname=q.getstr();
	      CET=q.getval();	
	     }
		 else {
		  q.free_result();
		  return 0;	 
		 };
	     q.free_result();	
	 }
	
//std::cout<<"Client found: "<<KontaktID<<"  : "<<Username<<" "<<Nickname<<"\n";		
KontaktID=htonl(KontaktID);
Client->Lock();	 
bool _b=(Client->send((char*)&"AL",2,0)>0  &&
 Client->sendStrWBLen(Username)>0 &&
 Client->sendStrWBLen(Nickname)>0&&
 Client->send(&Status,sizeof(Status),0)>0&&
 Client->send(&Gruppe,sizeof(Gruppe),0)>0&&
 Client->send(&CET,sizeof(CET),0)>0&&
 Client->send((char*)&KontaktID,sizeof(KontaktID),0)>0) ;
Client->Unlock();	 
return _b;
}

inline bool SendOwnInformation(User* Client){
signed char CET=Client->GetCET();	
unsigned int ID=Client->GetID();
ID=htonl(ID);
 #ifdef Debug
 std::cout<<"Sending own Information for: "<<Client->GetName()<<std::endl;
 #endif

Client->Lock();	
bool _b=(Client->send((char*)&"AC",2,0)>0 );
#ifdef Debug
 std::cout<<"SendingownInformation: AC sent with: "<<_b<<std::endl;
#endif
_b= (Client->sendStrWBLen(Client->GetNick())>0);
#ifdef Debug
 std::cout<<"SendingownInformation: sendStrWBLen sent with: "<<_b<<std::endl;
#endif
_b= (Client->send((char*)&CET,sizeof(CET),0)>0&&
 Client->send((char*)&ID,sizeof(ID),0)>0);
Client->Unlock();
#ifdef Debug
	std::cout<<"OwnInformation sent"<<std::endl;
#endif
return _b;
} 

void SendContacts(User* Client){
Query q(Backend::db);
SendOwnInformation(Client);//Eigene Daten als ersten Eintrag senden	
	
	q.get_result("select EINTRAG,GRUPPE from kontaktlisten where USER='"+str(Client->GetID(),10)+"'");
	while (q.fetch_row())
	{
		//Von jedem User der bei Client geadded ist daten heraussuchen und an Client senden
#ifdef Debug
		std::cout<<"Reading contactlist \n";
#endif
		unsigned int ID=q.getval();
		char Group=q.getval();
		SendContactInformation(Client,ID,Group);		
	}	
	q.free_result();
}

inline bool AddUserToList(User* Client){
	User oClient=User(*Client);
	Backend::ListenMutex.Lock();
	//Bool gibt an ob User eingefügt werden konnte
    	bool _b=Backend::Userliste.insert(pair<unsigned int, User>(Client->GetID(),oClient)).second;
	Backend::ListenMutex.Unlock();	  
return _b;		
}

void* ClientThread(void* MyCSocket){
std::cout<<"New User  called\n";	
CSocket*  MyConn=(CSocket*) MyCSocket;
	
User* Client=Auth(MyConn);  

	
if(Client) {
#ifdef Debug
	std::cout<<"Authenthication correct!\n";
#endif
    MyConn=0;//Wilden Zeiger sichern

		if(AddUserToList(Client)){
			std::cout<<*Client;
			
			SendContacts(Client); //Kontaktliste senden...
			SendStatus(Client,Client->GetStatus());
			
			
			Client->Act();
			
			SendStatus(Client,0);//Als abgemeldet melden
				
			Backend::ListenMutex.Lock();
			 Backend::Userliste.erase(Client->GetID());
			Backend::ListenMutex.Unlock();
		}

    delete Client;
	return 0; //Der Client closes the Connection im DTOR
 }

delete MyConn;
return 0;
}



bool NewUser(CSocket* Client){

pthread_t MyThread;	
pthread_create(&MyThread, &tattr, &ClientThread, (void*) Client);		 
return 1;
}    







int main( int argc, char ** argv){
 int i;
	pthread_attr_init(&tattr); 
	pthread_attr_setstacksize(&tattr,  2 * PTHREAD_STACK_MIN); 
	pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);

 //Erzeuge Server mit Port und Loginfunktionspointer
SSocket Server(6969,&NewUser);


	
std::cin >> i;
	return 0;
}




