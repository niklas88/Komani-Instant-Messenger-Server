#include <fcntl.h>
#include <termios.h>
#include <signal.h>

#ifndef __HEADER_ASKPASS__
#define __HEADER_ASKPASS__

namespace askpass {
#define TTY_NAME "/dev/tty"
void intr_handler(int sig);
int tty_readpass( const char *prompt, char *buf, size_t size );


}
#endif

