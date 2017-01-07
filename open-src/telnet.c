#include "header.h"
#include "telnet.h"

int     charactercount;
int     mysocket;

/****************************************************************************
  FUNCTION - closeup()
  Closes the socket.
 ****************************************************************************/

closeup()
{
    (void) close(mysocket);
}

/****************************************************************************
  FUNCTION - setupsocket()
  Sets up an internet socket on port number Portnum. If this fails then exit()
 ****************************************************************************/

setupsocket()
{
    int  tmp = 1;
    int  i;

    (void) time((time_t*)&lasstime);
    lasstime-=DLASSWAIT;
    (void) time((time_t*)&lasschecktime);
    lasstries=0;
    (void) time((time_t*)&lasttick);
    charactercount=0;
    lasswait=DLASSWAIT;
    /* Clear all buffers */
    for (i=0;i<MOST;i++) {
        bufpos[i]=0;
        buffer[i][0]=0;
	through[i]=0;
	time ((time_t*)&startthro[i]);
    };
    for (i=0;i<MAXHOSTS;i++) hostlist[i].s_addr=0,hoststatus[i]=0;
    bzero((char *) &myaddr, sizeof(struct sockaddr_in));
    myaddr.sin_family=AF_INET;
    myaddr.sin_port=ntohs(Portnum);
    if ((mysocket=socket(AF_INET,SOCK_STREAM,0))==-1) {
        (void) printf("Error: can't get socket.\n",stderr);
        exit(1);
    }
    if (setsockopt(mysocket,SOL_SOCKET,SO_REUSEADDR,(char *)&tmp,sizeof(tmp))<0) {
        closeup();
        exit(1);
    }
    if(bind(mysocket,(struct sockaddr*)&myaddr,sizeof(struct sockaddr_in))==-1) {
        closeup();
        exit(1);
    }
    if(listen(mysocket,10)==-1) {
        closeup();
        exit(1);
    }
    (void) ioctl(mysocket,FIONBIO,&tmp);
    readmask[0] = 1 << mysocket ;
}

/****************************************************************************
 ****************************************************************************/

dosocket()
{
    int i;
    (void) time((time_t *)&ti);
    whattime=localtime((time_t*)&ti); 
    for(i=0;i<MOST;i++)
	users[i].number=-1;
    for(;;) {
	int i,j;
	struct   timeval timeout;
	timeout.tv_sec=60;
        timeout.tv_usec=0;
        for (j=0;j<8;j++)
            rdfiled[j] = readmask[j];
        if((j=select(256,(fd_set*)rdfiled,(fd_set*)0,(fd_set*)0,&timeout)) < 0) {
            if (errno!=EINTR) {
                closeup();
                exit(1);
            }
        }
	if (j>=0) {
            if(rdfiled[0] & (1 << mysocket))  {
		pstatus("Cheeseplants House: Connecting");
                (void) DoConnect();
		pstatus("Cheeseplants House: Serving");
	    }
            for(i=0;i<MOST;i++)
                if(i!=mysocket &&(rdfiled[i/32] & ( 1<<(i&31) ))) {
                    (void) ServeConnect(i);};
	    charactercount++;
	}
	if ((j<1)||(charactercount>500)) {
	    (void) time((time_t*)&ti);
	    charactercount=0;
	    if ((ti-lasttick)>=TICKLENGTH) {
		int k,i;
		/* Tick tock, its my 'heartbeat' for idleness etc... */
		for(i=Firstuser;i!=-1;i=users[i].nextuser) {
		    if (users[i].leaving<0) {
			rshow(i,"Logging you out.\n");
			LostConnect(i);
		    } else {
			if (users[i].leaving) {
			    users[i].leaving-=(ti-lasttick);
			    if (users[i].leaving==0) users[i].leaving=-1;
			    if (users[i].leaving<0)
				rshow(i,"You will shortly be logged out.\n\n\n\n");
			}
			k=(ti-users[i].idletime);
			switch(users[i].logintype) {
			case 0:
			case 2:
			    k-=INETIDLE;
			    break;
			case 1:
			    k-=LASSIDLE;
			    break;
			default:
			    k=-1;
			}
			if (k>0)
			    rshow(i,"\007\007You are getting idle, your login will terminate automatically soon.\n");
			if (k>=TICKLENGTH*2)
			    LostConnect(i);
		    }
		}
		lasttick=ti;
	    }
	    whattime=localtime((time_t*)&ti);
	    if (whattime->tm_hour!=Lasthour) {
		Lasthour=whattime->tm_hour;
		dumpnotices();
		dumphostlist();
		/*            saveall(-1);*/
		/*Do limits on LASS users and things here */
		if ((whattime->tm_wday!=0)&&(whattime->tm_wday<6)) {
		    if ((whattime->tm_hour<DAYTIME)||(whattime->tm_hour>=NIGHTTIME)) {
			Maxlassusers=MAXNIGHTLASS;
			lasswait=NLASSWAIT;};
		    
		    if ((whattime->tm_hour>=DAYTIME)&&(whattime->tm_hour<NIGHTTIME)) {
			Maxlassusers=MAXDAYLASS;
			lasswait=DLASSWAIT;}
		} else {
		    Maxlassusers=MAXNIGHTLASS;
		    lasswait=NLASSWAIT;
		}
	    }
        }
    }
}

