#include <sys/mman.h>
#include <unistd.h>
#include "header.h"
#define BLOCKSIZE 256
#define BLOCKNUM 10

int cptr,dptr;
int bufpoint;
char filebuffer[(BLOCKSIZE-8)*BLOCKNUM];

#ifdef HAS_MMAP
int *Entry;

inline static int GETENTRY(int n) {return ntohl(Entry[n]);}
inline static void SETENTRY(int n, int v) {Entry[n]=htonl(v);}
#else
int *memEntry;

inline static int GETENTRY(int n) {return ntohl(memEntry[n]);}

inline static void SETENTRY(int n, int v) {
    lseek(cptr, (n*4), SEEK_SET);
    v=htonl(v);
    if (write(cptr, &v, 4) != 4) exit(2);
    memEntry[n]=v;
}
#endif

/****************************************************************************
 ****************************************************************************/

initfiles()
{
    int i;
    if ((cptr=open(DCATFILE,O_RDONLY))==-1) {
	if ((cptr=open(DCATFILE,O_CREAT|O_WRONLY,-1))==-1) {
	    perror("FATAL ERROR!!!! Couldnt open Catalogue At ALL!");
	    exit(1);}
	putint(cptr,0);
	for (i=0;i<=MAXUSERS;i++) putint(cptr,-1);
	if ((dptr=open(DATAFILE,O_CREAT|O_WRONLY,-1))==-1) {
            perror("FATAL ERROR!!!! Couldnt open Datafile At ALL!");
            exit(1);}
	(void) close((int) dptr);
    } else {
	int l;
	(void) close(cptr);
	if ((cptr=open(DCATFILE,O_RDWR))==-1) {
	    perror("FATAL ERROR!!!! Couldnt open Catalogue For Use!");
	    exit(1);}
	l=(int) lseek(cptr,0L,SEEK_END);
	if (l<(MAXUSERS*4+8))
	    for (i=(l-8)/4;i<=MAXUSERS;i++) putint(cptr,-1);
    }
    (void) close(cptr);
    if ((cptr=open(DCATFILE,O_RDWR))==-1) {
	perror("FATAL ERROR!!!! Couldnt open Catalogue For Use!");
	exit(1);}
#ifdef HAS_MMAP
    Entry=(int *)mmap((caddr_t)0,(MAXUSERS+2)*4,PROT_READ|PROT_WRITE,MAP_SHARED,cptr,0L);
    if ((int)Entry==-1) {
	perror("Mmap failed:");
	exit(1);
    }
#else
    memEntry=(int *)calloc((MAXUSERS+2)*4,1);
    fprintf(stderr,"Got %d bytes from catalogue\n",
	    read(cptr, memEntry, (MAXUSERS+2)*4));
#endif
    if ((dptr=open(DATAFILE,O_RDWR))==-1) {
	perror("FATAL ERROR!!!! Couldnt open Datafile For Use!");
	exit(1);}
    for(i=0;i<MAXUSERS;i++) if (GETENTRY(i+2)!=-1) {
	int j;
	(void) lseek(dptr,(long) GETENTRY(i+2)*BLOCKSIZE+24,SEEK_SET);
	(void) read(dptr,cnames[i],16);
	for (j=0;cnames[i][j];j++) unames[i][j]=cnames[i][j]|32;
	unames[i][j]=0;
	renhash(i);
    }
    
}

/****************************************************************************
 ****************************************************************************/

closefiles()
{
    (void) close(cptr);
    (void) close(dptr);
}

/****************************************************************************
 ****************************************************************************/

putint(fptr,i)
int fptr;
int i;
{
  i=htonl(i);
    (void) write(fptr,(char*)&i,4);
}

/****************************************************************************
 ****************************************************************************/

getint(fptr)
int fptr;
{
    int i;
    (void) read(fptr,(char*)&i,4);
    i=ntohl(i);
    return i;
}

/****************************************************************************
 ****************************************************************************/

