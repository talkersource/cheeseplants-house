#include "header.h"

/****************************************************************************
 ****************************************************************************/

rsay(i,r,s,t,a,b,c)
int i,r,t,a,b,c;
char *s;
{
    int j,l;
    char ekko[1024];
    if (t==3) {
	int k;
	(void) strcpy(ekko,s);
	for (k=strlen(ekko)-1;(k!=0)&&(ekko[k]!='\n');k--);
	(void) sprintf(&ekko[k],"<<%s\n",users[i].name);
    }
    if (s==0) return;
    /* who room str type ig1 ig2 ig3 */
    if (r==0) return;
    if (r<1000000) {
        j=Roomuser[r];
    } else {
        j=users[r-1000000].scapeuser;
    }
    for(;j!=-1;j=users[j].nextroom) {
	if  ((j!=a)&&(j!=b)&&(j!=c)) {
	    if ((l=onlist(j,users[i].number))>=0) 
		if ((users[j].inviteflags[l]&2)==0) l=-1;
	    if (l<0) {
		if ((t==3)&&((users[j].flags&0x400000)!=0)) {
		    rshow(j,ekko);
		} else {
		    rshow(j,s);
		}
	    }
	}
    }
    return;
}

/****************************************************************************
 ****************************************************************************/

rsayb(i,r,s,t,a,b,c)
int i,r,t,a,b,c;
char *s;
{
    int j,l;
    char ekko[1024];
    if (t==3) {
	int k;
	(void) strcpy(ekko,s);
	for (k=strlen(ekko)-1;(k!=0)&&(ekko[k]!='\n');k--);
	(void) sprintf(&ekko[k],"<<%s\n",users[i].name);
    }
    if (s==0) return;
    /* who room str type ig1 ig2 ig3 */
    if (r==0) return;
    if (r<1000000) {
        j=Roomuser[r];
    } else {
        j=users[r-1000000].scapeuser;
    }
    for(;j!=-1;j=users[j].nextroom) {
	if  ((j!=a)&&(j!=b)&&(j!=c)) {
	    if ((l=onlist(j,users[i].number))>=0) 
		if ((users[j].inviteflags[l]&2)==0) l=-1;
	    if (l<0) {
		if ((t==3)&&((users[j].flags&0x400000)!=0)) {
		    rtell(j,ekko);
		} else {
		    rtell(j,s);
		}
	    }
	}
    }
    return;
}

/****************************************************************************
 ****************************************************************************/

rtell(i,s)
int i;
char* s;
{
    if (users[i].flags&4) {
	rshow(i,s);
    } else {
	rshow(i,&s[1]);
    }
}

/****************************************************************************
 ****************************************************************************/

say(i)
int i;
{
    if (parr[1][0]==0) return 0;
    if ((users[i].flags&0x800)!=0) {
	rshow(i,"You cant talk through the gag!\n");
	return 0;}
    (void) sprintf(stringo,"=%s says \"%s\"\n",users[i].name,parr[1]);
    rsayb(i,users[i].room,stringo,1,i,-1,-1);
    (void) sprintf(stringo,"You say \"%s\"\n",parr[1]);
    rshow(i,stringo);
    return 0;
}
    
/****************************************************************************
 ****************************************************************************/

