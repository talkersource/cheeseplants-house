#include "header.h"
#include <sys/mman.h>
#include <unistd.h>

/****************************************************************************
  FUNCTION - crashme(chan)
  COMMAND SYNTAX - None
  Closes the house down, Logging the crash command into the logfile along with
  the name of the user that caused it.
 ****************************************************************************/

crashme(i)
int i;
{
    /*Ok, do this properly.... */
    (void) time((time_t*)&ti);
    timestr(ti,stringp);
    (void) sprintf(stringo,"%s: --------------- CRASHED %s ---------------\n",stringp,users[i].name);
    Logfile(stringo);
    for (i=0;i<MOST;i++) {
	if (users[i].doing!=0) {
	    if (users[i].doing<4||users[i].doing==7) {
		rshow(i,"\n HOUSE GOING DOWN!! Closing connection, sorry.\n");
		(void) close(i);
	    } else {
		rshow(i,"\n HOUSE GOING DOWN!! Closing connection, sorry.\n");
		LostConnect(i);
	    }
	}
    }
    dumpnotices();
    dumphostlist();
    closefiles();
    exit(1);
}

/****************************************************************************
  FUNCTION - patch(chan)
  COMMAND SYNTAX - <name> <number>
  Does a scanuser(name) to select a user (returns with error code if unfound)
  Then converts number to an integer... if >1 then sets any flag bits for the
  user that are set in number, If <1 then clears the bits. If 0 then well, its
  not exactly important.
 ****************************************************************************/

patch(i)
{
    int j,k;
    if ((j=scanuser(parv[1]))<0) return j;
    k=atoi(parv[2]);
    if (k<0) {
	k=-k;
	users[j].flags|=k;
	users[j].flags^=k;
	rshow(i,"Done...\n");
	return 0;
    } else {
	users[j].flags|=k;
	rshow(i,"Done...\n");
	return 0;
    }
}

/****************************************************************************
  FUNCTION - evict(chan)
  COMMAND SYNTAX - <name> <reason>
  Scanusers for user name. Returning error if unmatched. Then checks if the
  user is a resident, if this is so, checks if the name given is the ENTIRE
  name, since you need the whole thing to evict a resident. If not, returns
  an error message. Checks if you're trying to evict someone with Flag 9 set
  if so, tells them. If got this far, then evict the specified user and log
  the eviction and reason.
 ****************************************************************************/
	
evict(i)
int i;
{
    int j;
    if ((j=scanuser(parv[1]))<0) return j;
    if ((users[j].flags&1)!=0) 
	if (strcmp(unames[users[j].number],parv[1])!=0) {
	    rshow(i,"You must type the FULL name of that resident.\n");
	    return 0;
	}
    if ((users[j].flags&0x200)!=0) {
	rshow(i,"You must be JOKING!\n");
	(void) sprintf(stringo,"%s tried to evict you just then!\nReason: %s\n",users[i].name,parr[2]);
	rshow(j,stringo);
	return 0;
    }
    (void) time((time_t*)&ti);
    timestr(ti,stringp);
    (void) sprintf(stringo,"%s: %s evicted by %s - %s\n",stringp,users[j].name,users[i].name,parr[2]);
    Logfile(stringo);
    rshow(j,"You have been evicted.\n");
    LostConnect(j);
    rshow(i,"Done & Logged.\n");
    return 0;
}

/****************************************************************************
  FUNCTION - dotrace(chan)
  COMMAND SYNTAX - <name> <reason>
  Scanusers for user name. Returning error if unmatched.
  If they are there, trace them and log the reason
 ****************************************************************************/
	
dotrace(i)
int i;
{
    int j;
    if ((j=scanuser(parv[1]))<0) return j;
    if (parv[2][0]==0) {
	rshow(i,"You MUST give a reason.\n");
	return 0;
    }
    if ((users[j].flags&0x200)!=0) {
	(void) sprintf(stringo,"%s just traced you!\nReason: %s\n",users[i].name,parr[2]);
	rshow(j,stringo);
    }
    (void) time((time_t*)&ti);
    timestr(ti,stringp);
    (void) sprintf(stringo,"%s: %s traced by %s - %s\n",stringp,users[j].name,users[i].name,parr[2]);
    Logfile(stringo);
    if (users[j].logintype==2) {
	sprintf(stringo,"%s is calling from %s <%d>\n",users[j].name,users[j].host,users[j].ina[4]);
    } else {
	sprintf(stringo,"%s is calling from %s\n",users[j].name,users[j].host);
    }
    rshow(i,stringo);
    return 0;
}


