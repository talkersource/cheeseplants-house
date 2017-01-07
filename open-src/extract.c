#include <sys/mman.h>
#include <unistd.h>
#include "extract.h"
#define BLOCKSIZE 256
#define BLOCKNUM 10

int *Entry;
int cptr,dptr;
int bufpoint;
char filebuffer[(BLOCKSIZE-8)*BLOCKNUM];
int *ifilebuffer;
char stringo[2048],stringp[2048];
char stringq[2048],stringr[2048];
char stringt[2048],stringu[2048];

main(argc,argv)
int argc;
char *argv[];
{
    int i;
    initfiles();
    ifilebuffer=(int *)filebuffer;
    for (i=0;i<MAXUSERS;i++)
	if (Entry[i+2]!=-1) {
	    if (fileread(i)!=0) {
		strncpy(stringo,&ifilebuffer[4],16);
		stringp[0]=0;
		bufpoint=48;
		if (ifilebuffer[0]&64) bufpoint+=(4*INVLENGTH);
		loadstring(stringp);
		if (argc==1) {
		    printf("===============================================================================\n");
		    printf("%-.4d %s %s\n",i,stringo,stringp);
		    loadstring(stringo);
		    loadstring(stringp);
		    loadstring(stringq);
		    loadstring(stringr);
		    loadstring(stringr);
		    loadstring(stringr);
		    if (stringq[0]) printf("%s\n\n",stringq);
		    if (stringo[0]) printf("Email  : %s\n",stringo);
		    if (stringp[0]) printf("Comment: %s\n",stringp);
		    loadstring(stringo);
		    loadstring(stringp);
		    if (ifilebuffer[0]&128) {
			printf("\nMindscape: %s\n%s\n",stringo,stringp);
		    }
		} else {
		    loadstring(stringp);
		    loadstring(stringq);
		    loadstring(stringq);
		    loadstring(stringr);
		    if (argc==2) {
			printf("%-16s %s\n",stringo,stringp);
		    } else {
			if (stringp[0]) printf("%-16s %s\n",stringo,stringp);
		    }
		}
	    }
	}
    closefiles();
}
/****************************************************************************
 ****************************************************************************/

initfiles()
{
    int i;
    if ((cptr=open(DCATFILE,O_RDONLY))==-1) {
	perror("Couldnt open catalogue file:");
	exit(1);
    }
    if ((dptr=open(DATAFILE,O_RDWR))==-1) {
	perror("Couldnt open data file:");
	exit(1);
    }
    Entry=(int *)mmap((caddr_t)0,(MAXUSERS+2)*4,PROT_READ,MAP_SHARED,cptr,0L);
    if ((int)Entry==-1) {
	perror("Mmap failed:");
	exit(1);
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
    (void) write(fptr,(char*)&i,4);
}

/****************************************************************************
 ****************************************************************************/

getint(fptr)
int fptr;
{
    int i;
    (void) read(fptr,(char*)&i,4);
    return i;
}

/****************************************************************************
 ****************************************************************************/

CreateBlock()
{
    char empty[BLOCKSIZE-8];
    int index;
    bzero(empty,BLOCKSIZE-8);
    index=Entry[0];
    index=lseek(dptr,0L,SEEK_END)/BLOCKSIZE;
    putint(dptr,-1);
    putint(dptr,-1);
    (void) write(dptr,empty,BLOCKSIZE-8);
    Entry[0]++;
    return index;
}

/****************************************************************************
 ****************************************************************************/

filewrite(n,siz)
int n,siz;
{
    int istart,index,posn;
    int oldindex;
    if (Entry[n+2]!=-1) fileerase(n);
    if ((istart=Entry[1])==-1) istart=CreateBlock();
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
    Entry[1]=index;
    Entry[n+2]=istart;
}

/****************************************************************************
 ****************************************************************************/

fileerase(n)
int n;
{
    int index,oldindex;
    index=Entry[n+2];
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
	putint(dptr,Entry[1]);
    }
    Entry[1]=Entry[n+2];
    Entry[n+2]=-1;
}    