emote(i)
int i;
{
    int j,k,l;
    char a[5];
    if (parr[1][0]==0) return 0;
    if (strcmp(parv[1],"tells")==0) 
	if (strcmp(parv[2],"you:")==0) {
	    rshow(i,"You cant do that...\n");
	    return 0;
	}
    if (strcmp(parv[1],"whispers")==0) {
	rshow(i,"You cant do that...\n");
	return 0;
    }
    if ((users[i].flags&0x800)!=0) {
        rshow(i,"You cant communicate through the gag!\n");
        return 0;}
    for (j=strlen(parr[1]);j!=0;j--) if (parr[1][j]=='|') {
	l=j;
	if (l==0) return 0;
	for (j++;parr[1][j]!=0;j++) if (parr[1][j]!=' ') {
	    for (k=0;parr[1][j+k]!=0;k++)
		stringo[k]=parr[1][j+k];
	    stringo[k]=0;
	    if (k>15) k=15;
	    for (;k>=0;k--) {
		if ((stringo[k]>='A')&&(stringo[k]<='Z')) stringo[k]+=32;
		if ((stringo[k]<'a')||(stringo[k]>'z')) stringo[k]=0;
	    }
	    if ((j=scanuser(stringo))<0) return j;
	    if (users[i].room!=users[j].room) {
		rshow(i,"They're not in here!!!\n");
		return 0;};
	    if (j==i) {
		rshow(i,"Why bother... thats YOU!\n");
		return 0;};
	    parr[1][l]=0;
	    (void) strcpy(stringo,parr[1]);
	    l=0;
	    for(k=0;k<5;k++) a[k]=0;
	    for(k=0;(k==0)||(stringo[k-1]!=0);k++) {
		a[0]=a[1];a[1]=a[2];a[2]=a[3];a[3]=a[4];
		a[4]=(stringp[l]=stringo[k])|32;
		if (a[1]=='a'&&a[2]=='r'&&a[3]=='e'&&((a[4]<'a')||(a[4]>'z'))){
		    (void) strcpy(&stringp[l-3],"is");
		    l--;
		    stringp[l]=stringo[k];
		}
		if (a[1]=='y'&&a[2]=='o'&&a[3]=='u'&&((a[4]<'a')||(a[4]>'z'))){
		    (void) strcpy(&stringp[l-3],users[j].name);
		    l+=(strlen(users[j].name)-3);
		    stringp[l]=stringo[k];
		}
		if (a[0]=='y'&&a[1]=='o'&&a[2]=='u'&&a[3]=='r'&&((a[4]<'a')||(a[4]>'z'))){
		    (void) strcpy(&stringp[l-4],users[j].name);
		    (void) strcat(stringp,"'s");
		    l+=(strlen(users[j].name)-2);
		    stringp[l]=stringo[k];
		}
		l++;
	    }
	    (void) sprintf(stringq,"-%s %s\n",users[i].name,parr[1]);
	    (void) sprintf(stringo,"=%s %s\n",users[i].name,stringp);
	    rsayb(i,users[i].room,stringo,2,i,j,-1);
	    rtell(j,stringq);
	    (void) sprintf(stringp,"You emote: %s&  remote: %s",&stringo[1],&stringq[1]);
	    rshow(i,stringp);
	    return 0;
	}
	parr[1][l]=0;
	break;
    }
    (void) sprintf(stringo,"=%s %s\n",users[i].name,parr[1]);
    rsayb(i,users[i].room,stringo,2,i,-1,-1);
    (void) sprintf(stringo,"You emote: %s %s\n",users[i].name,parr[1]);
    rshow(i,stringo);
    return 0;
}
    
/****************************************************************************
 ****************************************************************************/

