#include  <stdio.h>
#include  <sys/types.h>
#include  <sys/socket.h>
#include  <signal.h>
#include  <netinet/in.h>
#include  <netdb.h>
#include  <sys/time.h>
#include  <string.h>
#include  <termios.h>
#include  <sys/file.h>
#include  <sys/filio.h>
#include  <errno.h>

#define   INVLENGTH    64
#define   LOGLENGTH    20
#define   MAXUSERS     1500
#define   DATAFILE     "Data/DBData"
#define   DCATFILE     "Data/DBCat"

char *crypt();
void exit();
long ulimit();
int  errno;
time_t time();