CreateBlock()
{
    char empty[BLOCKSIZE-8];
    int index;
    bzero(empty,BLOCKSIZE-8);
    index=GETENTRY(0);
    index=lseek(dptr,0L,SEEK_END)/BLOCKSIZE;
    putint(dptr,-1);
    putint(dptr,-1);
    (void) write(dptr,empty,BLOCKSIZE-8);
    SETENTRY(0, GETENTRY(0)+1);
    return index;
}

/****************************************************************************
 ****************************************************************************/

filewrite(n,siz)
int n,siz;
{
    int istart,index,posn;
    int oldindex;
    if (GETENTRY(n+2)!=-1) fileerase(n);
    if ((istart=GETENTRY(1))==-1) istart=CreateBlock();
    index=istart;
    oldindex=-1;
    posn=0;
    for (;siz>0;) {
	if (index==-1) {
	    index=CreateBlock();
	    if (oldindex!=-1) {
		(void) lseek(dptr,(long)oldindex*BLOCKSIZE,SEEK_SET);
		putint(dptr,index);
	    }
	}
	(void) lseek(dptr,(long)index*BLOCKSIZE,SEEK_SET);
	oldindex=index;
	index=getint(dptr);
	putint(dptr,n);
	(void) write(dptr,&filebuffer[posn],BLOCKSIZE-8);
	siz-=BLOCKSIZE-8;
	posn+=BLOCKSIZE-8;
    }
    (void) lseek(dptr,(long)oldindex*BLOCKSIZE,SEEK_SET);
    putint(dptr,-1);
    SETENTRY(1,index);
    SETENTRY(n+2,istart);
}

/****************************************************************************
 ****************************************************************************/

fileerase(n)
int n;
{
    int index,oldindex;
    index=GETENTRY(n+2);
    if (index==-1) return;
    oldindex=-1;
    for (;index!=-1;) {
	(void) lseek(dptr,(long)index*BLOCKSIZE,SEEK_SET);
	oldindex=index;
	index=getint(dptr);
	if (index<-1) return;
	putint(dptr,-1);
    }
    if (oldindex!=-1) {
	(void) lseek(dptr,(long)oldindex*BLOCKSIZE,SEEK_SET);
	putint(dptr,GETENTRY(1));
    }
    SETENTRY(1,GETENTRY(n+2));
    SETENTRY(n+2,-1);
}    

/****************************************************************************
 ****************************************************************************/

filefree()
{
    int count;
    int index;
    count=0;
    if (GETENTRY(1)==-1) return 0;
    for (index=GETENTRY(1);index!=-1;) {
	count++;
	(void) lseek(dptr,(long)BLOCKSIZE*index,SEEK_SET);
	index=getint(dptr);
    }
    return count;
}

/****************************************************************************
 ****************************************************************************/

fileread(n)
int n;
{
    int posn,index,count,tn,oindex;
    bzero(filebuffer,sizeof(filebuffer));
    if (GETENTRY(n+2)==-1) return 0;
    count=0;
    for (posn=0,index=GETENTRY(n+2);index!=-1;posn+=(BLOCKSIZE-8)) {
	count++;
	(void) lseek(dptr,(long)index*BLOCKSIZE,SEEK_SET);
	oindex=index;
	index=getint(dptr);
	if ((index<-1)||(count==BLOCKNUM)) return count;
	if (n!=(tn=getint(dptr))) {
	    (void) sprintf(stringo,"Inconsistancy!! Pos:%d Use:%d Is:%d\n",oindex,n,tn);
	    Logfile(stringo);};
	(void) read(dptr,&filebuffer[posn],BLOCKSIZE-8);
    }
    return count;
}

/****************************************************************************
 ****************************************************************************/