/****************************************************************************
 ****************************************************************************/

DoConnect()
{
    int path;
    int cadlen;
    struct hostent *j;
    int k;
    int a,b,c,d;
    char hostname[64];
    char *fullmsg="I'm afraid the house is too full!\n\rSorry\n\r";
    cadlen=sizeof(connectaddress);
    if((path=accept(mysocket,(struct sockaddr*)&connectaddress,&cadlen))<0) {
        return 0;
    }
    if ( path >=MOST ) {
        write(path,fullmsg,strlen(fullmsg));
        (void) close(path);
        return 0;
    }
    through[path]=0;
    time((time_t*)&startthro[path]);
    tstate[path]=0;
    tflags[path]=0;
    bufpos[path]=0;
    if (houseflags&1) {
	j=0;
    }  else {
	int before;
	(void) time((time_t*)&before);
	pstatus("Cheeseplants House: Connecting: Nameserve");
	j=gethostbyaddr((char*)&connectaddress.sin_addr,cadlen,AF_INET);
	(void) time((time_t*)&ti);
	if ((ti-before)>15) houseflags|=1;
    }
    {
	unsigned long ad=ntohl(connectaddress.sin_addr.s_addr);
	users[path].ina[0]=a=(int) (ad & 0xFF);
	ad=ad >> 8;
	users[path].ina[1]=b=(int) (ad & 0xFF);
	ad=ad >> 8;
	users[path].ina[2]=c=(int) (ad & 0xFF);
	ad=ad >> 8;
	users[path].ina[3]=d=(int) (ad & 0xFF);
    }
    users[path].ina[4]=(int)   connectaddress.sin_port;
    if (j!=0) {
        for(k=0;k<16;k++) hostname[k]=0;
        (void) strncpy(hostname,j->h_name,63);
        hostname[63]=0;
    } else {
	(void) sprintf(stringo,"%d.%d.%d.%d %d",a,b,c,d,connectaddress.sin_port);
	(void) strncpy(hostname,stringo,63);
    }
    {
	int l;
	users[path].cposn=0;
	users[path].number=-2; /* -2 being No name, but loggin in */
	users[path].name="* Login *";
	users[path].title[0]=0;
	users[path].ltitle[0]=0;
	users[path].desc[0]=0;
	users[path].flags=0;
	users[path].tempflags=0;
	users[path].age=0;
	(void) time((time_t*)&users[path].lastlogin);
	(void) time((time_t*)&users[path].agetime);
	users[path].lastlogout=0;
	users[path].prompt[0]=0;
	users[path].cprompt[0]=0;
	users[path].tprompt[0]=0;
	users[path].myprompt[0]=0;
	users[path].passwd[0]=0;
	users[path].oldpasswd=0;
	users[path].email[0]=0;
	users[path].comment[0]=0;
	strcpy(users[path].ignoremsg,"You get the feeling you are being ignored.\n");
	users[path].scapename[0]=0;
	users[path].scapedesc[0]=0;
	users[path].scapeuser=-1;
	for(l=0;l<INVLENGTH;l++) {
	    users[path].invitenum[l]=-1;
	    users[path].inviteflags[l]=0;};
	users[path].room=0;
	users[path].nextuser=-1;
	users[path].lastuser=-1;
	users[path].nextroom=-1;
	users[path].lastroom=-1;
	users[path].lastchar=0;
	users[path].thischar=0;
	users[path].charcount=0;
	users[path].localecho=0;
	users[path].leaving=0;
	users[path].ednum=-1;
	users[path].ttyheight=NORMALHEIGHT;
	users[path].ttywidth=NORMALWIDTH;
	users[path].tty[0]=0;
	users[path].plevel=5; /* Keep that autopromt OFF */
	users[path].objecting=-1;
	users[path].objection[0]=0;
	for (l=0;l<OBJECTNUM;l++)
	    users[path].objected[l]=-1;
    }
    (void) time((time_t*)&users[path].idletime);
    users[path].logintype=0;
    users[path].doing=1;
    if ((a==137)&&(b==205)) {
	users[path].logintype=2; /* WARWICK */
	switch(c) {
	case 233 : 
	    users[path].logintype=1; /*LASS - ts-ma*/
	    break;
	case 192 :
	    if (d==249) users[path].logintype=1; /*LASS n4*/
	    if (d==248) users[path].logintype=1; /*LASS n1*/
	    break;
/*	case 193 :
	    if ((d==129)||(d==130)) users[path].logintype=1; LASS n1 (old)*/
	    break;
	case 144 :
	    if (d==193) users[path].logintype=1; /*LASS n5*/
	    break;
	}
    }
/*    if (((a==127)&&(b==0)&&(c==0))||((a==140)&&(b==160))) {*/
    if (1) {
	if ((a==129)&&(b==31)&&(c==80)&&(d==121)) {
	    rshow(path,"Access from igate has been barred.\n");
	    (void) close(path);
	    users[path].logintype=0;
	    users[path].doing=0;
	    users[path].host[0]=0;
	    return 0;
	}
	{
	    int i;
	    for (i=0;(i<MAXHOSTS)&&(hostlist[i].s_addr!=0);i++)
		if (connectaddress.sin_addr.s_addr==hostlist[i].s_addr)
		    if (hoststatus[i]&2) {
			rshow(path,"Access from there has been barred.\n");
			(void) close(path);
			users[path].logintype=0;
			users[path].doing=0;
			users[path].host[0]=0;
			return 0;
		    }
	}
	(void) strncpy(users[path].host,hostname,31);
	/* Okay, now do the 'hello' stuff....... */
	rshow(path,"Welcome to Cheeseplant's house Version 3.38.\n");
	rshow(path,"At orchid.csv.warwick.ac.uk (137.205.192.5) 2001\n\n");
	rshow(path,"********************************************\n");
	rshow(path,"**                                        **\n");
	rshow(path,"**   ALL USERS SHOULD TYPE 'HELP RULES'   **\n");
	rshow(path,"**                                        **\n");
	rshow(path,"********************************************\n\n");
	if (users[path].logintype==1) {
	    Lassusers++;
	    (void) time((time_t*)&ti);
	    if (Lassusers>Maxlassusers) {
		sprintf(stringo,"There are too many logins via LASS at present.\nPlease do not retry within the next %d seconds.\nAny try before then will start the timer again so wont get in.\n",lasswait+39);
		rshow(path,stringo);
		rshow(path,"This is due to high loads on LASS caused by people repeatedly trying to log\nin using function keys and the like, you have them to thank.\n");
		lasstime=ti;
		Lassusers--;
		lasstries++;
		(void) close(path);
		users[path].logintype=0;
		users[path].doing=0;
		users[path].host[0]=0;
		return 0;
	    }
	    if ((ti-lasstime)<lasswait) {
		sprintf(stringo,"Due to use of function keys to repeatedly try to log in here\nLASS has been frequently overloaded, therefore the house will not accept any\nlogins from LASS within the next %d seconds.\nAn attempt by anyone within this time will reset the time again.\n",lasswait+19);
		rshow(path,stringo);
		lasstime=(ti+lasstime)/2;
		Lassusers--;
		lasstries++;
		(void) close(path);
		users[path].logintype=0;
		users[path].doing=0;
		users[path].host[0]=0;
		return 0;
	    }
	    lasstime=ti;
	    rshow(path,"Logins via LASS are restricted in number.\n\n");
	}
	rshow(path,"To contact me... E-Mail cheesey@dcs.warwick.ac.uk\n\n");
	(void) write(path,"\377\375\030",3); /*Request tty mode*/
	rprint(path,"Please enter your name:");
	readmask[path/32] |= 1 << (path&31);
	return 0;
    } else {
	close (path);
	return 0;
    }
}

