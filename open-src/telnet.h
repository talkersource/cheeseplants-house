int      bufpos[MOST];
char     buffer[MOST][256];
int      tstate[MOST];
int      tflags[MOST];
int      tsposn[MOST];
char     tsbuff[MOST][32];
int      through[MOST];
int      startthro[MOST];

int      rdfiled[8],readmask[8];
struct   sockaddr_in myaddr;
struct   sockaddr_in connectaddress;