/****************************************************************************
  FUNCTION - Logfile(string)
  Appends string to the end of LOGSFILE.
 ****************************************************************************/

Logfile(a)
char* a;
{
    int h,j;
    if ((h=open(LOGSFILE,O_WRONLY,-1))==-1)
	if ((h=open(LOGSFILE,O_CREAT|O_WRONLY,-1))==-1) return;
    (void) lseek(h,0L,SEEK_END);
    (void) write(h,a,strlen(a));
    (void) close(h);
    if (houseflags&4) {
	for (j=Firstuser;j!=-1;j=users[j].nextuser)
	    if (users[j].flags&512) {
		rshow(j,a);
	    }
    }
    return;
}

/****************************************************************************
  FUNCTION - Tempfile(chan)
  Appends the following on to the end of /tmp/templog:
  Username Time&Date - Host (Port number)
  For the user on channel chan.
 ****************************************************************************/

Tempfile(i)
int i;
{
    int h;
    if ((h=open("/tmp/templog",O_WRONLY,0600))==-1)
	if ((h=open("/tmp/templog",O_CREAT|O_WRONLY,0600))==-1) return;
    (void) lseek(h,0L,SEEK_END);
    (void) time((time_t*)&ti);
    timestr(ti,stringp);
    sprintf(stringo,"%-16s%-24s from %s (%d)\n",cnames[users[i].number],stringp,users[i].host,users[i].ina[4]);
    (void) write(h,stringo,strlen(stringo));
    (void) close(h);
    return;
}

/****************************************************************************
  FUNCTION - showhash(chan)
  COMMAND SYNTAX - <number>
  If number is outside the range 0-255, displays an error message, otherwise
  displays the hashing list numbered number.
 ****************************************************************************/

showhash(i)
int i;
{
    int j;
    j=atoi(parr[1]);
    if (j<0||j>255) {
	rshow(i,"Thats not a valid number, must be 0-255\n");
	return 0;};
    rshow(i,"Hash table info follows.....\n");
    if (nhash[j]==-1) {
	rshow(i,"That table is empty...\n");
	return 0;}
    for(j=nhash[j];j!=-1;j=nnext[j]) {
	sprintf(stringo,"%.5d %16s (%d > %d)\n",j,cnames[j],nlast[j],nnext[j]);
	rshow(i,stringo);
    }
    return 0;
}

/****************************************************************************
  FUNCTION - housecontrol(chan);
  Simply clears the 'houseflags' thingie....
  NEEDS FRUITING UP
 ****************************************************************************/

housecontrol(i)
int i;
{
    int t;
    t=0;
    if (parv[1][0]==0) {
	rshow(i,"Try one of 'name' 'tlog'\n");
	rshow(i,"Followed by 'on' or 'off' (default off)\n");
	return 0;
    }
    if (strcmp(parv[2],"on")==0) t=1;
    if (strcmp(parv[1],"name")==0) {
	if (t) {
	    rshow(i,"Nameserver override turned ON.\n");
	    houseflags|=1;
	} else {
	    rshow(i,"Nameserver override turned OFF.\n");
	    houseflags|=1;
	    houseflags^=1;
	}
	return 0;
    }
    if (strcmp(parv[1],"tlog")==0) {
	if (t) {
	    rshow(i,"Telnet logging turned ON.\n");
	    houseflags|=2;
	} else {
	    rshow(i,"Telnet logging turned OFF.\n");
	    houseflags|=2;
	    houseflags^=2;
	}
	return 0;
    }
    if (strcmp(parv[1],"log")==0) {
	if (t) {
	    rshow(i,"Log display turned ON.\n");
	    houseflags|=4;
	} else {
	    rshow(i,"Log display turned OFF.\n");
	    houseflags|=4;
	    houseflags^=4;
	}
	return 0;
    }
    rshow(i,"Illegal option.\n");
    return 0;
}

/****************************************************************************
  FUNCTION - locate(chan);
  COMMAND SYNTAX - name
  Scans all users for name and if not found, returns an error. Otherwise calls
  gethostbyaddr to locate them.
 ****************************************************************************/