saveuser(n,typ)
int n,typ;
{
    if ((users[n].flags&1)==0) {
	fileerase(users[n].number);
	unames[users[n].number][0]=0;
	cnames[users[n].number][0]=0;
	renhash(n);
	return;}
    bzero(filebuffer,sizeof(filebuffer));
    bufpoint=0;
    saveint(users[n].flags);
    (void) time((time_t*)&ti);
    if (typ==1) {
	users[n].age+=(ti-users[n].agetime);
	users[n].agetime=ti;
    }
    saveint(users[n].age);
    saveint(users[n].lastlogin);
    if (typ!=2) {
        int now;
	(void) time((time_t*)&now);
	saveint(now);
    } else {
	saveint(users[n].idletime);
    }
    savenstring(cnames[users[n].number],16);
    savenstring(users[n].passwd,16);
    if ((users[n].passwd[0]==0) && (users[n].oldpasswd!=0)) {
      bufpoint=9*4;
      saveint(users[n].oldpasswd);
      bufpoint=48;
    }
    if ((users[n].flags&64)!=0) {
	int i;
	for (i=0;i<INVLENGTH;i++) {
	    int v;
	    v=users[n].invitenum[i];
	    v=(v<<16);
	    v|=(users[n].inviteflags[i]&0xffff);
	    saveint(v);
	}
    }
    if ((users[n].flags&2)==0) users[n].title[0]=0;
    if ((users[n].flags&2)==0) users[n].desc[0]=0;
    if ((users[n].flags&8)==0) users[n].prompt[0]=0;
    if ((users[n].flags&8)==0) users[n].cprompt[0]=0;
    if ((users[n].flags&8)==0) users[n].tprompt[0]=0;
    if ((users[n].flags&128)==0) users[n].scapename[0]=0;
    if ((users[n].flags&128)==0) users[n].scapedesc[0]=0;
    savestring(users[n].title);
    savestring(users[n].email);
    savestring(users[n].comment);
    savestring(users[n].desc);
    savestring(users[n].prompt);
    savestring(users[n].cprompt);
    savestring(users[n].tprompt);
    savestring(users[n].scapename);
    savestring(users[n].scapedesc);
    filewrite(users[n].number,bufpoint);
    return;
}

/****************************************************************************
 ****************************************************************************/

saveint(i)
int i;
{
    int tmp=htonl(i);
    (void) memcpy(&filebuffer[bufpoint],&tmp,4);
    bufpoint+=4;
}

/****************************************************************************
 ****************************************************************************/

loadint(i)
int i;
{
  int tmp;
  (void) memcpy(&tmp,&filebuffer[bufpoint],4);
  bufpoint+=4;
  return ntohl(tmp);
}

/****************************************************************************
 ****************************************************************************/

savestring(sptr)
char *sptr;
{
    (void) strcpy(&filebuffer[bufpoint],sptr);
    bufpoint+=(strlen(sptr)+1);
}

/****************************************************************************
 ****************************************************************************/

savenstring(sptr, n)
char *sptr;
int n;
{
    (void) strncpy(&filebuffer[bufpoint],sptr,n);
    bufpoint+=n;
}

/****************************************************************************
 ****************************************************************************/

loaduser(n,typ)
int n,typ;
{
    if(fileread(users[n].number)==0) return;
    bufpoint=0;
    users[n].flags=loadint()|1;
    users[n].age=loadint();
    if (typ==1) {
	users[n].lastlogin=loadint();
	users[n].idletime=loadint();
    } else {
      int a,b;
      a=loadint();
      b=loadint();
	if ((users[n].lastlogin-b)<MAXDELAY) users[n].lastlogin=a;
    }
    bufpoint+=16;

    loadnstring(users[n].passwd, 16);
    users[n].oldpasswd=0;
    if (users[n].passwd[0]==0) {
      bufpoint=9*4;
      users[n].oldpasswd=loadint();
      bufpoint=48;
    }
    if ((users[n].flags&64)!=0) {
	int i;
	for (i=0;i<INVLENGTH;i++) {
	    int v;
	    v=loadint();
	    users[n].inviteflags[i]=v&0xffff;
	    v=(v>>16);
	    users[n].invitenum[i]=v&0xffff;
	    if (users[n].invitenum[i]==0xffff) {
		users[n].invitenum[i]=-1;
		users[n].inviteflags[i]=0;
	    }
	}
    }
    loadstring(users[n].title);
    loadstring(users[n].email);
    loadstring(users[n].comment);
    loadstring(users[n].desc);
    loadstring(users[n].prompt);
    loadstring(users[n].cprompt);
    loadstring(users[n].tprompt);
    loadstring(users[n].scapename);
    loadstring(users[n].scapedesc);
    if ((users[n].flags&2)==0) (void) strcpy(users[n].title,"the resident.");
    if ((users[n].flags&8)==0) (void) strcpy(users[n].prompt,"-=> ");
    if ((users[n].flags&128)==0) (void) sprintf(users[n].scapename,"the mindscape of %s.",cnames[users[n].number]);
    if ((users[n].flags&128)==0) (void) sprintf(users[n].scapedesc,"This is %s's mindscape.",cnames[users[n].number]);
    return;
}

