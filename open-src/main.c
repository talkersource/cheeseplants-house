#include "header.h"

/****************************************************************************
  FUNCTION - exit_nicely()
  Calls closeup then exit()s. This function is purely here for use with
  signal trapping.
 ****************************************************************************/

void exit_nicely()
{
    closeup();
    exit(0);
}

/****************************************************************************
  FUNCTION - main(argc,argv)
  Starts up the house in one of the following ways....
  No arguments - Runs on port Portnumber.
  R            - Remakes the database then dies.
  port         - Runs on requested port.
  port D       - Runs on requested port but doesnt detatch.
  Logs its startup if the actual socket code gets going......
 ****************************************************************************/

main(argc,argv)
    int  argc;
    char *argv[];
{
    int i;
    int dts;
    Firstuser=-1;
/*    if (strlen(argv[0])<79) {
	int siz,i;
	char *args;
	char **argps;
	siz=argc*4+84;
	for (i=1;i<argc;i++)
	    siz+=(strlen(argv[i])+1);
	argps=(char**)malloc((unsigned)siz);
	args=(char*) (argps+(argc*4)+4);
	siz=80;
	for (i=0;i<79;i++) args[i]='*';
	args[79]=0;
	argps[0]=args;
	argps[argc]=(char*)0;
	for (i=1;i<argc;i++) {
	    strcpy(&args[siz],argv[i]);
	    argps[i]=&args[siz];
	    siz+=(strlen(argv[i])+1);
	}
	execvp(argv[0],argps);
    }
    */
    for (i=0;i<MAXROOMS;i++)
	Roomuser[i]=-1;

    Portnum=Portnumber;
    if (argc>1) {
        i=atoi(argv[1]);
        Portnum=i;
    }
    (void) printf("Cheeseplant`s house version 3.0\n");
    (void) printf("Starting code up on port %d\n",Portnum);
    (void) printf("Maximum login channels set to %d\n",MOST);
    Lasthour=-1;
    Ipmonitor=-1;
    Lassusers=0;
    housestatus=0;
    houseflags=0;
    Maxlassusers=MAXDAYLASS;
    (void) printf("Maximum LASS logins during day set to   %d\n",MAXDAYLASS);
    (void) printf("Maximum LASS logins during night set to %d\n",MAXNIGHTLASS);
    (void) printf("Daytime starts at   %-.2d00 Hrs\n",DAYTIME);
    (void) printf("Nighttime starts at %d00 Hrs\n",NIGHTTIME);
    (void) printf("Maximum users allowed in resident list is %d\n",MAXUSERS);
    (void) printf("Trapping signals.....\n");
    if ((signal(SIGINT,exit_nicely))==SIG_ERR) {
        (void) printf("Error: Signals handling failure!\n");
        exit(1);
    }
    if ((signal(SIGPIPE,SIG_IGN))==SIG_ERR) {
        (void) printf("Error: Signals handling failure!\n");
        exit(1);
    }
    (void) ulimit(4,256L);
    (void) printf("File descriptor table size %d\n",dts=getdtablesize());
    if (dts<256) exit(0);
    if (argc>1) {
	if (argv[1][0]=='R') {
	    (void) printf("Remaking database for you......\n");
	    initfiles();
	    (void) time((time_t*)&ti);
	    fileremake();
	    (void) printf("Done...\n");
	    exit(0);
	}
    }
    if ((argc<3)||(argv[2][0]!='D')) {
	if ( fork() ) {
	    (void) printf("Startup code forked and exiting.....\n");
	    exit(0);
	}
	fflush(stdout);fflush(stdin);
	for (i= getdtablesize()-1;i>=0;i--)
	    (void) close(i);
	if ((i = open("/dev/tty",O_RDWR)) >=0) {
	    (void) ioctl(i,TIOCNOTTY,(char *)0);
	    (void) close(i);};
	(void) setpgrp();
    }
    argles=argv[0];
    pstatus("Cheeseplants House Startup: Initialising");
    for (i=0;i<MAXEDITS;i++) edits[i].user=-1;
    for (i=0;i<MAXUSERS;i++) 
	cnames[i][0]=0,unames[i][0]=0,nnext[i]=-1,nlast[i]=-1;
    for (i=0;i<MAXNOTICES;i++) noticenum[i]=-1,noticemsg[i][0]=0;
    for (i=0;i<256;i++) nhash[i]=-1;
    pstatus("Cheeseplants House Startup: Socket");
    setupsocket();
    pstatus("Cheeseplants House Startup: Mail");
    mailinit();
    pstatus("Cheeseplants House Startup: Files");
    initfiles();
    pstatus("Cheeseplants House Startup: Rooms");
    loadrooms();
    pstatus("Cheeseplants House Startup: Exits");
    loadexits();
    pstatus("Cheeseplants House Startup: Hosts");
    undumphostlist();
    pstatus("Cheeseplants House Startup: Notices");
    undumpnotices();
    (void)time((time_t*)&ti);
    timestr(ti,stringp);
    (void) sprintf(stringo,"%s: *\n",stringp);
    Logfile(stringo);
    pstatus("Cheeseplants House Startup: Begin");
    dosocket();
    return 0;
}

