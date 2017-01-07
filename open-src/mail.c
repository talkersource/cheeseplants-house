#include "header.h"
#include <sys/mman.h>
#include <unistd.h>

int iptr,bptr[16];

#ifdef HAS_MMAP
int *bods;

static int MailCount_Sent(int n) {
    return ntohl(bods[n*3]);
}

static int MailCount_Recvd(int n) {
    return ntohl(bods[n*3+1]);
}

static int MailCount_Quota(int n) {
    return ntohl(bods[n*3+2]);
}

static void MailSet_Sent(int n, int v) {
    bods[n*3]=htonl(v);
}

static void MailSet_Recvd(int n, int v) {
    bods[n*3+1]=htonl(v);
}

static void MailSet_Quota(int n, int v) {
    bods[n*3+2]=htonl(v);
}
#else
int *membods;

static int MailCount_Sent(int n) {
    return ntohl(membods[n*3]);
}

static int MailCount_Recvd(int n) {
    return ntohl(membods[n*3+1]);
}

static int MailCount_Quota(int n) {
    return ntohl(membods[n*3+2]);
}

static void MailSet_Sent(int n, int v) {
    lseek(iptr, ((n*3)*4), SEEK_SET);
    membods[n*3]=htonl(v);
    if (write(iptr, &membods[n*3+0], 4) != 4) exit(2);
}

static void MailSet_Recvd(int n, int v) {
    lseek(iptr, ((n*3+1)*4), SEEK_SET);
    membods[n*3+1]=htonl(v);
    if (write(iptr, &membods[n*3+1], 4) != 4) exit(2);
}

static void MailSet_Quota(int n, int v) {
    lseek(iptr, ((n*3+2)*4), SEEK_SET);
    membods[n*3+2]=htonl(v);
    if (write(iptr, &membods[n*3+2], 4) != 4) exit(2);
}

#endif

/****************************************************************************
  FUNCTION - mailinit()
  Opens/creates the 16 Mailbox files as bptr[0-15] and mmaps/creates the
  MailInfo file as the int array bods. If any of this fails, exit() is called.
 ****************************************************************************/

mailinit()
{
    int i;
    char boxname[16];
    if ((iptr=open("Mail/MailInfo",O_RDONLY))==-1) {
	if ((iptr=open("Mail/MailInfo",O_CREAT|O_WRONLY,-1))==-1) {
	    exit(1);}
	for (i=0;i<MAXUSERS*3;i++) putint(iptr,0);
#ifndef HAS_MMAP
	close(iptr);
#endif
        strcpy(boxname,"Mail/MailBoxA");
        for (i=0;i<16;i++) {
            if ((bptr[i]=open(boxname,O_CREAT|O_WRONLY,-1))==-1) {
                exit(1);}
            boxname[12]++;
            close (bptr[i]);
	    bptr[i]=-1;
        }
    } else {
#ifndef HAS_MMAP
	close(iptr);
#endif
    }
    strcpy(boxname,"Mail/MailBoxA");
    for (i=0;i<16;i++) {
	if ((bptr[i]=open(boxname,O_RDWR,-1))==-1) {
	    exit(1);}
	boxname[12]++;
    }
    if ((iptr=open("Mail/MailInfo",O_RDWR))==-1) exit(1);
#ifdef HAS_MMAP
    bods=(int *)mmap((caddr_t)0,(MAXUSERS*3)*4,PROT_READ|PROT_WRITE,MAP_SHARED,iptr,0L);
    close (iptr);
    if ((int)bods==-1) {
        exit(1);
    }
#else
    membods=(int *)calloc((MAXUSERS *3) * 4,1);
    if (!membods) exit(1);
    read(iptr, membods, MAXUSERS * 3 * 4);
#endif
}
	
/****************************************************************************
  FUNCTION - mailinfo(chan,num)
  Shows the mailbox of user num to the user on channel chan in table form.
 ****************************************************************************/