mytell(i)
int i;
{
    int j,k;
    if (parr[2][0]==0) return 0;
    if ((j=scanuser(parv[1]))<0) return j;
    if (shielded(j,i)) {
	rshow(i,"You're shielded from tells from that person!\n");
	return 0;};
    if (!(users[i].flags&0x200))
	if ((k=onlist(j,users[i].number))>=0) 
	    if ((users[j].inviteflags[k]&2)!=0) {
		rshow(i,users[j].ignoremsg);
		return 0;
	    }
    if (!shielded(i,j)) {
	(void) sprintf(stringo,"You tell %s: %s\n",users[j].name,parr[2]);
	rshow(i,stringo);
	(void) sprintf(stringo,">%s tells you: %s\n",users[i].name,parr[2]);
	rtell(j,stringo);
	return 0;
    }
    (void) sprintf(stringo,"%s is shielded from tells and remotes.\n",users[j].name);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

remote(i)
int i;
{
    int j,k;
    if (parr[2][0]==0) return 0;
    if (strcmp(parv[1],"says")==0) {
	rshow(i,"You cant do that...\n");
	return 0;
    }
    if ((users[i].flags&0x800)!=0) {
        rshow(i,"You cant communicate through the gag!\n");
        return 0;}
    if ((j=scanuser(parv[1]))<0) return j;
    if (shielded(j,i)) {
	rshow(i,"You're shielded from remotes from that person!\n");
	return 0;};
    if (!(users[i].flags&0x200))
	if ((k=onlist(j,users[i].number))>=0) {
	    if ((users[j].inviteflags[k]&2)!=0) {
		rshow(i,users[j].ignoremsg);
		return 0;
	    }
	}
    if (!shielded(i,j)) {
	(void) sprintf(stringo,"You remote: %s %s\n",users[i].name,parr[2]);
	rshow(i,stringo);
	(void) sprintf(stringo,">%s %s\n",users[i].name,parr[2]);
	rtell(j,stringo);
	return 0;
    }
    (void) sprintf(stringo,"%s is shielded from tells and remotes.\n",users[j].name);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

echoto(i)
int i;
{
    int j,k;
    if (parr[2][0]==0) return 0;
    if ((users[i].flags&0x800)!=0) {
        rshow(i,"You cant communicate through the gag!\n");
        return 0;}
    if ((j=scanuser(parv[1]))<0) return j;
    if (shielded(j,i)) {
        rshow(i,"You're shielded from replies from that person!\n");
        return 0;};
    if (!(users[i].flags&0x200))
	if ((k=onlist(j,users[i].number))>=0) {
	    if ((users[j].inviteflags[k]&2)!=0) {
		rshow(i,users[j].ignoremsg);
		return 0;
	    }
	}
    if (!shielded(i,j)) {
	if (strcmp(unames[users[i].number],parv[2])!=0) {
	    for (k=0;k<MAXUSERS;k++)
		if (strcmp(unames[k],parv[2])==0) {
		    (void) sprintf(stringo,"%s echoto's :%s\n",users[i].name,parr[2]);
		    rshow(j,stringo);
		    (void) sprintf(stringo,"You sorta echoto \'%s\'\n",parr[2]);
		    rshow(i,stringo);
		    return 0;
		}
	}
        (void) sprintf(stringo,"You echoto: %s\n",parr[2]);
        rshow(i,stringo);
	if ((users[j].flags&0x400000)==0) {
	    (void) sprintf(stringo,"%s\n",parr[2]);
	} else {
	    (void) sprintf(stringo,"%s<<-%s\n",parr[2],users[i].name);
	}
        rshow(j,stringo);
        return 0;
    }
    (void) sprintf(stringo,"%s is shielded.\n",users[j].name);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

echo(i)
int i;
{
    int j;
    if (parr[1][0]==0) return 0;
    if ((users[i].flags&0x800)!=0) {
        rshow(i,"You cant communicate through the gag!\n");
        return 0;}
    if (users[i].room==1) {
	rshow(i,"You cannot echo in the gathering place.\n");
	return 0;}
    if (strcmp(unames[users[i].number],parv[1])!=0) {
	for (j=0;j<MAXUSERS;j++)
	    if (strcmp(unames[j],parv[1])==0) {
		(void) sprintf(stringo,"=%s echoes :%s\n",users[i].name,parr[1]);
		rsayb(i,users[i].room,stringo,3,i,-1,-1);
		(void) sprintf(stringo,"You sorta echo \'%s\'\n",parr[1]);
		rshow(i,stringo);
		return 0;
	    }
    }
    (void) sprintf(stringo,"=%s\n",parr[1]);
    rsayb(i,users[i].room,stringo,3,i,-1,-1);
    (void) sprintf(stringo,"You echo \'%s\'\n",parr[1]);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

shout(i)
int i;
{
    int j,l;
    if (parr[1][0]==0) return 0;
    if ((users[i].flags&0x800)!=0) {
        rshow(i,"You cant communicate through the gag!\n");
        return 0;};
    if ((ti-lastshout[users[i].number])>SHOUTTIME) shouts[users[i].number]=0;
    if ((users[i].flags&0x200)!=0) shouts[users[i].number]=0;
    if (shouts[users[i].number]==0) lastshout[users[i].number]=ti;
    shouts[users[i].number]++;
    if (shouts[users[i].number]==(SHOUTMAX+1)) lastshout[users[i].number]+=SHOUTADD;
    if (shouts[users[i].number]>SHOUTMAX) {
	rshow(i,"You have a sore throat from shouting....\n");
	return 0;
    }
    (void) sprintf(stringo,"You shout \"%s\"\n",parr[1]);
    rshow(i,stringo);
    (void) sprintf(stringo,"*%s shouts \"%s\"\n",users[i].name,parr[1]);
    for(j=Firstuser;j!=-1;j=users[j].nextuser) {
	if ((i!=j)&&(users[j].doing<1000))
	    if (((users[j].room==users[i].room)&&(users[j].flags&0x010))||
		((users[j].flags&0x1000)==0)) {
		if ((l=onlist(j,users[i].number))>=0)
		    if ((users[j].inviteflags[l]&2)==0) l=-1;
		if (l<0) 
		    rtell(j,stringo);
	    }
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

echoall(i)
int i;
{
    int j;
    if (parr[1][0]==0) return 0;
    if ((users[i].flags&0x800)!=0) {
        rshow(i,"You cant communicate through the gag!\n");
        return 0;}
    (void) sprintf(stringo,"You echoall: %s\n",parr[1]);
    rshow(i,stringo);
    (void) sprintf(stringo,"%s\n",parr[1]);
    for(j=Firstuser;j!=-1;j=users[j].nextuser) {
	if ((i!=j)&&(users[j].doing<1000)) rshow(j,stringo);
    }
    timestr(ti,stringp);
    (void) sprintf(stringo,"%s: %s echoalled: %s\n",stringp,users[i].name,parr[1]);
    Logfile(stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

shielded(me,you)
int me,you;
{
    /* Are you shielded to me? */
    int k;
    if ((users[me].flags&0x200)!=0) return 0;
    if ((users[you].flags&0x200000)==0) return 0;
    if ((k=onlist(you,users[me].number))<0) return -1;
    return ((users[you].inviteflags[k]&8)==0);
}
    
/****************************************************************************
 ****************************************************************************/

echoat(i)
int i;
{
    int j,k;
    if (parr[2][0]==0) return 0;
    if ((k=scanuser(parv[1]))<0) return k;
    if ((users[i].flags&0x800)!=0) {
        rshow(i,"You cant communicate through the gag!\n");
        return 0;}
    if (shielded(k,i)) {
	rshow(i,"You're shielded from replies from that person!\n");
	return 0;};
    if (!(users[i].flags&0x200))
	if ((j=onlist(k,users[i].number))>=0) {
	    if ((users[k].inviteflags[j]&2)!=0) {
		rshow(i,users[k].ignoremsg);
		return 0;
	    }
	}
    if (!shielded(i,k)) {
	if (strcmp(unames[users[i].number],parv[2])!=0) {
	    for (j=0;j<MAXUSERS;j++)
		if (strcmp(unames[j],parv[2])==0) {
		    (void) sprintf(stringo,"=%s echoes :%s\n",users[i].name,parr[2]);
		    rsayb(i,users[k].room,stringo,3,i,-1,-1);
		    (void) sprintf(stringo,"You sorta echo \'%s\'\n",parr[2]);
		    rshow(i,stringo);
		    return 0;
		}
	}
	(void) sprintf(stringo,"=%s\n",parr[2]);
	rsayb(i,users[k].room,stringo,3,i,-1,-1);
	(void) sprintf(stringo,"You echo \'%s\'\n",parr[2]);
	rshow(i,stringo);
	return 0;
    }
    rshow(i,"They're shielded.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

whisper(i)
int i;
{
    int j,k;
    if (parr[2][0]==0) return 0;
    if ((k=scanuser(parv[1]))<0) return k;
    if ((users[i].flags&0x800)!=0) {
        rshow(i,"You cant communicate through the gag!\n");
        return 0;}
    if (users[i].room!=users[k].room) {
	rshow(i,"They're not in here!\n");
	return 0;};
    if (shielded(k,i)) {
	rshow(i,"You're shielded from replies from that person!\n");
	return 0;};
    if (!(users[i].flags&0x200))
	if ((j=onlist(k,users[i].number))>=0) {
	    if ((users[k].inviteflags[j]&2)!=0) {
		rshow(i,users[k].ignoremsg);
		return 0;
	    }
	}
    if (!shielded(i,k)) {
	(void) sprintf(stringo,"You whisper '%s' to %s\n",parr[2],users[k].name);
	(void) sprintf(stringq,">%s whispers '%s'\n",users[i].name,parr[2]);
	rshow(i,stringo);
	rtell(k,stringq);
	(void)sprintf(stringo,"=%s whispers something to %s.\n",users[i].name,users[k].name);
	rsayb(i,users[k].room,stringo,1,i,k,-1);
	return 0;
    }
    rshow(i,"They're shielded.\n");
    return 0;
}
