#include "header.h"

/****************************************************************************
 ****************************************************************************/

editors(i)
int i;
{
    int j,k;
    for (j=0;j<MAXEDITS;j++) {
	if ((k=edits[j].user)==-1) {
	    (void) sprintf(stringo," %2d : *** This edit slot currently unused! ***\n",j);
	} else {
	    (void) sprintf(stringo," %2d : %s %.55s\n",j,users[k].name,users[k].title);
	}
	rshow(i,stringo);
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

allusers(i)
int i;
{
    int n,m;
    m=0;
    for (n=0;n<MAXUSERS;n++) {
	if (unames[n][0]!=0) {
	    if (!(m&3)) stringp[0]=0;
	    m=(m+1)&3;
	    sprintf(stringo,"%-15s%4d%c",cnames[n],n,(m==0)?'\n':' ');
	    strcat(stringp,stringo);
	    if ((m&3)==0) rshow(i,stringp);
	}
    }
    if (m!=0) {
	strcat(stringp,"\n");
	rshow(i,stringp);
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

emails(i)
int i;
{
    int j,count,start,done;
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=Firstuser;j!=-1;j=users[j].nextuser) {
	count++;
	if ((count>start)&&(done<users[i].ttyheight)) {
	    done++;
	    (void) sprintf(stringo," %-16s: %s\n",users[j].name,users[j].email);
	    rshow(i,stringo);
	}
    }
    if (done==0) return -2;
    (void) sprintf(stringo,"Users %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'emails %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

comments(i)
int i;
{
    int j,count,start,done;
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=Firstuser;j!=-1;j=users[j].nextuser) {
	count++;
	if ((count>start)&&(done<users[i].ttyheight)) {
	    done++;
	    (void) sprintf(stringo," %-16s: %s\n",users[j].name,users[j].comment);
	    rshow(i,stringo);
	}
    }
    if (done==0) return -2;
    (void) sprintf(stringo,"Users %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'comments %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

ttys(i)
int i;
{
    int j,count,start,done;
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=Firstuser;j!=-1;j=users[j].nextuser) {
	count++;
	if ((count>start)&&(done<users[i].ttyheight)) {
	    done++;
	    (void) sprintf(stringo," %-16s: %s\n",users[j].name,users[j].tty);
	    rshow(i,stringo);
	}
    }
    if (done==0) return -2;
    (void) sprintf(stringo,"Users %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'ttys %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

email(i)
int i;
{
    int j,k;
    if ((k=findfile(parv[1]))<0) return k;
    if ((j=locateusernum(k))<0) {
	loadtempuser(k);
	(void) strncpy(users[255].email,parr[2],79);
	(void) sprintf(stringo,"You set the email address:\n %-16s: %s\n",cnames[k],users[255].email);
	rshow(i,stringo);
	savetempuser();
	return 0;
    }
    (void) strncpy(users[j].email,parr[2],79);
    (void) sprintf(stringo,"You set the email address:\n %-16s: %s\n",cnames[k],users[j].email);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

comment(i)
int i;
{
    int j,k;
    if ((k=findfile(parv[1]))<0) return k;
    if ((j=locateusernum(k))<0) {
	loadtempuser(k);
	(void) strncpy(users[255].comment,parr[2],79);
	(void) sprintf(stringo,"You set the comment:\n %-16s: %s\n",cnames[k],users[255].comment);
	rshow(i,stringo);
	savetempuser();
	return 0;
    }
    if ((j=scanuser(parv[1]))<0) return j;
    (void) strncpy(users[j].comment,parr[2],79);
    (void) sprintf(stringo,"You set the comment:\n %-16s: %s\n",cnames[k],users[j].comment);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

saveflag(a)
char *a;
{
    a[3]=0;
    if (strcmp(a,"use")==0) return 1;
    if (strcmp(a,"tit")==0) return 2;
    if (strcmp(a,"pro")==0) return 8;
    if (strcmp(a,"lis")==0) return 64;
    if (strcmp(a,"sna")==0) return 128;
    return -1;
}

/****************************************************************************
 ****************************************************************************/

allow(i)
int i;
{
    int j,k;
    if ((j=scanuser(parv[1]))<0) return j;
    if ((k=saveflag(parv[2]))<0) {
	rshow(i,"What on earth am I supposed to be allowing them to do?\n");
	return 0;}
    if ((users[j].flags&k)!=0) {
	rshow(i,"They are ALREADY saving that!\n");
	return 0;}
    users[j].flags|=k;
    rshow(i,"Ok.. They can now save that.\n");
    return 0;
}
    
/****************************************************************************
 ****************************************************************************/

deny(i)
int i;
{
    int j,k;
    if ((j=scanuser(parv[1]))<0) return j;
    if ((k=saveflag(parv[2]))<0) {
	rshow(i,"What on earth am I supposed to be denying them from doing?\n");
	return 0;}
    if ((users[j].flags&k)==0) {
	rshow(i,"They are NOT already saving that!\n");
	return 0;}
    users[j].flags|=k;
    users[j].flags^=k;
    rshow(i,"Ok.. They can now not save that.\n");
    return 0;
}
    
/****************************************************************************
 ****************************************************************************/

powerflag(a)
char *a;
{
    if (strcmp(a,"texttag")==0)    return 0x00000004;
    if (strcmp(a,"seeshout")==0)   return 0x00000010;
    if (strcmp(a,"trust")==0)      return 0x00000400;
    if (strcmp(a,"earmuffs")==0)   return 0x00001000;
    if (strcmp(a,"hiding")==0)     return 0x00002000;
    if (strcmp(a,"informin")==0)   return 0x00004000;
    if (strcmp(a,"informout")==0)  return 0x00008000;
    if (strcmp(a,"inbeeb")==0)     return 0x00010000;
    if (strcmp(a,"autoprompt")==0) return 0x00020000;
    if (strcmp(a,"linewrap")==0)   return 0x00040000;
    if (strcmp(a,"mindopen")==0)   return 0x00080000;
    if (strcmp(a,"mindlock")==0)   return 0x00100000;
    if (strcmp(a,"shield")==0)     return 0x00200000;
    if (strcmp(a,"seeecho")==0)    return 0x00400000;
    if (strcmp(a,"trace")==0)      return 0x00800000;
    if (strcmp(a,"echoall")==0)    return 0x08000000;
    if (strcmp(a,"evict")==0)      return 0x10000000;    
    if (strcmp(a,"love")==0)       return 0x20000000;
    return -1;
}
    
/****************************************************************************
 ****************************************************************************/

bestow(i)
int i;
{
    int j,k;
    if ((j=scanuser(parv[1]))<0) return j;
    if ((k=powerflag(parv[2]))<0) {
        rshow(i,"What on earth am I supposed to be giving them?\n");
        return 0;}
    if ((users[j].flags&k)!=0) {
        rshow(i,"They can ALREADY do that!\n");
        return 0;}
    users[j].flags|=k;
    rshow(i,"Ok.. They can now do that.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

ban(i)
int i;
{
    int j,k;
    if ((j=scanuser(parv[1]))<0) return j;
    if ((k=powerflag(parv[2]))<0) {
        rshow(i,"What on earth am I supposed to be taking off them?\n")
;
        return 0;}
    if ((users[j].flags&k)==0) {
        rshow(i,"They can NOT do that!\n");
        return 0;}
    users[j].flags|=k;
    users[j].flags^=k;
    rshow(i,"Ok.. They can now not do that.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

gag(i)
int i;
{
    int j;
    if ((j=scanuser(parv[1]))<0) return j;
    if ((users[j].flags&0x800)!=0) {
	rshow(i,"You've already got them gagged!\n");
	return 0;
    }
    users[j].flags|=0x800;
    rshow(j,"Somebody creeps up behind you and ties a gag around your mouth.\n");
    rshow(i,"They have been gagged now..\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

ungag(i)
int i;
{
    int j;
    if ((j=scanuser(parv[1]))<0) return j;
    if ((users[j].flags&0x800)==0) {
        rshow(i,"You've not got them gagged!\n");
        return 0;
    }
    users[j].flags^=0x800;
    rshow(j,"The gag dissolves leaving no traces.\n");
    rshow(i,"Ungagged at your command.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

mega_goto(i)
int i;
{
    int j;
    if ((j=scanuser(parv[1]))<0) return j;
    if (users[i].room==users[j].room) {
	rshow(i,"You're already there!!!\n");
	return 0;
    }
    (void) sprintf(stringp,"%s just isnt here anymore.\n",users[i].name);
    (void) sprintf(stringq,"%s is now here.\n",users[i].name);
    moveuser(i,users[j].room,stringp,stringq);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

jump(i)
int i;
{
    int j;
    if ((j=whichroom(parv[1]))<0) return j;
    if (users[i].room==j) {
	rshow(i,"You're already IN there!\n");
	return 0;
    }
    (void) sprintf(stringp,"%s seems not to be here.\n",users[i].name);
    (void) sprintf(stringq,"%s enters from nowhere in particular.\n",users[i].name);
    moveuser(i,j,stringp,stringq);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

quietjump(i)
int i;
{
    int j;
    if ((j=whichroom(parv[1]))<0) return j;
    if (users[i].room==j) {
	rshow(i,"You're already IN there!\n");
	return 0;
    }
    (void) sprintf(stringp,"%s aint here any more.\n",users[i].name);
    moveuser(i,j,stringp,"");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

move(i)
int i;
{
    int j,k;
    if ((k=scanuser(parv[1]))<0) return k;
    rshow(i,"User ok....\n");
    if ((j=whichroom(parv[2]))<0) return j;
    if (users[k].room==j) {
        rshow(i,"They're already IN there!\n");
        return 0;
    }
    (void) sprintf(stringp,"%s is dragged off somewhere.\n",users[k].name);
    (void) sprintf(stringq,"%s arrives from elsewhere.\n",users[k].name);
    moveuser(k,j,stringp,stringq);
    rshow(i,"Moved them.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

beep(i)
int i;
{
    int j;
    if ((j=scanuser(parv[1]))<0) return j;
    rshow(j,"\007\007\007\007\007\007\007\007");
    if (parv[2][0]!=0) {
	(void) sprintf(stringo,"%s pages you: %s\n",users[i].name,parr[2]);
	rshow(j,stringo);
    }
    rshow(i,"Beeped!\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

openhouse(i)
int i;
{
    int n;
    if (parv[1][0]==0) {
	if (housestatus&1) rshow(i,"House closed to LASS non-residents\n");
	if (housestatus&2) rshow(i,"House closed to Warwick non-residents\n");
	if (housestatus&4) rshow(i,"House closed to Internet non-residents\n");
	return 0;
    } else {
	switch (parv[1][0]) {
	case 'f' : rshow(i,"Open to non-residents ");n=0;break;
	case 'h' : rshow(i,"Closed to non-residents ");n=7;break;
	    default : rshow(i,"Illegal option!\n");return 0;
	}
	switch (parv[2][0]) {
	case 'l' : rshow(i,"from LASS.\n");housestatus&=~1;housestatus|=(n&1);return 0;
	case 'w' : rshow(i,"from warwick.\n");housestatus&=~2;housestatus|=(n&2);return 0;
	case 'i' : rshow(i,"from internet.\n");housestatus&=~4;housestatus|=(n&4);return 0;
	default: rshow(i,"from everywhere.\n");housestatus=n;return 0;
	}
    }
}

/****************************************************************************
 ****************************************************************************/

stats(i)
int i;
{
    int j,count,start,done,f;
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=Firstuser;j!=-1;j=users[j].nextuser) {
	count++;
	if ((count>start)&&(done<users[i].ttyheight)) {
	    done++;
	    f=users[j].flags;
	    (void) sprintf(stringo," %-16sRes Tit Des Pro CPr TPr Lst SNa SDe ETo EAt EAl Evc Trc Tag\n",users[j].name);
	    doflag(f,1,"Res","---",17);
	    doflag(f,2,"Tit","---",21);
	    doflag(f,8,"Pro","---",25);
	    doflag(f,0x40,"Lst","---",29);
	    doflag(f,0x80,"SNa","---",33);
	    doflag(f,0x200,"CP!","---",37);
	    doflag(f,0x20000000,"Lov","---",41);
	    doflag(f,0x8000000,"EAl","---",45);
	    doflag(f,0x10000000,"Evc","---",49);
	    doflag(f,0x800000,"Trc","---",53);
	    doflag(f,0x1000000,"Tag","---",57);
	    doflag(f,0x10,"See","---",61);
	    doflag(f,0x200000,"Shl","---",65);
	    doflag(f,0x2000,"Hid","---",69);
	    doflag(f,0x1000,"Ear","---",73);
	    rshow(i,stringo);
	}
    }
    if (done==0) return -2;
    (void) sprintf(stringo,"Users %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'stats %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

maxlass(i)
int i;
{
    int j;
    double lpm,spl;
    if (parv[1][0]==0) {
	rshow(i,"Showing data on LASS Externals....\n");
	(void) sprintf(stringo,"Current number of LASS externals on:%d\nLASS User limit set up as:%d\n",Lassusers,Maxlassusers);
	rshow(i,stringo);
	(void) time((time_t*)&ti);
	if ((ti-lasschecktime)==0) lasschecktime--;
	lpm=((double)(60*lasstries))/((double)(ti-lasschecktime));
	spl=((double)(ti-lasschecktime))/(double)lasstries;
	sprintf(stringo,"Average Lass attempts per minute: %f\n",lpm);
	rshow(i,stringo);
	sprintf(stringo,"Average Time between attempts   : %f secs.\n",spl);
	rshow(i,stringo);
	sprintf(stringo,"Time between retries should be %d secs\nTime waited: %d Calls Tried %d\n",lasswait,ti-lasschecktime,lasstries);
	rshow(i,stringo);
	if (lasstries>100) {lasschecktime=ti;lasstries=0;};
	return 0;
    }
    j=atoi(parv[1]);
    if (j<=0) j=1;
    Maxlassusers=j;
    (void) sprintf(stringo,"Maximum LASS users set to %d.\n",Maxlassusers);
    rshow(i,stringo);
    return 0;
}
      
/****************************************************************************
 ****************************************************************************/

throat(i)
int i;
{
    int j,k;
    if ((j=scanuser(parv[1]))<0) return j;
    if (parv[2][0]==0) {
	(void) sprintf(stringo,"Shout data for user %s.\n",users[j].name);
	rshow(i,stringo);
	if ((ti-lastshout[users[j].number])>SHOUTTIME) {
	    rshow(i,"They are free to shout again.....\n");
	} else {
	    ltimestr(SHOUTTIME-(ti-lastshout[users[j].number]),stringp);
	    (void) sprintf(stringo,"Next shout in %s.\n",stringp);
	    rshow(i,stringo);
	}
	(void) sprintf(stringo,"Shouts made recently: %d\n",shouts[users[j].number]);
	rshow(i,stringo);
	if (shouts[users[j].number]>=SHOUTMAX) 
	    rshow(i,"Time listed about is in effect... *** SORE THROAT ***\n");
	return 0;
    }
    k=atoi(parv[2]);
    if (k<0) {
	rshow(j,"Your sore throat has been cured!\n");
	shouts[users[j].number]=0;
	rshow(i,"Sore throat lifted.\n");
	return 0;
    }
    k*=60;
    shouts[users[j].number]=SHOUTMAX;
    (void) time((time_t*)&lastshout[users[j].number]);
    lastshout[users[j].number]+=k;
    rshow(j,"Your throat feels all wierd.\n");
    rshow(i,"They have been throated!!!!!\n");
    return 0;
}    

/****************************************************************************
 ****************************************************************************/

chpass(i)
int i;
{
    int j,k;
    if (parv[1][0]==0) {
	rshow(i,"I need a name!\n");
	return 0;
    }
    if (parr[2][0]==0) {
	rshow(i,"I need a password!\n");
	return 0;
    }
    for (j=strlen(parr[2]);j<10;j++) parr[2][j]=0;
    if ((j=findfile(parv[1]))<0) return j;
    if ((k=locateusernum(j))>-1) {
	rshow(i,"Theyre logged in... changing their password....\n");
	(void) strcpy(users[k].passwd,crypt(parr[2],"CP"));
	return 0;
    }
    filechangepass(j,crypt(parr[2],"CP"));
    rshow(i,"File's password entry changed.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

sendchar(i)
int i;
{
    int j,k,l;
    char c;
    j=atoi(parv[1]);
    if ((j==0)&&(parv[1][0]!='0'))
	if ((j=scanuser(parv[1]))<0) return j;
    for (k=0;;) {
	for (;parr[2][k]==' ';k++) if (parr[2][k]==0) return 0;
	for (l=0;parr[2][k]!=' '&&parr[2][k]!=0;l++,k++) stringo[l]=parr[2][k];
	l++;
	stringo[l]=0;
	c=(char) atoi(stringo);
	(void) write(j,&c,1);
	(void) sprintf(stringo,"Sent character %d.\n",(int) c);
	rshow(i,stringo);
	if (parr[2][k]==0) return 0;
    }
}

/****************************************************************************
 ****************************************************************************/

myrename(i)
int i;
{
    int j;
    if (parv[1][0]==0) {
	rshow(i,"I need an old name!\n");
	return 0;
    }
    if (parr[2][0]==0) {
	rshow(i,"I need a new name!\n");
	return 0;
    }
    parr[2][strlen(parv[2])]=0;
    if ((j=findfile(parv[1]))<0) return j;
    (void) strcpy(cnames[j],parr[2]);
    {
	int k;
	for (k=0;cnames[j][k];k++) unames[j][k]=cnames[j][k]|32;
	unames[j][k]=0;
    }
    if (locateusernum(j)>-1) {
	rshow(i,"They're logged in... changing their name....\n");
	(void) strcpy(cnames[j],parr[2]);
	{
	    int k;
	    for (k=0;cnames[j][k];k++) unames[j][k]=cnames[j][k]|32;
	    unames[j][k]=0;
	}
	renhash(j);
	return 0;
    }
    filechangename(j,cnames[j]);
    renhash(j);
    rshow(i,"File's name entry changed.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

eradicate(i)
int i;
{
    int j,k,l;
    if (parv[1][0]=='#') {
	j=atoi(&parv[1][1]);
	if ((j<0)||(j>=MAXUSERS)) {
	    rshow(i,"Bad user number!\n");
	    return 0;
	}
    } else {
	if ((j=findfile(parv[1]))<0) return j;
    }
    sprintf(stringo,"Removing user '%s' #%d.....\n",cnames[j],j);
    rshow(i,stringo);
    vapemail(j);
    filezaplist(j);
    for(l=0;(l<MAXNOTICES)&&(noticenum[l]!=-1);l++) {
	if (noticenum[l]==j) {
	    for(k=l+1;k<MAXNOTICES;k++) {
		noticenum[k-1]=noticenum[k];
		strcpy(noticemsg[k-1],noticemsg[k]);
	    }
	    noticenum[MAXNOTICES-1]=-1;
	    noticemsg[MAXNOTICES-1][0]=0;
	    l=MAXNOTICES;
	    rshow(i,"There was a notice which has now been removed.\n");
	}
    }
    if (filekill(j)<0) {
	rshow(i,"That user isnt resident anyway!!!!\n");
    } else {
	rshow(i,"Done....\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

whopop(i)
int i;
{
    int j;
    if (parv[1][0]==0) return 0;
    if ((j=scanuser(parv[1]))<0) return j;
    if (Firstuser==j) {
	rshow(i,"Already at top!\n");
	return 0;
    }
    if (users[j].nextuser!=-1) 
	users[users[j].nextuser].lastuser=users[j].lastuser;
    if (users[j].lastuser!=-1) 
	users[users[j].lastuser].nextuser=users[j].nextuser;
    users[j].nextuser=Firstuser;
    users[j].lastuser=-1;
    users[Firstuser].lastuser=j;
    Firstuser=j;
    rshow(i,"Popped!!\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

hostinfo(i)
int i;
{
    int j,count,start,done;
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=start;(hostlist[j].s_addr!=0)&&(done<users[i].ttyheight)&&(j<MAXHOSTS);j++) {
	done++;
	{
	    unsigned long ina=hostlist[j].s_addr;
	    unsigned char *b=(unsigned char *)&ina;
	    (void) sprintf(stringo,"%.4d %.3d.%.3d.%.3d.%.3d %-20s",j,
			   b[0], b[1], b[2], b[3],
			   hostlabel[j]);
	}
	switch(hoststatus[j]) {
	case 1:
	    strcat(stringo,"  RST\n");break;
	case 3:
	    strcat(stringo,"  RST BAR\n");break;
	case 2:
	    strcat(stringo,"      BAR\n");break;
	default:
	    strcat(stringo,"\n");break;
	}
	rshow(i,stringo);
    }
    if (done==0) return -2;
    (void) sprintf(stringo,"Hosts %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more on 'hostinfo %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

hostadd(i)
int i;
{
    struct hostent* h;
    struct in_addr* a;
    struct in_addr p;
    int j;
    h=gethostbyname(parr[1]);
    if (h==0) {
	int aa,ab,ac,ad;
	if(sscanf(parr[1],"%d.%d.%d.%d",&aa,&ab,&ac,&ad)!=4) {
	    rshow(i,"Host name not recognised.\n");
	    return 0;
	}
	a=(struct in_addr*)&p;
	{
	    unsigned long ina=hostlist[j].s_addr;
	    unsigned char *b=(unsigned char *)&ina;

	    b[0]=(char) aa;
	    b[1]=(char) ab;
	    b[2]=(char) ac;
	    b[3]=(char) ad;
	}
    } else {
	a=(struct in_addr*) h->h_addr_list[0];
    }
    if (a==0) {
	rshow(i,"Host address not returned.\n");
	return 0;
    }
    for (j=0;(hostlist[j].s_addr!=0)&&(j<MAXHOSTS);j++)
	if (hostlist[j].s_addr==a->s_addr) {
	    sprintf(stringo,"Already on list as number %d.\n",j);
	    rshow(i,stringo);
	    return 0;};
    if (j==MAXHOSTS) {
	rshow(i,"Too many hosts on list.\n");
	return 0;
    }
    hostlist[j].s_addr=a->s_addr;
    hoststatus[j]=0;
    strncpy(hostlabel[j],parr[1],19);
    sprintf(stringo,"Host added to list as entry %d.\n",j);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

hostdo(i)
int i;
{
    struct hostent *h;
    int j;
    if (parv[1][0]==0) {
	rshow(i,"Options are c:Clear r:Restrict b:Bar D:Delete ?:Query");
	return 0;
    }
    if (parv[2][0]==0) {
	rshow(i,"Please give a host number?\n");
	return 0;
    }
    j=atoi(parv[2]);
    if ((j<0)||(j>MAXHOSTS)||(hostlist[j].s_addr==0)) {
	rshow(i,"Thats not a legal host number, see hostinfo.\n");
	return 0;
    }
    switch(parv[1][0]) {
    case '?':
	h=gethostbyaddr(&hostlist[j],4,AF_INET);
	if (h==0) {
	    rshow(i,"Could not be located.\n");
	    return 0;
	}
	sprintf(stringo,"Host is %s.\n",h->h_name);
	strncpy(hostlabel[j],h->h_name,19);
	rshow(i,stringo);
	return 0;
    case 'r':
	hoststatus[j]^=1;
	rshow(i,"Host restriction changed.\n");
	return 0;
    case 'b':
	hoststatus[j]^=2;
	rshow(i,"Host barring changed.\n");
	return 0;
    case 'd':
	for (;(hostlist[j].s_addr!=0)&&(j<(MAXHOSTS-1));j++)
	    hostlist[j].s_addr=hostlist[j+1].s_addr;
	hostlist[MAXHOSTS-1].s_addr=0L;
	rshow(i,"Host deleted.\n");
	return 0;
    default:
	rshow(i,"Illegal option.\n");
	return 0;
    }
}

/****************************************************************************
  FUNCTION - objectto(chan)
 ****************************************************************************/

objectto(i)
int i;
{
    int n,j;
    if (parv[1][0]==0) {
	if (users[i].objecting==-1) {
	    rshow(i,"Errr, WHo do you object to?\n");
	} else {
	    rshow(i,"Removing your objection.\n");
	    for (j=0;j<OBJECTNUM;j++)
		if (users[users[i].objecting].objected[j]==users[i].number)
		    users[users[i].objecting].objected[j]=-1;
	}
	users[i].objecting=-1;
	users[i].objection[0]=0;
	return 0;
    }
    if (parv[2][0]==0) {
	rshow(i,"You must specify your objection.\n");
	return 0;
    }
    if ((n=scanuser(parv[1]))<0) return n;
    if (n==users[i].objecting) {
	rshow(i,"Reason changed.\n");
	strncpy(users[i].objection,parr[2],79);
	return 0;
    }
    if (users[i].objecting!=-1) {
	rshow(i,"Removing your old objection.\n");
	for (j=0;j<OBJECTNUM;j++)
	    if (users[users[i].objecting].objected[j]==users[i].number)
		users[users[i].objecting].objected[j]=-1;
	users[i].objecting=-1;
	users[i].objection[0]=0;
    }
    if (users[n].flags&1) {
	rshow(i,"That user is a resident!!!\n");
	return 0;
    }
    strncpy(users[i].objection,parr[2],79);
    rshow(i,"Objection noted.\n");
    users[i].objecting=n;
    for (j=0;(j<OBJECTNUM)&&(users[n].objected[j]!=-1);j++);
    if (j<OBJECTNUM) users[n].objected[j]=users[i].number;
    for (j=0;(j<OBJECTNUM)&&(users[n].objected[j]!=-1);j++);
    if (j<OBJECTNUM) return 0;
    /* Now evict them */
    (void) time((time_t*)&ti);
    timestr(ti,stringp);
    (void) sprintf(stringo,"%s: %s objected to by:",stringp,users[n].name);
    for (j=0;j<OBJECTNUM;j++) {
	strcat(stringo,cnames[users[n].objected[j]]);
	if (j!=(OBJECTNUM-1)) {
	    strcat(stringo," ");
	} else {
	    strcat(stringo,"\n");
	};
    }
    Logfile(stringo);
    for (j=0;j<OBJECTNUM;j++) {
	int k;
	k=findusernum(users[n].objected[j]);
	if (k<0) {
	    sprintf(stringo,"   %-15s     *** FAULT ***\n",cnames[users[n].objected[j]]);
	} else {
	    sprintf(stringo,"   %-15s:%s\n",cnames[users[n].objected[j]],
		    users[k].objection);
	    users[k].objection[0]=0;
	    users[k].objecting=-1;
	}
	Logfile(stringo);
    }
    rshow(n,"You have been evicted.\n");
    LostConnect(n);
}


