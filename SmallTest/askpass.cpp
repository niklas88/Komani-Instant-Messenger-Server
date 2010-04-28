#include <iostream>


#include "askpass.h"

namespace askpass{

int intr_flag = 0;

 void intr_handler(int sig)
 {
     intr_flag = 1;
 }

 void tty_change_echo(int fd, int enable)
 {
    static struct termios ntio, otio;		/* new/old termios */
    static sigset_t nset, oset;			/* new/old sigset */
    static struct sigaction nsa, osa;		/* new/old sigaction */
    static int disabled = 0;

     if ( disabled && enable ) {
	 /* enable echo */
	 tcsetattr(fd, TCSANOW, &otio);
	 disabled = 0;
	 /* resotore sigaction */
	 sigprocmask(SIG_SETMASK, &oset, NULL);
	 sigaction(SIGINT, &osa, NULL);
	 if ( intr_flag != 0 ) {
	     /* re-generate signal  */
	     kill(getpid(), SIGINT);
	     sigemptyset(&nset);
	     sigsuspend(&nset);
	     intr_flag = 0;
	 }
     } else if (!disabled && !enable) {
	 /* set SIGINTR handler and break syscall on singal */
 	 sigemptyset(&nset);
	 sigaddset(&nset, SIGTSTP);
	 sigprocmask(SIG_BLOCK, &nset, &oset);
	 intr_flag = 0;
	 memset(&nsa, 0, sizeof(nsa));
	 nsa.sa_handler = intr_handler;
	 sigaction(SIGINT, &nsa, &osa);
	 /* disable echo */
 	 if (tcgetattr(fd, &otio) == 0 && (otio.c_lflag & ECHO)) {
	     disabled = 1;
	     ntio = otio;
	     ntio.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	     (void) tcsetattr(fd, TCSANOW, &ntio);
	 }
     }
	
     return;
 }


 int
 tty_readpass( const char *prompt, char *buf, size_t size )
 {
     int  tty, ret = 0;
     

     tty = open(TTY_NAME, O_RDWR);
     if ( tty < 0 ) {
	 //error("Unable to open %s\n", TTY_NAME);
	 return -1;				/* can't open tty */
     }
     if ( size <= 0 )
	 return -1;				/* no room */
     write(tty, prompt, strlen(prompt));
     buf[0] = '\0';
     tty_change_echo(tty, 0);			/* disable echo */
     ret = read(tty,buf, size-1);
     tty_change_echo(tty, 1);			/* restore */
     write(tty, "\n", 1);			/* new line */
     close(tty);
	 

     if ( strchr(buf,'\n') == NULL  )
	 return -1;
     if ( ret > 0 )
 	 buf[ret] = '\0';
     return ret;
		   
 }
}

//206704855
