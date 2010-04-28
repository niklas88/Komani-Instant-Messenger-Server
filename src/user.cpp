#include "../include/user.h"
 
 namespace Backend{
	Mutex ListenMutex;
	Mutex DatabaseMutex;
	Database db(DatabaseMutex,"localhost",
		"niklas",
		"hogwarts",
		"komani2");
    std::map<unsigned int, User>  Userliste;
    
}

 
 User::User(string NewName,string NewNick,unsigned int NewID,CSocket* NewConn, char NewStatus,char NewZeitzone)
:mpCont(new Cont)
 {
	// std::cout<<"ctor User"<<endl;
	mpCont->name=NewName;
	mpCont->nick=NewNick;
	mpCont->conn=NewConn; 
	mpCont->status=NewStatus;
	mpCont->Zeitzone=NewZeitzone;
	mpCont->ID=NewID;
 
 }
 
User::User( const User& _rCpy )
{	// now it's gonna be tricky...
	User*		pCpy = ( User* ) &_rCpy;
	mpCont = pCpy->mpCont;
	mpCont->Lock.Lock();
	  mpCont->mnRefCnt++; // ULTRA KRITISCHE STELLE!!! MIGHT NOT WORK!!!! POSSIBLE BUG HERE
   	mpCont->Lock.Unlock();
}
 
User User::operator =( const User& _rCpy )
{
	return User( _rCpy );
}
 
 User::~User()
{
	mpCont->Lock.Lock(); 
	 mpCont->mnRefCnt--;// ULTRA KRITISCHE STELLE!!! MIGHT NOT WORK!!!! POSSIBLE BUG HERE
	 if( !mpCont->mnRefCnt ){		// terminating dtor reached!
		mpCont->Lock.Unlock();
        	delete mpCont;
	}
	else  mpCont->Lock.Unlock();
}
 
 
const string  User::GetName() 
{

	const string  rueck(mpCont->name.data(),mpCont->name.length());

	
	return rueck;
}

const string  User::GetNick(){

	     const string rueck(mpCont->nick.data(),mpCont->nick.length());

	return rueck;
}

bool User::IsName(string& Name2)  {

	  bool _b=(mpCont->name==Name2);

	return _b;
}

 bool User::IsID(unsigned int ID2){

	 bool _b=(mpCont->ID==ID2);

  return _b;
}	 

int  User::sendStrWBLen(const std::string& String){

	return SendStringWBLen(mpCont->conn,String);
}



inline bool SendMessage(unsigned int ID,MBuffer& Nachricht, unsigned int SenderID){

			 User* Empfaenger=GetFromList(ID);
			 if(!Empfaenger){
			  std::cout<<"GetFromList failed!"<<std::endl;
			  return 0;
			 }
	unsigned short int length=Nachricht.GetLen(); 	
	length=htons(length);
	SenderID=htonl(SenderID);
	Empfaenger->Lock();		 
	   bool _b=(Empfaenger->send((char*)&"RM",2,0)>0&&
	 	 Empfaenger->send((char*)&SenderID,sizeof(SenderID),0)>0&&	
		 Empfaenger->send((char*)&length,sizeof(length),0)>0 &&
		 Empfaenger->send(Nachricht)>0 ) ;
	Empfaenger->Unlock();
	delete Empfaenger;
			 
	return _b;		 
}


inline bool SendAddQuestion(Query& q,std::string& Name,std::string& Bitte,unsigned int SenderID){
	User* Empfaenger=GetFromList(GetUserID(q,Name));
	SenderID=htonl(SenderID);
	if(!Empfaenger)return 0;
	Empfaenger->Lock();			 
	 bool _b= (Empfaenger->send((char*)&"QU",2,0)<=0 ||
			   Empfaenger->send((char*)&SenderID,sizeof(SenderID),0)<=0||
			   !Empfaenger->send(Bitte));				 
	Empfaenger->Unlock();
return _b;
}


inline bool AddToContactList(Query& q,unsigned int Kontakt, unsigned int ListenBesitzer){
return	q.execute("insert into kontaktlisten(USER, EINTRAG) values("+str(ListenBesitzer,10)+","+str(Kontakt,10)+")");
}
	
	
bool User::Act(){
	 unsigned short int Befehl=0;
	 unsigned short int length=0;
     unsigned int ID=0;
	 Query myquery(Backend::db);
	 MBuffer Nachricht;
	
	std::cout<<"Act\n";
	for(;;)
	{
		if(mpCont->conn->recv((char*)&Befehl,sizeof(Befehl),MSG_WAITALL)<=0) return 0;
		//Befehl=ntohs(Befehl);
		switch(Befehl){
			
		case MESSAGE:			

		ID=0;
		length=0;
		    if(mpCont->conn->recv((char*)&ID,sizeof(ID),MSG_WAITALL)<=0||
			mpCont->conn->recv((char*)&length,sizeof(length),MSG_WAITALL)<=0) //length is short int=2 byrtes
				return 0;
			ID=ntohl(ID);
			length=ntohs(length);
			if (mpCont->conn->recv(Nachricht,length)<=0) return 0;	
			
			std::cout<<"MessageReceived"<<endl;
			   
			if(!SendMessage(ID,Nachricht,this->GetID())) std::cout<<"Message couldn't be delivered to: "<<ID<<"\n";
		  	
			break;
			  
		case ADDUSER:{	  			  
			std::string Bitte;
			std::string Name;
			if(!RecvStringWBLen(mpCont->conn, Name)|| 
			   !RecvStringWBLen(mpCont->conn,Bitte))return 0;
			   			
			SendAddQuestion(myquery, Name, Bitte,this->GetID());
		    break;
		}
		case ADDUSEROK:{
		  char Flag;	
		  // Der user *this  will/ will nicht das der User ID ihn adden darf
		  if(mpCont->conn->recv((char*)&ID,sizeof(ID)<=0)||
		   	  mpCont->conn->recv((char*)&Flag, sizeof(Flag))<=0) return 0;
		  ID=ntohl(ID);
		  if(Flag==1) AddToContactList(myquery ,this->GetID(), ID);
		 break;
         }			 
		}	

	
	}
 }
 
 ostream& operator<< (ostream &os,  User &p)
{
    return os <<"Name: "<<p.GetName()<<" \n"<<"Nick: "<<p.GetNick()<<"\n"<<"ID: "<<p.GetID()<<"\n";//Eventuell Lock hier einfŸgen
}