/****************************************************************************
 ****************************************************************************/

ServeConnect(i)
int i;
{
    char c;
    if(read(i,&c,1)!=1) {
        LostConnect(i);
    } else {
	through[i]++;
        users[i].lastchar=users[i].thischar;
        users[i].thischar=c;

	/* A nice telnet state machine to handle them telnet things */

	switch (tstate[i]) {
	case 0 : if (c!=-1) break;
	    tstate[i]=1;
	    return 0;
	case 1 : 
	    if (c==-1) {
		tstate[i]=0;
		break;};
	    if (c==-2) {
		tstate[i]=6;
		return 0;};
	    if (c==-3) {
		tstate[i]=5;
		return 0;};
	    if (c==-4) {
		tstate[i]=4;
		return 0;};
	    if (c==-5) {
		tstate[i]=3;
		return 0;};
	    if (c==-6) {
		int u;
		tstate[i]=2;
		tsposn[i]=0;
		for(u=0;u<32;u++) tsbuff[i][u]=0;
		return 0;};
	    if (c==-9) {
		c=127;
		tstate[i]=0;
		break;};
	    switch(c) {
	    case -7 : TLog(i,"GO AHEAD");break;
	    case -8 : TLog(i,"ERASE LINE");break;
	    case -10: TLog(i,"ARE YOU THERE?");break;
	    case -11: TLog(i,"ABORT O/P");break;
	    case -12: TLog(i,"INTERRUPT PROCESS");break;
	    case -13: TLog(i,"BREAK");break;
	    case -14: TLog(i,"DATA MARK");break;
	    case -15: TLog(i,"NO-OPERATION");break;
	    case -16: TLog(i,"END SUBNEGOTIATION!!!!");break;
	    default:
		(void) sprintf(stringp,"IAC CHARACTER %d.\n",(int) c);
		TLog(i,stringp);
	    }
	    tstate[i]=0;
	    return 0;
	case 2 : return TSubNeg(i,c,0);
	case 3 : return TWill(i,c);
	case 4 : return TWont(i,c);
	case 5 : return TDo(i,c);
	case 6 : return TDont(i,c);
	case 7 : return TSubNeg(i,c,1);
	default :
	    Logfile("Telnet state machine Buggered! Illegal state!");
	    return 0;
	}
	if (c<0) {
	    (void) sprintf(stringp,"ILLEGAL CHARACTER %d (=%d).\n",(int) c,((int)c)+256);
	    TLog(i,stringp);
	}
        c&=127;
        if((c==13) && (users[i].lastchar!=10)) c=10;
        if((c==10) && (users[i].lastchar==13)) return 0;
        users[i].charcount++;
        if(c==10 && users[i].localecho==1)
            (void) write(i,"\r\n",2);
        if(c==10) processinput(i);
        if(c==8) c=127;
        if(c==-9) c=127;
        if(c==127 && bufpos[i]>0) {
            if(users[i].localecho==1) (void) write(i,"\010 \010",3);
            bufpos[i]--;
            buffer[i][bufpos[i]]=0;
        }
	if(c<32 || c>126) c=0;
        if(bufpos[i]<250 && c!=0) {
            buffer[i][bufpos[i]]=c;
	    if (((users[i].doing>3)&&(users[i].doing<9))||(users[i].doing==2))
		c=42;
	    if (users[i].localecho==1) {
		(void) write(i,&c,1);
		users[i].cposn++;
		if (users[i].cposn==(users[i].ttywidth-1)) {
		    if ((users[i].flags&0x40000)!=0)
			write(i,"\r\n",2);
		    users[i].cposn=0;};}
	    bufpos[i]++;
	    buffer[i][bufpos[i]]=0;
        }
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

LostConnect(path)
int   path;
{
    int i,j,n;
    if ((users[path].doing<4)||(users[path].doing==7)||(users[path].doing==8)) {
	if (users[path].number>-1)
	    if ((users[path].flags&1)==0) {
		unames[users[path].number][0]=0;
		cnames[users[path].number][0]=0;
	    }
	KillConnect(path);
	return;
    }
    if (users[path].number>-1) {
	if (users[path].doing<1000) {
	    (void) sprintf(stringo,"%s leaves the house.\n",users[path].name);
	    moveuser(path,0,stringo,"");
	    (void) sprintf(stringo,"\007\007%s has left the house.\n",users[path].name);
	    for (i=Firstuser;i!=-1;i=users[i].nextuser) 
		if ((i!=path)&&(users[i].doing<1000)) {
		    int k,f;
		    if ((k=onlist(i,users[path].number))>-1) {
			f=users[i].inviteflags[k];
			if ((users[path].flags&1)==0) {
			    users[i].inviteflags[k]=0;
			    users[i].invitenum[k]=-1;};
			if ((f&1)==0) k=-1;
		    };
		    if (k!=-1) {
			rshow(i,stringo);
		    } else {
			if ((users[i].flags&0x8000)!=0) {
			    if ((users[i].flags&0x10000)==0) {
				rshow(i,stringo);
			    } else {
				rshow(i,&stringo[2]);
			    }
			} else {
			    if (users[path].room==users[i].room)
				rshow(i,&stringo[2]);
			}
		    }
		}
	}
	for (i=0;i<INVLENGTH;i++)
	    if (users[path].invitenum[i]!=-1) 
		if ((j=locateusernum(users[path].invitenum[i]))>-1) 
		    if ((users[j].flags&1)==0) {
			users[path].invitenum[i]=-1;
			users[path].inviteflags[i]=0;
		    }
	(void) sprintf(stringp,"%s has logged out so the mindscape vanishes.\n",users[path].name);
	for (i=users[path].scapeuser;i!=-1;i=n) {
	    n=users[i].nextroom;
	    if (i!=path) {
		rshow(i,stringp);
		(void) sprintf(stringq,"%s arrives amidst a shower of sparks.\n",users[i].name);
		moveuser(i,1000000+i,"",stringq);
	    }
	}
	saveuser(path,1);
    }
    KillConnect(path);
}

/****************************************************************************
 ****************************************************************************/

KillConnect(path)
int   path;
{
    int l;
    rshow(path,"Closing connection, Bye.\n");
    (void) close(path);
    readmask[path/32] &= ~(1 << (path&31));
    if (users[path].objecting!=-1) {
	for (l=0;l<OBJECTNUM;l++)
	    if (users[users[path].objecting].objected[l]==users[path].number)
		users[users[path].objecting].objected[l]=-1;
    }
    for (l=0;l<OBJECTNUM;l++) {
	int k;
	k=findusernum(users[users[path].objecting].objected[l]);
	if (k>=0) {
	    users[k].objecting=-1;
	    users[k].objection[0]=0;
	}
    }
    if (users[path].logintype==1) Lassusers--;
    moveuser(path,0,"","");
    if (users[path].lastuser!=-1)
        users[users[path].lastuser].nextuser=users[path].nextuser;
    if (users[path].nextuser!=-1)
        users[users[path].nextuser].lastuser=users[path].lastuser;
    if (Firstuser==path)
        Firstuser=users[path].nextuser;
    if (users[path].ednum!=-1) {
	edits[users[path].ednum].user=-1;
	users[path].ednum=-1;};
    users[path].title[0]=0;
    users[path].number=-1; /* -2 being No name, but loggin in */
    users[path].name="* Empty *";
    users[path].title[0]=0;
    users[path].ltitle[0]=0;
    users[path].desc[0]=0;
    users[path].flags=0;
    users[path].tempflags=0;
    users[path].age=0;
    users[path].lastlogin=0;
    users[path].lastlogout=0;
    users[path].prompt[0]=0;
    users[path].cprompt[0]=0;
    users[path].tprompt[0]=0;
    users[path].myprompt[0]=0;
    users[path].passwd[0]=0;
    users[path].oldpasswd=0;
    users[path].email[0]=0;
    users[path].comment[0]=0;
    users[path].scapename[0]=0;
    users[path].scapedesc[0]=0;
    users[path].scapeuser=-1;
    for(l=0;l<INVLENGTH;l++) {
        users[path].invitenum[l]=-1;
        users[path].inviteflags[l]=0;};
    users[path].room=0;
    users[path].nextuser=-1;
    users[path].lastuser=-1;
    users[path].nextroom=-1;
    users[path].lastroom=-1;
    users[path].leaving=0;
    users[path].idletime=0;
    users[path].logintype=0;
    users[path].doing=0;
    users[path].host[0]=0;
    users[path].tty[0]=0;
    users[path].plevel=0;
    users[path].ttyheight=NORMALHEIGHT;
    users[path].ttywidth=NORMALWIDTH;
    users[path].objecting=-1;
    users[path].objection[0]=0;
    for (l=0;l<OBJECTNUM;l++)
	    users[path].objected[l]=-1;
    through[path]=0;
    time((time_t*)&startthro[path]);
}

/****************************************************************************
 ****************************************************************************/

processinput(i)
int i;
{
    int ppos,spos,first,j;
    char c;
    char prstat[80];
    if (users[i].number<0) {
	pstatus("Cheeseplants House: Processing: -- Unknown --");
    } else {
	sprintf(prstat,"Cheeseplants House: Processing: %s",users[i].name);
	pstatus(prstat);
    }
    (void) time((time_t*)&ti);
    users[i].idletime=ti;
    buffer[i][bufpos[i]]=0;
    bufpos[i]=0;
    for(parc=0;parc<4;parc++) {
        parv[parc][0]=0;
        parr[parc][0]=0;};
    parc=0;
    ppos=0;
    spos=0;
    first=0;
    for(;((c=buffer[i][spos])!=0)&&(parc<4);spos++) {
        if (c==32) {
            if(ppos!=0) {
                ppos=0;
                parc++;
                if (parc==4) break;
            }
        } else {
            if (ppos==0)
                for(j=0;buffer[i][spos+j]!=0;j++) {
                    parr[parc][j]=buffer[i][spos+j];
                    parr[parc][j+1]=0;};
            if (c>='A' && c<='Z') c=c+32;
            parv[parc][ppos]=c;
	    if ((parc==0&&ppos==0)&&(users[i].doing==100)&&(first==0)) {
		switch (c) {
		case ':' : (void) strcpy(parv[0],"emote");parr[0][0]=0;
		    parc++;ppos--;first++;break;
		case ';' : (void) strcpy(parv[0],"emote");parr[0][0]=0;
		    parc++;ppos--;first++;break;
		case '>' : (void) strcpy(parv[0],"tell");parr[0][0]=0;
		    parc++;ppos--;first++;break;
		case '\'' : (void) strcpy(parv[0],"say");parr[0][0]=0;
		    parc++;ppos--;first++;break;
		case '"' : (void) strcpy(parv[0],"say");parr[0][0]=0;
		    parc++;ppos--;first++;break;
		case '`' : (void) strcpy(parv[0],"say");parr[0][0]=0;
		    parc++;ppos--;first++;break;
		case '<' : (void) strcpy(parv[0],"remote");parr[0][0]=0;
		    parc++;ppos--;first++;break;
		case '=' : (void) strcpy(parv[0],"whisper");parr[0][0]=0;
		    parc++;ppos--;first++;break;
		case '/' : parv[0][0]=0;parr[0][0]=0;
		    ppos--;first++;break;
		case '\\' : parv[0][0]=0;parr[0][0]=0;
		    ppos--;first++;break;
		}
	    }
            ppos++;
	    if ((first==0)&&((users[i].flags&0x40000000)!=0)&&(users[i].doing==100)) {
		(void) strcpy(parr[1],parr[0]);
		(void) strcpy(parv[0],"say");
		parr[0][0]=0;
		parc++;first++;}
	    first++;
            parv[parc][ppos]=0;
        }
    }
    if (parv[parc][0]!=0) parc++;
    users[i].plevel++;
    switch (users[i].doing) {
    case 1   : getusername(i);break;
    case 2   : getpassword(i);break;
    case 3   : wantresident(i);break;
    case 4   : wantoldpass(i);break;
    case 5   : wantnewpass(i);break;
    case 6   : wantnewpassa(i);break;
    case 7   : wanthousepass(i);break;
    case 8   : ejectmyself(i);break;
    case 100 : normalcommand(i);break;
    case 200 : (void) editcommand(i);break;
    case 201 : (void) editcommand(i);break;
    case 202 : (void) editcommand(i);break;
    case 203 : (void) editcommand(i);break;
    case 1000: prelogmenu(i);break;
    case 1001: prehitreturn(i);break;
    case 1002: prehitreturn(i);break;
    case 1003: prehitreturn(i);break;
    case 1004: prehitreturn(i);break;
    case 1202: (void) editcommand(i);break;
    case 1203: (void) editcommand(i);break;
    }

    buffer[i][0]=0;
    switch (users[i].doing) {
    case 1 : (void) strcpy(users[i].myprompt,"Please enter your name:");break;
    case 2 : (void) strcpy(users[i].myprompt,"Please enter your password:");break;
    case 3 : (void) strcpy(users[i].myprompt,"Please enter (y/n):");break;
    case 4 : (void) strcpy(users[i].myprompt,"Enter OLD Password        :");break;
    case 5 : (void) strcpy(users[i].myprompt,"Enter NEW Password        :");break;
    case 6 : (void) strcpy(users[i].myprompt,"Enter NEW Password (Again):");break;
    case 7 : (void) strcpy(users[i].myprompt,"Enter Old House Password  :");break;
    case 8 : (void) strcpy(users[i].myprompt,"You're already here: Enter password to clear calls:");break;
    case 100 : if (((users[i].flags&0x40000000)!=0)&&(users[i].cprompt[0]!=0)){
	(void) strcpy(users[i].myprompt,users[i].cprompt);break;
    } else {
	(void) strcpy(users[i].myprompt,users[i].prompt);break;
    }
    case 200 : (void) strcpy(users[i].myprompt,"+");break;
    case 201 : (void) strcpy(users[i].myprompt,"=");break;
    case 202 : (void) strcpy(users[i].myprompt,"#");break;
    case 203 : (void) strcpy(users[i].myprompt,"#");break;
    case 1000: (void) strcpy(users[i].myprompt,"Option: ");break;
    case 1004:
    case 1003:
    case 1002:
    case 1001: (void) strcpy(users[i].myprompt,"Type ok <return>");break;
    case 1202: (void) strcpy(users[i].myprompt,"#");break;
    case 1203: (void) strcpy(users[i].myprompt,"#");break;
    }
    rprint(i,users[i].myprompt);
    users[i].plevel=0;
}

/****************************************************************************
 ****************************************************************************/

TLog(i,s)
int i;
char *s;
{
    int j;
    if (users[i].number<0) {
	(void) sprintf(stringo,"(%.3d) --- Unknown! --- %s\n",i,s);
    } else {
	(void) sprintf(stringo,"(%.3d) %-16s %s\n",i,users[i].name,s);
    }
    if (houseflags&2) {
	for (j=Firstuser;j!=-1;j=users[j].nextuser)
	    if (users[j].flags&512) {
		rshow(j,stringo);
	    }
    }
/*    Logfile(stringo);*/
}

/****************************************************************************
 ****************************************************************************/

TSubNeg(i,c,n)
int i,n;
char c;
{
    int u;
    switch (n) {
    case 0 : 
	switch (c) {
	case -1:
	    tstate[i]=7;return 0;
        default:
	    tsbuff[i][tsposn[i]]=c;
	    tsposn[i]++;
	    if (tsposn[i]==32) tsposn[i]=31;

            return 0;
	}
    case 1 :
	switch (c) {
	case -1:
	    tsbuff[i][tsposn[i]]=-1;
	    tsposn[i]++;
	    if (tsposn[i]==32) tsposn[i]=31;
	    tstate[i]=6;
            return 0;
	case -16:
	    tsbuff[i][tsposn[i]]=0;
	    switch(tsbuff[i][0]) {
	    case 23: 
		if (tsbuff[i][1]==0) 
		    (void) strcpy(users[i].host,&tsbuff[i][2]);
		break;
	    case 24: 
		if (tsbuff[i][1]==0) 
		    (void) strcpy(users[i].tty,&tsbuff[i][2]);
		break;
	    default:
		(void) strcpy(stringp,"Subneg ");
		for (u=0;u<tsposn[i];u++) {
		    (void) sprintf(stringo,"%d ",(int) tsbuff[i][u]);
		    (void) strcat(stringp,stringo);
		}
		(void) strcat(stringp,"\n");
		TLog(i,stringp);
		(void) strcpy(stringp,"ASCII ");
		for (u=0;u<tsposn[i];u++) {
		    if ((tsbuff[i][u]<' ')||(tsbuff[i][u]>'~')) tsbuff[i][u]='.';
		    (void) sprintf(stringo,"%c",tsbuff[i][u]);
		    (void) strcat(stringp,stringo);
		}
		(void) strcat(stringp,"\n");
		TLog(i,stringp);
	    }
	    tstate[i]=0;
	    return 0;
	default:
	    TLog(i,"BUGGERED UP SUBNEGOTIATION.. 8-(\n");
	    tstate[i]=0;
	    return 0;
	}
    }
    TLog(i,"VERY BUGGERED UP SUBNEGOTIATION.. 8-(\n");
    tstate[i]=0;
    return 0;
}

/****************************************************************************
 ****************************************************************************/

TWill(i,c)
int i;
char c;
{
    switch (c) {
    case 24 :/* Terminal Type!!! */
	(void) write(i,"\377\372\030\001\377\360",6);
	break;
    default:
	TReport(i,"WILL",c);
    }
    tstate[i]=0;
    return 0;
}

/****************************************************************************
 ****************************************************************************/

TWont(i,c)
int i;
char c;
{
    switch (c) {
    case 24 :/* Terminal Type!!! */
	(void) strcpy(users[i].tty,"Wont Tell Me!");
	break;
    default:
	TReport(i,"WONT",c);
    }
    tstate[i]=0;
    return 0;
}

/****************************************************************************
 ****************************************************************************/

TDo(i,c)
int i;
char c;
{
    switch (c) {
    case 1 :
	users[i].localecho=1;
	(void) write(i,"\377\373\001",3);
	break;
    case 3 :
	/* CHARACTER MODE */
	tflags[i]|=1;
	break;
    default:
	TReport(i,"DO",c);
    }
    tstate[i]=0;
    return 0;
}

/****************************************************************************
 ****************************************************************************/

TDont(i,c)
int i;
char c;
{
    TReport(i,"DONT",c);
    tstate[i]=0;
    return 0;
}

/****************************************************************************
 ****************************************************************************/

TReport(i,s,c)
int i;
char *s;
char c;
{
    switch(c) {
    case 0 :TMsg(s,"TX-BINARY");break;
    case 1 :TMsg(s,"ECHO");break;
    case 2 :TMsg(s,"PREPARE-RECONNECT");break;
    case 3 :TMsg(s,"CHARACTER-MODE");break;
    case 4 :TMsg(s,"MESSAGE-SIZE");break;
    case 5 :TMsg(s,"STATUS");break;
    case 6 :TMsg(s,"TIMING-MARK");break;
    case 7 :TMsg(s,"RC-TRANS-AND-ECHO");break;
    case 8 :TMsg(s,"NA-O/P-LINE-WIDTH");break;
    case 9 :TMsg(s,"NA-O/P-PAGESIZE");break;
    case 10:TMsg(s,"NA-CR-DISPOSITION");break;
    case 11:TMsg(s,"NA-HORIZ-TABSTOPS");break;
    case 12:TMsg(s,"NA-HORIZ-TAB-DISPOSITION");break;
    case 13:TMsg(s,"NA-FORMFEEDS");break;
    case 14:TMsg(s,"NA-VERT-TABSTOPS");break;
    case 15:TMsg(s,"NA-VERT-TAB-DISPOSITION");break;
    case 16:TMsg(s,"NA-LF-DISPOSITION");break;
    case 17:TMsg(s,"EXTENDED-ASCII");break;
    case 18:TMsg(s,"FORCED-LOGOUT");break;
    case 19:TMsg(s,"BYTE-MACRO");break;
    case 20:TMsg(s,"DATA-ENTRY-TERMINAL");break;
    case 21:TMsg(s,"SUPDUP-PROTOCOL");break;
    case 22:TMsg(s,"SUPDUP-O/P");break;
    case 23:TMsg(s,"SEND-LOCATION");break;
    case 24:TMsg(s,"TERMINAL-TYPE");break;
    case 25:TMsg(s,"END-OF-RECORD");break;
    case 27:TMsg(s,"OUTMARK");break;
    case 28:TMsg(s,"TTYLOC");break;
    case 30:TMsg(s,"X.3-PAD");break;
    case 31:TMsg(s,"NAWS");break;
    case 33:TMsg(s,"TOGGLE-FLOW-CONTROL");break;
    case 34:TMsg(s,"LINEMODE");break;
    case 35:TMsg(s,"X-DISPLAY");break;
    default:
	(void) sprintf(stringp,"%s %d.",s,(int)c);
    }
    TLog(i,stringp);
}

/****************************************************************************
 ****************************************************************************/

TMsg(s,t)
char *s,*t;
{
    (void) sprintf(stringp,"%s %s",s,t);
}

/****************************************************************************
 ****************************************************************************/

files(i)
int i;
{
    int j;
    for (j=0;j<256;j++) {
	stringo[j]='0';
	if (readmask[j/32]&(1<<(j&31))) stringo[j]='1';
    }
    stringo[j]='\n';    
    stringo[j+1]=0;
    rshow(i,stringo);
    for (j=0;j<MOST;j++) {
	stringo[j]='0';
	if (users[j].doing!=0) stringo[j]='1';
    }
    stringo[j]='\n';    
    stringo[j+1]=0;
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

throughput(i)
int i;
{
    int j;
    int t;
    time ((time_t*)&ti);
    if ((parv[1][0]!=0)&&(parv[1][0]!='-')) {
	if ((j=scanuser(parv[1]))<0) return j;
	t=ti-startthro[j];
	if (users[j].number<0) {
		sprintf(stringo,"%.3d ---------------- %8d %f char/sec\n",j,through[j],(double) through[j]/t);
	    } else {
		sprintf(stringo,"%.3d %-16s %8d %f char/sec\n",j,users[j].name,through[j],(double) through[j]/t);
	}	    
	if (parv[2][0]=='-') {
	    time((time_t*)&startthro[j]);
	    through[j]=0;
	}
	rshow(i,stringo);
	return 0;
    }
    for (j=0;j<MOST;j++) {
	if (through[j]>0) {
	    t=ti-startthro[j];
	    if (users[j].number<0) {
		sprintf(stringo,"%.3d ---------------- %8d %f char/sec\n",j,through[j],(double) through[j]/t);
	    } else {
		sprintf(stringo,"%.3d %-16s %8d %f char/sec\n",j,users[j].name,through[j],(double) through[j]/t);
	    }	    
	    rshow(i,stringo);
	}
	if (parv[1][0]=='-') {
	    through[j]=0;
	    time((time_t*)&startthro[j]);
	}
    }	
    return 0;
}