mailinfo(i,n,mm)
int i,n,mm;
{
    int m,jm,fr,to,ti;
    if (n<0) return 0;
    sprintf(stringo,"      Messages Sent: %d/%d       Messages Received: %d\n\n",MailCount_Sent(n),LETTERS+MailCount_Quota(n),MailCount_Recvd(n));
    rshow(i,stringo);
    if (MailCount_Recvd(n)==0)
	return 0;
    rshow(i,"      Num         Sent                Sender       Details. (Ie: New)\n      ---------------------------------------------------------------\n");
    m=1;
    (void) lseek(bptr[n&15],0L,SEEK_SET);
    for(;read(bptr[n&15],(char *)&jm,4)==4;) {
      jm=ntohl(jm);
        if (jm&0x00080000) {
            (void) lseek(bptr[n&15],(long) (jm&0xffff),SEEK_CUR);
        } else {
	    to=getint(bptr[n&15]);
	    if (to!=n) {
		(void) lseek(bptr[n&15],(long) (jm&0xffff)-4,SEEK_CUR);
	    } else {
		fr=getint(bptr[n&15]);
		ti=getint(bptr[n&15]);
		(void) lseek(bptr[n&15],(long) (jm&0xffff)-12,SEEK_CUR);
		timestr(ti,stringp);
		switch(jm&0x00030000) {
		case 0x00000000:
		    sprintf(stringo,"      %-3d %s  %-15s\n",m++,stringp,cnames[fr]);
		    break;
		case 0x00010000:
		    sprintf(stringo,"      %-3d %s  %-15s     -+* NEW *+-\n",m++,stringp,cnames[fr]);
		    break;
		case 0x00020000:
		    if (mm) {
			sprintf(stringo,"      %-3d %s  ---Anonymous---\n",m++,stringp);
		    } else {
			sprintf(stringo,"      %-3d %s (%-15s)\n",m++,stringp,cnames[fr]);
		    }
		    break;
		case 0x00030000:
		    if (mm) {
			sprintf(stringo,"      %-3d %s  ---Anonymous---     -+* NEW *+-\n",m++,stringp);
		    } else {
			sprintf(stringo,"      %-3d %s (%-15s)    -+* NEW *+-\n",m++,stringp,cnames[fr]);
		    }
		    break;
		}
		rshow(i,stringo);
	    }
	}
    }
    rshow(i,"      ---------------------------------------------------------------\n      Use 'read' and 'delete' to see/wipe messages.\n");
    return 0;
}

/****************************************************************************
  FUNCTION - mailstat(chan)
  COMMAND SYNTAX - <resident>
  Checks the given user is a resident (if not, return an error code) then
  shows that user's (incoming) mailbox.
 ****************************************************************************/

mailstat(i)
int i;
{
    int n,j;
    if ((n=findfile(parv[1]))<0) return n;
    if ((j=locateusernum(n))>=0) 
	if ((users[j].flags&1)==0) {
	    rshow(i,"They are not a resident.... \n");
	    return 0;
	}
    sprintf(stringo,"You examine %s's mailbox....\n",cnames[n]);
    rshow(i,stringo);
    return mailinfo(i,n,0);
}

/****************************************************************************
  FUNCTION - domail(chan)
  COMMAND SYNTAX - [name]
  If given no name, shows you your (incoming) mailbox. If given a name, calls
  mailedit().
 ****************************************************************************/

domail(i)
int i;
{
    if (parv[1][0]==0) {
	if ((users[i].flags&1)==0) {
	    rshow(i,"You're not a resident.... \n");
	    return 0;
	}
	return mailinfo(i,users[i].number,-1);
    }
    return mailedit(i);
}
	
/****************************************************************************
 ****************************************************************************/

