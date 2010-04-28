#include "TCPNet.h"
#include <map>
#include <mysql/mysql.h>
#include "Mutex.h"
#include "Database.h"
#include "Query.h"
#include "str.h"
#include "Protodefines.h"

#ifndef _USERHPP
#define _USERHPP
using namespace std;

class User;

namespace Backend{
	extern Mutex DatabaseMutex;
	extern Database db;
	extern map<unsigned int,User>  Userliste;
    extern Mutex ListenMutex;
}

class User{
	public: 
         User(string NewName,string NewNick,unsigned int NewID ,CSocket* NewConn, char NewStatus, char NewZeitzone);
	     User( const User& _rCpy );
	     virtual ~User();
         User	operator =( const User& _rCpy );

	    

	    bool Act();
	    bool Lock(){return mpCont->Lock.Lock();}
		bool Unlock(){return mpCont->Lock.Unlock();}
	
	    inline int  send(const MBuffer& Buff);
        inline int  send(const char* Buffer,unsigned int length, int flag );	
	    inline int  send(const std::string& String);
	    int  sendStrWBLen(const std::string& String);
	
	    const string GetName() ;
	    const string GetNick() ;
	    const unsigned int GetID() const {return mpCont->ID;}
		const char GetStatus()const {return mpCont->status;}
	    const char GetCET()const {return mpCont->Zeitzone;}
		
	    bool IsName(string&) ;
	    bool IsID(unsigned int);

	private:	
     struct Cont
      {	
		int	volatile	mnRefCnt;
		Cont( void )	{ mnRefCnt = 1; }
	
		//***********************************
			Mutex Lock;
        	unsigned int ID;
	    	string name;
	    	string nick;	
        	CSocket* conn;	
   	    	char status;
	    	char Zeitzone;
	    	//Query myquery;            		
	 };		
		
	 Cont*	mpCont; //Wichtig
	 
	 //void SetName();
	 //void SetNick();
	 //void SetCET();
	

friend ostream& operator<< (ostream &os,  User& us);	
};




inline int User::send(const MBuffer& Buff){

return mpCont->conn->send(Buff);
}	

inline int User::send(const char* Buff,unsigned int length, int flag){

return mpCont->conn->send(Buff,length,flag);
}

inline int  User::send(const std::string& String ){

	return mpCont->conn->send(String);
}






inline User* GetFromList(unsigned int ID){
Backend::ListenMutex.Lock();
    map<unsigned int,User>::iterator p=Backend::Userliste.find(ID);	
	    if(p==Backend::Userliste.end()) {
		    Backend::ListenMutex.Unlock();
		
			return 0;
		}			
    User* Client=new User(p->second);
Backend::ListenMutex.Unlock();	  
return Client;		
}

inline unsigned int GetUserID(Query& q,std::string Name){
	return q.get_count("Select ID From user where Name='"+q.safestr(Name)+"'");	
}
#endif