/****************************************************************************
 ****************************************************************************/

loadstring(sptr)
char *sptr;
{
    (void) strcpy(sptr,&filebuffer[bufpoint]);
    bufpoint+=(strlen(sptr)+1);
}

/****************************************************************************
 ****************************************************************************/

loadnstring(sptr,n)
char *sptr;
int n;
{
    (void) strcpy(sptr,&filebuffer[bufpoint]);
    bufpoint+=n;
}

/****************************************************************************
 ****************************************************************************/

filelog(n,li,lo)
int n;
int *li,*lo;
{
    if (GETENTRY(n+2)==-1) {
	*li=-1;
	*lo=-1;
	return -1;}
    (void) lseek(dptr,(long)GETENTRY(n+2)*BLOCKSIZE+8,SEEK_SET);
    (void) read(dptr,filebuffer,32);
    bufpoint=2*4;
    *li=loadint();
    *lo=loadint();
    return *li;
}

/****************************************************************************
 ****************************************************************************/

fileinfo(i)
int i;
{
    int s,f;
    rshow(i,"Ok then.... file data on its way...\n");
    f=filefree();
    s=GETENTRY(0);
    (void) sprintf(stringo,"Database size : %d p-sectors %d(%d) K\n",s,s/4,(s*256)/1000);
    rshow(i,stringo);
    (void) sprintf(stringo,"Free memory   : %d p-sectors %d K\n\n",f,f/4);
    rshow(i,stringo);
    if (parv[1][0]==0) return 0;
    rshow(i,"User specific requests not yet enabled... sorry.");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

filechangepass(i,s)
int i;
char *s;
{
    int p[4];
    int l;
    (void) strncpy((char*)&p[0],s,16);
    if (GETENTRY(i+2)==-1) return;
    l=GETENTRY(i+2);
    (void) lseek(dptr,(long)l*BLOCKSIZE+40,SEEK_SET);
    /* KLUDGE */
    putint(dptr,ntohl(p[0]));
    putint(dptr,ntohl(p[1]));
    putint(dptr,ntohl(p[2]));
    putint(dptr,ntohl(p[3]));
    return;
}

/****************************************************************************
 ****************************************************************************/

filechangename(i,s)
int i;
char *s;
{
    int p[4];
    int l;
    (void) strncpy((char*)&p[0],s,16);
    if (GETENTRY(i+2)==-1) return;
    l=GETENTRY(i+2);
    (void) lseek(dptr,(long)l*BLOCKSIZE+24,SEEK_SET);
    putint(dptr,p[0]);
    putint(dptr,p[1]);
    putint(dptr,p[2]);
    putint(dptr,p[3]);
    return;
}

/****************************************************************************
 ****************************************************************************/

fileremake()
{
    int tcptr,tdptr;
    int thisuser,thisblock;
    int blocksize,i;
    /* Must take the database and make a new one with it */
    if ((tcptr=open("/tmp/DBCat",O_CREAT|O_WRONLY,-1))==-1) {
	printf("Cannot open tempory cat file...\n");
	exit(1);};
    if ((tdptr=open("/tmp/DBData",O_CREAT|O_WRONLY,-1))==-1) {
	printf("Cannot open tempory data file...\n");
	exit(1);};
    putint(tcptr,0);
    putint(tcptr,-1);
    thisblock=0;
    for (thisuser=0;thisuser<MAXUSERS;thisuser++) {
	if (GETENTRY(thisuser+2)==-1) {
	    putint(tcptr,-1);
	} else {
	  int tmpi;
	    putint(tcptr,thisblock);
	    blocksize=fileread(thisuser);
	    bufpoint=0;
	    tmpi=loadint();
	    if (tmpi & 0x04000000)
		printf("%s\n",&filebuffer[16]);
	    bufpoint=0;
	    saveint(tmpi & 0x78fffecf);
	    for (i=1;i<=blocksize;i++) {
		if (i==blocksize) {
		    putint(tdptr,-1);
		} else {
		    putint(tdptr,thisblock+1);
		}
		putint(tdptr,thisuser);
		(void)write(tdptr,&filebuffer[(BLOCKSIZE-8)*(i-1)],BLOCKSIZE-8);
		thisblock++;
	    }
	}
    }
    (void) lseek(tcptr,0L,SEEK_SET);
    putint(tcptr,thisblock);
    (void) close(tcptr);
    (void) close(tdptr);
}

/****************************************************************************
 ****************************************************************************/

filekill(n)
int n;
{
    /* attempt to delete user n */
    int i;
    if ((i=locateusernum(n))<0) {
	unames[n][0]=0;
	cnames[n][0]=0;
	renhash(n);
	fileerase(n);
	return 0;
    } else {
	if (users[i].flags&1) {
	    users[i].flags^=1;
	    fileerase(n);
	    return 0;
	} else {
	    return -1;
	}
    }
}
	
/****************************************************************************
 ****************************************************************************/

countresidents(i)
int i;
{
    int n,c;
    for (c=0,n=0;n<MAXUSERS;n++)
	if (GETENTRY(n+2)!=-1) c++;
    (void) sprintf(stringo,"Users with database entries: %d.\n",c);
    rshow(i,stringo);
}

/****************************************************************************
 ****************************************************************************/

filezaplist(n)
int n;
{
    int i,j,k;
    /* Stage 1, zip through all logged in users and strip lists... */
    for (j=Firstuser;j!=-1;j=users[j].nextuser)
	for (i=0;i<INVLENGTH;i++)
	    if (users[j].invitenum[i]==n) {
		users[j].invitenum[i]=-1;
		users[j].inviteflags[i]=0;};
    /* Stage 2, go for a merry romp through my disk space.... */
    for (j=0;j<MAXUSERS;j++)
	if (GETENTRY(j+2)!=-1) {
	    bzero(filebuffer,BLOCKSIZE-8);
	    (void) lseek(dptr,(long)GETENTRY(j+2)*BLOCKSIZE+8,SEEK_SET);
	    (void) read(dptr,filebuffer,BLOCKSIZE-8);
	    bufpoint=0;
	    k=0;
	    if (loadint()&0x40) {
	      for (i=0;i<INVLENGTH;i++) {
		bufpoint=(12+i)*4;
		if (((loadint()>>16)&0xffff)==n) {
		  bufpoint=(12+i)*4;
		  saveint(-1);
		  k=-1;
		};
	      }
	      if (k!=0) {
		(void)lseek(dptr,(long)GETENTRY(j+2)*BLOCKSIZE+8,SEEK_SET);
		(void) write(dptr,filebuffer,BLOCKSIZE-8);
	      }
	    }
	}
}

/****************************************************************************
 ****************************************************************************/

loadtempuser(n) 
int n;
{
    int j;
    (void) time((time_t*)&users[255].lastlogin);
    (void) time((time_t*)&users[255].agetime);
    (void) time((time_t*)&users[255].idletime);
    users[255].objecting=-1;
    for (j=0;j<OBJECTNUM;j++)
	users[255].objected[j]=-1;
    users[255].number=n;
    users[255].name=cnames[n];
    loaduser(255,1);
}

/****************************************************************************
 ****************************************************************************/

savetempuser()
{
    saveuser(255,2);
    users[255].number=-1;
}


