#include "header.h"

/****************************************************************************
 ****************************************************************************/

dolflag(i,f,ys,ns)
int i,f;
char *ys,*ns;
{
    int n,j,p;
    if ((n=findfile(parv[1]))<0) return n;
    p=0;
    if (strcmp(parv[2],"on")==0) p=1;
    if (strcmp(parv[2],"off")==0) p=-1;
    if (parv[2][0]=='?') p=2;
    for (j=0;(j<INVLENGTH)&&(n!=users[i].invitenum[j]);j++);
    if (j==INVLENGTH) {
	if ((p==-1)||(p==2)) {
	    (void) sprintf(stringo,"%s is not on your list.\n",cnames[n]);
	    rshow(i,stringo);
	    return 0;
	} else {
	    for (j=0;(j<INVLENGTH)&&(users[i].invitenum[j]!=-1);j++);
	    if (j==INVLENGTH) {
		(void) sprintf(stringo,"Theres not enough room on your list to add %s.\n",cnames[n]);
		rshow(i,stringo);
		return 0;}
	    users[i].invitenum[j]=n;
	    users[i].inviteflags[j]=f;
	    (void) sprintf(stringo,"%s has been added to your list.\n",cnames[n]);
	    rshow(i,stringo);
	    if (f==16) (void) giveinvite(i,n);
	    return 0;
	}
    }
    if (p==2) {
	if ((users[i].inviteflags[j]&f)==0) {
	    (void) sprintf(stringo,ns,cnames[n]);
	} else {
	    (void) sprintf(stringo,ys,cnames[n]);
	}
	rshow(i,stringo);
	return 0;
    }
    if (p==0) {
	p=1;
	if ((users[i].inviteflags[j]&f)!=0) p=-1;
    }
    if (p==1) {
	users[i].inviteflags[j]|=f;
	(void) sprintf(stringo,ys,cnames[n]);
	rshow(i,stringo);
	if (f==16) (void) giveinvite(i,n);
	return 0;}
    users[i].inviteflags[j]|=f;
    users[i].inviteflags[j]^=f;
    if (users[i].inviteflags[j]==0) {
	users[i].invitenum[j]=-1;
	(void) sprintf(stringo,"%s has been removed from your list.\n",cnames[n]);
	rshow(i,stringo);
	return 0;}
    (void) sprintf(stringo,ns,cnames[n]);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

inform(i)
int i;
{
    return(dolflag(i,1,"You will be informed about %s.\n","You will not be informed about %s.\n"));
}

/****************************************************************************
 ****************************************************************************/

ignore(i)
int i;
{
    return(dolflag(i,2,"You are ignoring %s.\n","You aren't ignoring %s.\n"));
}

/****************************************************************************
 ****************************************************************************/

grabme(i)
int i;
{
    return(dolflag(i,4,"You are allowing %s to grab you.\n","%s cannot grab you.\n"));
}

/****************************************************************************
 ****************************************************************************/

noshield(i)
int i;
{
    return(dolflag(i,8,"%s can get through your shield.\n","Your shield will work against %s.\n"));
}

/****************************************************************************
 ****************************************************************************/

invite(i)
int i;
{
    return(dolflag(i,16,"%s is invited to your mindscape.\n","%s is not invited to your mindscape.\n"));
}

/****************************************************************************
 ****************************************************************************/

bar(i)
int i;
{
    return(dolflag(i,32,"%s is barred from your mindscape.\n","%s is not barred from your mindscape.\n"));
}

/****************************************************************************
 ****************************************************************************/

friend(i)
int i;
{
    return(dolflag(i,64,"%s is listed as a friend.\n","%s is not listed as a friend.\n"));
}

/****************************************************************************
 ****************************************************************************/

giveinvite(i,n)
int i,n;
{
    int j;
    if ((j=finduser(unames[n]))<0) return 0;
    (void) sprintf(stringq,"You are invited to %s's mindscape.\n",users[i].name);
    rshow(j,stringq);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

list(i)
int i;
{
    int j,count,start,done;
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    rshow(i,"       ------Name------ Inform Ignore GrabMe NoShld Invite Barred Friend\n");
    for(j=0;j<INVLENGTH;j++) {
	if (users[i].invitenum[j]!=-1) {
	    count++;
	    if ((count>start)&&(done<users[i].ttyheight)) {
		int f;
		done++;
		f=users[i].inviteflags[j];
		(void) sprintf(stringo,"       %-16s  %s   %s   %s   %s   %s   %s   %s\n",cnames[users[i].invitenum[j]],(f&1)==0?"----":"YES!",(f&2)==0?"----":"YES!",(f&4)==0?"----":"YES!",(f&8)==0?"----":"YES!",(f&16)==0?"----":"YES!",(f&32)==0?"----":"YES!",(f&64)==0?"----":"YES!");
		rshow(i,stringo);
	    }
	}
    }
    if (done==0) return -2;
    (void) sprintf(stringo,"Users %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'list %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

onlist(i,j)
int i,j;
{
    int l;
    for (l=0;(l<INVLENGTH)&&(users[i].invitenum[l]!=j);l++);
    if (l==INVLENGTH) return  -1;
    return l;
}

/****************************************************************************
 ****************************************************************************/

invites(i)
int i;
{
    /* See whos mindscape I can get into... */
    int j,k;
    rshow(i,"You can get into the following mindscapes....\n");
    for (j=Firstuser;j!=-1;j=users[j].nextuser)
	if (users[j].doing<1000) {
	    if ((users[j].flags&0x100000)==0) {
		k=onlist(j,users[i].number);
		if (k!=-1) {
		    int l;
		    l=k;
		    if ((users[j].inviteflags[k]&16)==0) l=-1;
		    if ((users[j].inviteflags[k]&32)!=0) l=-2;
		    k=l;
		}
		if (k==-1)
		    if ((users[j].flags&0x80000)!=0) k=99999;
		if (k>=0) {
		    if (k!=99999) {
			(void) sprintf(stringo,"In-%-15s: %.59s\n",users[j].name,users[j].scapename);
		    } else {
			(void) sprintf(stringo,"Op-%-15s: %.59s\n",users[j].name,users[j].scapename);
		    }
		    rshow(i,stringo);
		} 
	    }
	}
    return 0;
}
			 
/****************************************************************************
 ****************************************************************************/

grabs(i)
int i;
{
    int j,k,l;
    l=0;
    for (j=Firstuser;j!=-1;j=users[j].nextuser) {
	k=onlist(j,users[i].number);
	if (k!=-1) {
	    if ((users[j].inviteflags[k]&4)!=0) {
		if (l==0) rshow(i,"You can currently grab the following people.\n");
		l++;
		(void) sprintf(stringo,"%s %s\n",users[j].name,users[j].title);
		rshow(i,stringo);
	    }
	}
    }
    if (l==0) rshow (i,"You cannot grab anyone just now.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

grab(i)
int i;
{
    int j,k;
    if ((j=scanuser(parv[1]))<0) return j;
    if ((users[i].flags&512)==0) {
	if ((k=onlist(j,users[i].number))<0) {
	    rshow(i,"You cannot grab that person.\n");
	    return 0;
	}
	if ((users[j].inviteflags[k]&4)==0) {
	    rshow(i,"You cannot grab that person.\n");
	    return 0;
	}
    }
    if (users[i].room==users[j].room) {
	rshow(i,"But they're HERE!\n");
	return 0;
    }
    (void) sprintf(stringo,"You're grabbed by %s.\n",users[i].name);
    rshow(j,stringo);
    (void) sprintf(stringo,"You grab %s.\n",users[j].name);
    rshow(i,stringo);
    (void) sprintf(stringp,"%s is yanked away by an unseen force.\n",users[j].name);
    (void) sprintf(stringq,"%s spins rapidly into being.\n",users[j].name);
    moveuser(j,users[i].room,stringp,stringq);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

qw(i)
int i;
{
    int j,k,count,start,done;
    char ltype[5];
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=0;j<INVLENGTH;j++) {
	if ((users[i].invitenum[j]!=-1)&&(users[i].inviteflags[j]&64))
	    if ((k=findusernum(users[i].invitenum[j]))>0) {
		count++;
		if ((count>start)&&(done<users[i].ttyheight)) {
		    done++; 		    switch(users[k].logintype) {
		    case 0 : (void) strcpy(ltype,"INet");break;
		    case 1 : (void) strcpy(ltype,"LASS");break;
		    case 2 : (void) strcpy(ltype,"Wwk.");break;
			default : (void) strcpy(ltype,"----");
		    }
		    (void) sprintf(stringo,"%s - %s %s\n",ltype,users[k].name,users[k].title);
		    if ((users[k].flags&1)!=0) stringo[5]='+';
		    if ((stringo[78]!=0)&&(stringo[78]!='\n')) {
			stringo[78]='\n';
			stringo[79]=0;};
		    rshow(i,stringo);
		}
	    }
    }
    if ((done==0)&&(parv[1][0]==0)) {
	rshow(i,"You have no friends on at the moment. *schniff* 8-(\n");
	return 0;}
    if (done==0) return -2;
    (void) sprintf(stringo,"Friends %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'qw %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

qf(i)
int i;
{
    int j,k,count,start,done;
    char ltype[5];
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=0;j<INVLENGTH;j++) {
	if ((users[i].invitenum[j]!=-1)&&(users[i].inviteflags[j]&64))
	    if ((k=findusernum(users[i].invitenum[j]))>0) {
		if (users[k].doing<1000) {
		    if (((users[k].flags&0x2000)==0)||
			((users[i].flags&0x200)!=0)||
			(i==k)&&(users[k].room!=0)) {
			count++;
			if ((count>start)&&(done<users[i].ttyheight)) {
			    done++;
			    (void) roomname(users[k].room,stringp);
			    (void) sprintf(stringo,"%s is in %s\n",users[k].name,stringp);
			    rshow(i,stringo);
			}
		    }
		}
	    }
    }
    if ((done==0)&&(parv[1][0]==0)) {
	rshow(i,"You have no friends on at the moment. *schniff* 8-(\n");
	return 0;}
    if (done==0) return -2;
    (void) sprintf(stringo,"Friends %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'qf %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
  FUNCTION - listclear(chan);
 ****************************************************************************/

listclear(i)
int i;
{
    int j;
    for (j=0;(j<INVLENGTH);j++)
	users[i].invitenum[j]=-1,users[j].inviteflags[j]=0;
    rshow(i,"Your list has been cleared.\n");
    return 0;
}
	  
/****************************************************************************
  FUNCTION - listdo(chan);
  COMMAND - <name> <flaglist>
 ****************************************************************************/

listdo(i)
int i;
{
    int n,andmask,eormask,j,d,f;
    if ((n=findfile(parv[1]))<0) return n;
    andmask=-1;
    eormask=0;
    d=strlen(parv[2]);
    if (d>7) {
	rshow(i,"Too many flags!!!!\n");
	return 0;
    }
    f=1;
    for (j=0;j<d;j++) {
	switch(parv[2][j]) {
	case '-':
	    break;
	case '0':
	    andmask^=f;
	    break;
	case '1':
	    andmask^=f;
	    eormask|=f;
	    break;
	case '!':
	    eormask|=f;
	    break;
	default:
	    rshow(i,"Error! Invalid flag!\n");
	    return 0;
	}
	f=f<<1;
    }
    if ((andmask==-1)&&(eormask==0)) return 0;
    for (j=0;(j<INVLENGTH);j++)
	if (users[i].invitenum[j]==n) {
	    users[i].inviteflags[j]&=andmask;
	    users[i].inviteflags[j]^=eormask;
	    if (users[i].inviteflags[j]==0) {
		rshow(i,"Entry removed.\n");
		users[i].invitenum[j]=-1;
		return 0;
	    }
	    rshow(i,"Entry ammended.\n");
	    return 0;
	}
    if (eormask==0) {
	rshow(i,"Nothing changed.\n");
	return 0;
    }
    for (j=0;(j<INVLENGTH);j++)
	if (users[i].invitenum[j]==-1) {
	    users[i].inviteflags[j]=eormask;
	    users[i].invitenum[j]=n;
	    rshow(i,"Entry added.\n");
	    return 0;
	}
    rshow(i,"List too full!!\n");
    return 0;
}

    