locate(i)
int i;
{
    int j,k;
    struct hostent *h;
    char addr[4];
    char hostname[64];
    if ((j=findfile(parv[1]))<0) return j;
    if ((k=locateusernum(j))<0) {
	rshow(i,"Not logged in.\n");
	return 0;
    }
    addr[0]=(char) users[k].ina[0];
    addr[1]=(char) users[k].ina[1];
    addr[2]=(char) users[k].ina[2];
    addr[3]=(char) users[k].ina[3];
    h=gethostbyaddr(addr,4,AF_INET);
    if (h==0) {
	rshow(i,"Could not be located.\n");
	return 0;
    }
    for(j=0;j<16;j++) hostname[k]=0;
    (void) strncpy(hostname,h->h_name,63);
    hostname[63]=0;
    sprintf(stringo,"%s located as coming from %s.\n",users[k].name,hostname);
    rshow(i,stringo);
    strcpy(users[k].host,hostname);
    return 0;
}

/****************************************************************************
  FUNCTION - pstatus(string);
  Sets the ps x status string to string;
 ****************************************************************************/
/*
pstatus(s)
char *s;
{
    int n,m,t;
    m=strlen(argles);
    t=strlen(s);
    if (m<t) s[m]=0;
    for (n=0;n<t;n++) argles[n]=s[n];
    for (;n<m;n++) argles[n]=32;
}
*/

pstatus(s)
char *s;
{
    return;
}

/****************************************************************************
 ****************************************************************************/

dumpnotices()
{
    int h,i;
    if ((h=open(NOTCFILE,O_CREAT|O_WRONLY|O_TRUNC,-1))==-1) return;
    for(i=0;(i<MAXNOTICES)&&(noticenum[i]!=-1);i++) {
	(void) sprintf(stringo,"%-15s:%s\n",unames[noticenum[i]],noticemsg[i]);
	(void) write(h,stringo,strlen(stringo));
    }
    (void) close(h);
}

/****************************************************************************
 ****************************************************************************/

undumpnotices()
{
    int h,i,j;
    if ((h=open(NOTCFILE,O_RDONLY))==-1) return;
    for(i=0;(i<MAXNOTICES)&&(read(h,stringo,16)==16);) {
	for(j=15;(j>0)&&(stringo[j]==' '||stringo[j]==':');j--);
	stringo[j+1]=0;
	for (j=0;(read(h,&stringp[j],1)==1)&&stringp[j]!='\n';j++);
	stringp[j]=0;
	if ((j=findfile(stringo))>0) {
	    noticenum[i]=j;
	    strcpy(noticemsg[i],stringp);
	    i++;
	}
    }
    (void) close(h);
}

/****************************************************************************
 ****************************************************************************/

dumphostlist()
{
    int h,j;
    if ((h=open(HOSTFILE,O_CREAT|O_WRONLY|O_TRUNC,-1))==-1) return;
    for(j=0;(hostlist[j].s_addr!=0)&&(j<MAXHOSTS);j++) {
	(void) sprintf(stringo,"%.12u %.12u %-20s\n",
		       hostlist[j].s_addr,
		       hoststatus[j],
		       hostlabel[j]);
	(void) write(h,stringo,47);
    }
    (void) close(h);
}

/****************************************************************************
 ****************************************************************************/

undumphostlist()
{
    int h,i,j,n;
    if ((h=open(HOSTFILE,O_RDONLY))<0) return;
    n=3;
    for(i=0;(n>0)&&(i<MAXHOSTS);i++) {
	n=read(h,stringo,26);
	read(h,stringp,21);
	if (n==26) {
	    stringo[25]=0;
	    stringp[19]=0;
	    sscanf(stringo,"%u %u ",&hostlist[i].s_addr,&hoststatus[i]);
	    strcpy(hostlabel[i],stringp);
	}
    }
    (void) close(h);
}

/****************************************************************************
 ****************************************************************************/

delnotice(i)
int i;
{
    int j,k,n;
    if (parv[1][0]==0) return 0;
    if ((n=findfile(parv[1]))<0) return n;
    for(j=0;(j<MAXNOTICES)&&(noticenum[j]!=-1);j++) {
	if (noticenum[j]==n) {
	    for(k=j+1;k<MAXNOTICES;k++) {
		noticenum[k-1]=noticenum[k];
		strcpy(noticemsg[k-1],noticemsg[k]);
	    }
	    noticenum[MAXNOTICES-1]=-1;
	    noticemsg[MAXNOTICES-1][0]=0;
	    rshow(i,"Notice removed.\n");
	    return 0;
	}
    }
    rshow(i,"No notice to remove!\n");
    return 0;
}