/****************************************************************************
 ****************************************************************************/

filefree()
{
    int count;
    int index;
    count=0;
    if (Entry[1]==-1) return 0;
    for (index=Entry[1];index!=-1;) {
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
    if (Entry[n+2]==-1) return 0;
    count=0;
    for (posn=0,index=Entry[n+2];index!=-1;posn+=(BLOCKSIZE-8)) {
	count++;
	(void) lseek(dptr,(long)index*BLOCKSIZE,SEEK_SET);
	oindex=index;
	index=getint(dptr);
	if ((index<-1)||(count==BLOCKNUM)) return count;
	if (n!=(tn=getint(dptr))) 
	    (void) fprintf(stderr,"Inconsistancy!! Pos:%d Use:%d Is:%d\n",oindex,n,tn);
	(void) read(dptr,&filebuffer[posn],BLOCKSIZE-8);
    }
    return count;
}

/****************************************************************************

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
    ifilebuffer=(int *)filebuffer;
    ifilebuffer[0]=users[n].flags;
    (void) time((time_t*)&ti);
    if (typ==1) {
	users[n].age+=(ti-users[n].agetime);
	users[n].agetime=ti;
    }
    ifilebuffer[1]=users[n].age;
    ifilebuffer[2]=users[n].lastlogin;
    if (typ!=2) {
	(void) time((time_t*)&ifilebuffer[3]);
    } else {
	ifilebuffer[3]=users[n].idletime;
    }
    (void) strcpy((char*)&ifilebuffer[4],cnames[users[n].number]);
    ifilebuffer[9]=0;
    (void) strcpy((char*)&ifilebuffer[8],users[n].passwd);
    if ((users[n].passwd[0]==0) && (users[n].oldpasswd!=0))
	ifilebuffer[9]=users[n].oldpasswd;
    bufpoint=48;
    if ((users[n].flags&64)!=0) {
	int i;
	for (i=0;i<INVLENGTH;i++) {
	    int v;
	    v=users[n].invitenum[i];
	    v=(v<<16);
	    v|=(users[n].inviteflags[i]&0xffff);
	    ifilebuffer[(bufpoint/4)+i]=v;
	}
	bufpoint+=(INVLENGTH*4);
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


savestring(sptr)
char *sptr;
{
    (void) strcpy(&filebuffer[bufpoint],sptr);
    bufpoint+=(strlen(sptr)+1);
}


loaduser(n,typ)
int n,typ;
{
    if(fileread(users[n].number)==0) return;
    ifilebuffer=(int *)filebuffer;
    users[n].flags=ifilebuffer[0]|1;
    users[n].age=ifilebuffer[1];
    if (typ==1) {
	users[n].lastlogin=ifilebuffer[2];
	users[n].idletime=ifilebuffer[3];
    } else {
	if ((users[n].lastlogin-ifilebuffer[3])<MAXDELAY) users[n].lastlogin=ifilebuffer[2];
    }
    (void) strcpy(users[n].passwd,(char*)&ifilebuffer[8]);
    users[n].oldpasswd=0;
    if (users[n].passwd[0]==0) users[n].oldpasswd=ifilebuffer[9];
    bufpoint=48;
    if ((users[n].flags&64)!=0) {
	int i;
	for (i=0;i<INVLENGTH;i++) {
	    int v;
	    v=ifilebuffer[(bufpoint/4)+i];
	    users[n].inviteflags[i]=v&0xffff;
	    v=(v>>16);
	    users[n].invitenum[i]=v&0xffff;
	    if (users[n].invitenum[i]==0xffff) {
		users[n].invitenum[i]=-1;
		users[n].inviteflags[i]=0;
	    }
	}
	bufpoint+=(INVLENGTH*4);
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


 ****************************************************************************/

loadstring(sptr)
char *sptr;
{
    (void) strcpy(sptr,&filebuffer[bufpoint]);
    bufpoint+=(strlen(sptr)+1);
}