readmail(i)
int i;
{
    int p,jm,fr,to,ti,n,j;
    if (parv[1][0]==0) {
	rshow(i,"Please specify a message number....\n");
	return 0;
    } else {
	p=atoi(parv[1]);
    }
    if (p<=0) {
	rshow(i,"Bad message number!\n");
	return 0;
    }
    n=users[i].number;
    if (p>MailCount_Recvd(n)) {
	rshow(i,"You dont have that many items of mail.\n");
	return 0;
    }
    /* read mail message p */
    (void) lseek(bptr[n&15],0L,SEEK_SET);
    for(;read(bptr[n&15],(char *)&jm,4)==4;) {
      jm=ntohl(jm);
	if (jm&0x080000) {
	    (void) lseek(bptr[n&15],(long) (jm&0xffff),SEEK_CUR);
	} else {
	    to=getint(bptr[n&15]);
	    if (to!=n) {
		(void) lseek(bptr[n&15],(long) (jm&0xffff)-4,SEEK_CUR);
	    } else {
		fr=getint(bptr[n&15]);
		ti=getint(bptr[n&15]);
		p--;
		if (p!=0) {
		    (void) lseek(bptr[n&15],(long) (jm&0xffff)-12,SEEK_CUR);
		} else {
		    timestr(ti,stringp);
		    rshow(i,"  ===========================================================================\n");
		    if (jm&0x00020000) {
			sprintf(stringo,"Mail from someone anonymous sent %s:\n",stringp);
		    } else {
			sprintf(stringo,"Mail from %s sent %s:\n",cnames[fr],stringp);
		    }
		    rshow(i,stringo);
		    read(bptr[n&15],stringo,(jm&0xffff)-12);
		    (void) lseek(bptr[n&15],(long)-4-(jm&0xffff),SEEK_CUR);
		    putint(bptr[n&15],jm&0xfffeffff);
		    maildecrypt(stringo);
		    for (j=0;stringo[j]!=0;j++)
			if (stringo[j]=='\177') stringo[j]='\n';
		    sprintf(stringp,"%s\n  ===========================================================================\n",stringo);
		    rshow(i,stringp);
		    return 0;
		}
	    }
	}
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

deletemail(i)
int i;
{
    int p,n,fr,to,jm;
    if (parv[1][0]==0) {
	rshow(i,"Please specify a message number....\n");
	return 0;
    } else {
	p=atoi(parv[1]);
    }
    if (p<=0) {
	rshow(i,"Bad message number!\n");
	return 0;
    }
    n=users[i].number;
    if (MailCount_Recvd(n)<p) {
	rshow(i,"You dont have that many messages.\n");
	return 0;
    }
    /* delete mail message p */
    (void) lseek(bptr[n&15],0L,SEEK_SET);
    for(;read(bptr[n&15],(char*)&jm,4)==4;) {
      jm=ntohl(jm);
	if (jm&0x80000) {
            (void) lseek(bptr[n&15],(long) (jm&0xffff),SEEK_CUR);
	} else {
	    to=getint(bptr[n&15]);
	    if (to!=n) {
		(void) lseek(bptr[n&15],(long) (jm&0xffff)-4,SEEK_CUR);
	    } else {
		p--;
		if (p!=0) {
		    (void) lseek(bptr[n&15],(long) (jm&0xffff)-4,SEEK_CUR);
		} else {
		    fr=getint(bptr[n&15]);
		    (void) lseek(bptr[n&15],-12L,SEEK_CUR);
		    putint(bptr[n&15],jm|0x00080000);
		    MailSet_Recvd(to,MailCount_Recvd(to)-1);
		    MailSet_Sent(fr,MailCount_Sent(fr)-1);
		    rshow(i,"Message deleted.\n");
		    return 0;
		}
	    }
	}
    }
    return 0;
}
    
/****************************************************************************
 ****************************************************************************/

sendmail(i,s,n,ff)
int i,ff;
char *s;
char *n;
{
    int j,k;
    int m;
    if ((users[i].flags&1)==0) {
	rshow(i,"You're not resident! You cant send mail!\n");
	return 0;
    }
    for (j=0;s[j]!=0;j++)
	if (s[j]=='\n') s[j]='\177';
    j=findfile(n);
    if (j<0) return 0;
    if ((k=locateusernum(j))>=0)
	if ((users[k].flags&1)==0) {
	    rshow(i,"They're not resident! You cant send mail!\n");
	    return 0;
	}
    mailcrypt(s);
    (void) time((time_t*)&ti);
    (void) lseek(bptr[j&15],0L,SEEK_END);
    putint(bptr[j&15],(strlen(s)+13)|0x00010000|ff);
    putint(bptr[j&15],j);
    putint(bptr[j&15],users[i].number);
    putint(bptr[j&15],ti);
    write(bptr[j&15],s,strlen(s)+1);
    MailSet_Recvd(j, MailCount_Recvd(j)+1);
    MailSet_Sent(users[i].number, MailCount_Sent(users[i].number)+1);
    rshow(i,"Mail sent....\n");
    /* send mail to user k */
    if ((m=findusernum(j))<0) return 0;
    if (ff&0x00020000) {
	sprintf(stringo,"     **** You have new mail from someone anonymous (read %d) ****.\n",MailCount_Recvd(j));
    } else {
	sprintf(stringo,"     **** You have new mail from %s (read %d) ****.\n",users[i].name,MailCount_Recvd(j));
    }
    rshow(m,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

getmailrcvd(n)
int n;
{
    /* Return mail messages recieved */
    return MailCount_Recvd(n);
}

/****************************************************************************
 ****************************************************************************/

getnewmailrcvd(n)
int n;
{
    int jm,to;
    if (MailCount_Recvd(n)==0)
	return 0;
    (void) lseek(bptr[n&15],0L,SEEK_SET);
    for(;read(bptr[n&15],(char *)&jm,4)==4;) {
      jm=ntohl(jm);
        if ((jm&0x00090000)!=0x00010000) {
            (void) lseek(bptr[n&15],(long) (jm&0xffff),SEEK_CUR);
        } else {
	    to=getint(bptr[n&15]);
	    (void) lseek(bptr[n&15],(long) (jm&0xffff)-4,SEEK_CUR);
	    if (to==n) return 1;
	}
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

overmaillimit(n)
int n;
{
    /* return if you're over your mail limit */
    return (MailCount_Sent(n)>=(MailCount_Quota(n)+LETTERS));
}

/****************************************************************************
 ****************************************************************************/

vapemail(n)
int n;
{
    int f,fr,to,jm;
    /* Wipe all mail from/to n */
    for (f=0;f<16;f++) {
	(void) lseek(bptr[f],0L,SEEK_SET);
	for(;read(bptr[f],(char *)&jm,4)==4;) {
	  jm=ntohl(jm);
	    if (jm&0x80000) {
		(void) lseek(bptr[f],(long) (jm&0xffff),SEEK_CUR);
	    } else {
		to=getint(bptr[f]);
		fr=getint(bptr[f]);
		if ((to!=n)&&(fr!=n)) {
		    (void) lseek(bptr[f],(long) (jm&0xffff)-8,SEEK_CUR);
		} else {
		    (void) lseek(bptr[f],-12L,SEEK_CUR);
		    putint(bptr[f],jm|0x80000);
		    if (to>0) MailSet_Recvd(to, MailCount_Recvd(to)-1);
		    if (fr>0) MailSet_Sent(fr, MailCount_Sent(fr)-1);
		    (void) lseek(bptr[f],(long) (jm&0xffff)-4,SEEK_CUR);
		}
	    }
	}
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

mailcrypt(s)
char *s;
{
    int j,e,a,c,d;
    e=7;a=23;
    for (j=0;s[j]!=0;j++) {
	c=(int) s[j];
	c-=32;
	d=c;
	c+=e;
	c+=a;
	c=c%96;
	a=a+3;
	e=e*d;
	e=e%100;
	c=c+32;
	s[j]=(char) c;
    }
}

/****************************************************************************
 ****************************************************************************/

maildecrypt(s)
char *s;
{
    int j,e,a,c;
    e=7;a=23;
    for (j=0;s[j]!=0;j++) {
	c=(int) s[j];
	c-=32;
	c-=a;
	c-=e;
	for(;c<0;c+=96);
	c=c%96;
	a+=3;
	e=e*c;
	e=e%100;
	c=c+32;
	s[j]=(char) c;
    }
}

/****************************************************************************
 ****************************************************************************/

mailredo(j)
int j;
{
    int i;
    int tptr;
    int jm,to,fr,ti;
    int items;
    int saved;
    char msg[2048];
    char boxname[16];
    pstatus("Cheeseplants House: Mail: Cleaning");
    rshow(j,"Clearing counters....\n");
    for (i=0;i<MAXUSERS;i++) {
	MailSet_Sent(i,0);
	MailSet_Recvd(i,0);
    }
    items=0;
    saved=0;
    unlink("Mail/Tempory");
    strcpy(boxname,"Mail/MailBoxA");
    for (i=0;i<16;i++) {
	int tempsaved;
	tempsaved=0;
        if (link(boxname,"Mail/Tempory")==-1)
            exit(1);
        if ((tptr=open("Mail/Tempory",O_RDWR))==-1)
            exit(1);
        (void) close(bptr[i]);
	if (unlink(boxname)==-1)
            exit(1);
        if ((bptr[i]=open(boxname,O_CREAT|O_WRONLY,-1))==-1)
            exit(1);
	(void) lseek(tptr,0L,SEEK_SET);
	for(;read(tptr,(char *)&jm,4)==4;) {
	  jm=ntohl(jm);
	    if (jm&0x80000) {
                (void) lseek(tptr,(long) (jm&0xffff),SEEK_CUR);
		saved+=(jm&0xffff)+4;
	    } else {
		to=getint(tptr);
		fr=getint(tptr);
		ti=getint(tptr);
		putint(bptr[i],jm);
		putint(bptr[i],to);
		putint(bptr[i],fr);
		putint(bptr[i],ti);
		read(tptr,msg,(jm&0xffff)-12);
		write(bptr[i],msg,(jm&0xffff)-12);
		MailSet_Sent(fr, MailCount_Sent(fr)+1);
		MailSet_Recvd(to, MailCount_Recvd(to)+1);
		items++;
		tempsaved++;
	    }
	}
        close(tptr);
        close(bptr[i]);
        if (unlink("Mail/Tempory")==-1)
            exit(1);
	sprintf(stringo,"Cleaned out %s...(%d) %d->%d\n",boxname,tempsaved,bptr[i],tptr);
	rshow(j,stringo);
        boxname[12]++;
    }
    sprintf(stringo,"%d items of mail processed...\n",items);
    rshow(j,stringo);
    sprintf(stringo,"%d bytes saved...\n",saved);
    rshow(j,stringo);
    strcpy(boxname,"Mail/MailBoxA");
    for (i=0;i<16;i++) {
        if ((bptr[i]=open(boxname,O_RDWR))==-1)
            exit(1);
        boxname[12]++;
    }
    rshow(j,"Mailboxes reopened... service restored....\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

maillimit(i)
int i;
{
    int n,j;
    if (parv[1][0]==0) return 0;
    if ((n=findfile(parv[1]))<0) return n;
    if ((j=locateusernum(n))>=0) 
	if ((users[j].flags&1)==0) {
	    rshow(i,"They are not a resident.... \n");
	    return 0;
	}
    if (parv[2][0]!=0) MailSet_Quota(n, MailCount_Quota(n)+atoi(parv[2]));
    sprintf(stringo,"%s can send %d+%d=%d letters.\n",cnames[n],LETTERS,MailCount_Quota(n),MailCount_Quota(n)+LETTERS);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
  FUNCTION - mailscan(chan,num)
  Shows the mailbox of user num to the user on channel chan in table form.
 ****************************************************************************/

mailscan(i,n)
int i,n;
{
    int m,jm,fr,to,ti,mb;
    sprintf(stringo,"      Messages Sent: %d/%d\n\n",MailCount_Sent(n),LETTERS+MailCount_Quota(n));
    rshow(i,stringo);
    if (MailCount_Sent(n)==0)
	return 0;
    rshow(i,"      Num         Sent              Recipient      Details. (Ie: New)\n      ---------------------------------------------------------------\n");
    m=1;
    for (mb=0;mb<16;mb++) {
	(void) lseek(bptr[mb],0L,SEEK_SET);
	for(;read(bptr[mb],(char *)&jm,4)==4;) {
	  jm=ntohl(jm);
	    if (jm&0x00080000) {
		(void) lseek(bptr[mb],(long) (jm&0xffff),SEEK_CUR);
	    } else {
		to=getint(bptr[mb]);
		fr=getint(bptr[mb]);
		if (fr!=n) {
		    (void) lseek(bptr[mb],(long) (jm&0xffff)-8,SEEK_CUR);
		} else {
		    ti=getint(bptr[mb]);
		    (void) lseek(bptr[mb],(long) (jm&0xffff)-12,SEEK_CUR);
		    timestr(ti,stringp);
		    switch(jm&0x00030000) {
		    case 0x00000000:
			sprintf(stringo,"      %-3d %s  %-15s\n",m++,stringp,cnames[to]);
			break;
		    case 0x00010000:
			sprintf(stringo,"      %-3d %s  %-15s     -+* NEW *+-\n",m++,stringp,cnames[to]);
			break;
		    case 0x00020000:
			sprintf(stringo,"      %-3d %s  %-15s        Anon.\n",m++,stringp,cnames[to]);
			break;
		    case 0x00030000:
			sprintf(stringo,"      %-3d %s  %-15s     New & Anon.\n",m++,stringp,cnames[to]);
			break;
		    }
		    rshow(i,stringo);
		}
	    }
	}
    }
    rshow(i,"      ---------------------------------------------------------------\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

mailfind(i)
int i;
{
    int n,j;
    if ((n=findfile(parv[1]))<0) return n;
    if ((j=locateusernum(n))>=0) 
	if ((users[j].flags&1)==0) {
	    rshow(i,"They are not a resident.... \n");
	    return 0;
	}
    sprintf(stringo,"You look for %s's messages....\n",cnames[n]);
    rshow(i,stringo);
    return mailscan(i,n);
}

/****************************************************************************
 ****************************************************************************/

mailreply(i)
int i;
{
    int p,jm,fr,to,ti,n,j;
    if (parv[1][0]==0) {
	rshow(i,"Please specify a message number....\n");
	return 0;
    } else {
	p=atoi(parv[1]);
    }
    if (p<=0) {
	rshow(i,"Bad message number!\n");
	return 0;
    }
    n=users[i].number;
    if (p>MailCount_Recvd(n)) {
	rshow(i,"You dont have that many items of mail.\n");
	return 0;
    }
    /* read mail message p */
    (void) lseek(bptr[n&15],0L,SEEK_SET);
    for(;read(bptr[n&15],(char *)&jm,4)==4;) {
      jm=ntohl(jm);
	if (jm&0x80000) {
	    (void) lseek(bptr[n&15],(long) (jm&0xffff),SEEK_CUR);
	} else {
	    to=getint(bptr[n&15]);
	    if (to!=n) {
		(void) lseek(bptr[n&15],(long) (jm&0xffff)-4,SEEK_CUR);
	    } else {
		fr=getint(bptr[n&15]);
		ti=getint(bptr[n&15]);
		p--;
		if (p!=0) {
		    (void) lseek(bptr[n&15],(long) (jm&0xffff)-12,SEEK_CUR);
		} else {
		    if (jm&0x00020000) {
			sprintf(stringo,"Attemtping to reply to the anonymous sender:\n");
			
		    } else {
			sprintf(stringo,"Attempting to reply to %s:\n",cnames[fr]);
			
		    }
		    rshow(i,stringo);
		    replyedit(i,fr,jm&0x00020000);
		    return 0;
		}
	    }
	}
    }
    return 0;
}
