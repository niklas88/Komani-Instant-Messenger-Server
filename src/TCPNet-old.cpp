#include "TCPNet.h"
//Initialisiere statische Elementvariable
//bool CSocket::inited=0;

CSocket::CSocket (const char *Address, short Port, unsigned int BufferLength):
MaxBufferLength (BufferLength)
{

#ifdef _WIN32
  WSADATA wsaData;
  short wVersionRequested;
  /*
   * Initialisiere TCP für Windows ("winsock") 
   */
  if (!inited)
    {
      wVersionRequested = MAKEWORD (1, 1);
      if (WSAStartup (wVersionRequested, &wsaData) != 0)
	{
	  //fprintf( stderr, "Failed to init windows sockets\n");
	  exit (1);
	}
    }
//fprintf( stderr, "Initialisiert!\n");  
#endif

  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      //      perror( "failed to create socket");
      exit (1);
    }

  /*
   * Erzeuge die Socketadresse des Servers 
   * * Sie besteht aus Typ, IP-Adresse und Portnummer 
   */
  memset (&server, 0, sizeof (server));	//NULLEN

  unsigned long addr;
  hostent *host_info;
  if ((addr = inet_addr (Address)) != INADDR_NONE)
    {
      /*
       * Address ist eine numerische IP-Adresse 
       */

      memcpy (&server.sin_addr, &addr, sizeof (addr));
    }
  else
    {
      /*
       * Wandle den Servernamen in eine IP-Adresse um 
       */

      host_info = gethostbyname (Address);
      if (NULL == host_info)
	{
	  fprintf (stderr, "unknown server: %s\n", Address);
	  exit (1);
	}
      memcpy (&server.sin_addr, host_info->h_addr, host_info->h_length);
    };

  server.sin_family = AF_INET;
  server.sin_port = htons (Port);

  /*
   * Baue die Verbindung zum Server auf 
   */
  if (connect (sock, (const sockaddr *) &server, sizeof (server)) < 0)
    {
      perror ("can't connect to server");
      exit (1);
    }


  Buffer = new char[MaxBufferLength];
}

CSocket::~CSocket ()
{
  closesocket (sock);
  delete[]Buffer;
}

CSocket::CSocket (int neusock, sockaddr_in & Adresse,
		  unsigned int BufferLength):
server (Adresse),
sock (neusock),
MaxBufferLength (BufferLength)
{
  Buffer = new char[MaxBufferLength];
}

int
     CSocket::send (const MBuffer & Buffer, int flag) const const const const const const const const const
     {

       int status;
         status =::send (sock, Buffer.GetBuffer (), Buffer.GetLen (), flag);
       if (status == -1)
	 {
	   switch (errno)
	     {
	     case ECONNRESET:
	       std::cout << "Connection reset by peer\n";

	       default:std::cout << "Socket Error on send\n";

	       return 0;
	     }
	 }

       return status;
     }

int
     CSocket::send (const std::string & String, int flag) const const const const const const const const const
     {
       int status;
         status =::send (sock, String.data (), String.length (), flag);	//Sende OHNE Terminating NULL    
       if (status == -1)
	 {
	   switch (errno)
	     {
	     case ECONNRESET:
	       std::cout << "Connection reset by peer\n";

	       default:std::cout << "Socket Error on send\n";

	       return 0;
	     }
	 }

       return status;
     }

int
CSocket::recv (MBuffer & BEmpfangen, unsigned int DataLength)
{
  unsigned int got = 0;
  BEmpfangen.Clear ();
  DataLength = (DataLength > MaxBufferLength) ? MaxBufferLength : DataLength;
  while (got < DataLength)
    {
      int len = recv (Buffer, DataLength, MSG_WAITALL);
      if (len > 0)
	{
	  got += len;
	  BEmpfangen.Write ((const void *) Buffer, len);
	}
      else if (len == -1)
	{
	  switch (errno)
	    {
	    case ENOTCONN:
	      std::cout << "The Socket is no connected\n";
	      break;
	    default:
	      std::cout << "Socket Error on recv\n";
	      break;
	    }
	  return -1;
	}
      else if (len == 0)
	{
	  std::cout << "The client has disconnected properly\n";
	  return 0;
	}
    }
  if (BEmpfangen.GetLen () != got)
    return 0;			//irschend was is schief gegangen
  BEmpfangen.Seek (0);		// Seek to start to not confuse
  return got;
}

int
CSocket::recv (std::string & String, unsigned int DataLength)
{
  unsigned int got = 0;
  String.clear ();
  DataLength = (DataLength > MaxBufferLength) ? MaxBufferLength : DataLength;
  while (got < DataLength)
    {
      int len = recv (Buffer, DataLength, MSG_WAITALL);
      if (len > 0)
	{
	  got += len;
	  String.append ((const char *) Buffer, len);

	}
      else if (len == -1)
	{
	  switch (errno)
	    {
	    case ENOTCONN:
	      std::cout << "The Socket is no connected\n";
	      break;
	    default:
	      std::cout << "Socket Error on recv\n";
	      break;
	    }
	  return -1;
	}
      else if (len == 0)
	{
	  std::cout << "The client has disconnected properly\n";
	  return 0;
	}
    }
  if (String.length () != got)
    return 0;			//irschend was is schief gegangen

  return got;
}



//###########################################
//Initialisiere statische Elementvariable
//bool SSocket::inited=0;
void *
AcceptThread (void *Pointer)
{
  //std::cout<<"in Thread\n";
  SSocket *dies = (SSocket *) (Pointer);

  dies->myAcceptThread ();
  return 0;
}



SSocket::SSocket (short Port, bool (*func) (CSocket *))
{

  sock = socket (PF_INET, SOCK_STREAM, 0);
  if (sock < 0)
    {
      perror ("failed creating socket");
      exit (1);
    }
  /*
   * Erzeuge die Socketadresse des Servers 
   */
  /*
   * Sie besteht aus Typ und Portnummer 
   */
  memset (&server, 0, sizeof (server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl (INADDR_ANY);
  server.sin_port = htons (Port);

  //Erzeuge die Bindung an die Serveradresse 
  // (d.h. an einen bestimmten Port) 
  if (bind (sock, (struct sockaddr *) &server, sizeof (server)) < 0)
    {
      perror ("can't bind socket");
      exit (0);
    }


  /*
   * Teile dem Socket mit, dass Verbindungswünsche
   * * von Clients entgegengenommen werden 
   */
  listen (sock, 5);
  std::cout << "SSocket erstellt\n";
  //Wenn alles gut dann login Funktion setzen 
  loginfunc = func;

  //Fake Threaden einer Elementfunktion ueber den this Pointer und eine Hilfsfunktion

  pthread_create (&ThreadID, 0, &AcceptThread, (void *) this);


}

SSocket::~SSocket ()
{
  pthread_cancel (ThreadID);
}





int
SSocket::myAcceptThread ()
{
  int fd;
  sockaddr_in client;
  for (;;)
    {
      socklen_t len = sizeof (client);
      fd = accept (sock, (struct sockaddr *) &client, &len);
      if (fd >= 0)
	loginfunc (new CSocket (fd, client));

    }
  return 0;
}
