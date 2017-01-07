#include "header.h"

/****************************************************************************
 ****************************************************************************/

who(i)
int i;
{
    int j,count,start,done;
    char ltype[5];
    if (parv[1][0]=='-') return fastwho(i);
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=Firstuser;j!=-1;j=users[j].nextuser)
	if ((users[j].doing<1000)||((users[i].flags&0x200)!=0)) {
	    count++;
	    if ((count>start)&&(done<users[i].ttyheight)) {
		done++;
		switch(users[j].logintype) {
		case 0 : (void) strcpy(ltype,"INet");break;
		case 1 : (void) strcpy(ltype,"LASS");break;
		case 2 : (void) strcpy(ltype,"Wwk.");break;
		    default : (void) strcpy(ltype,"----");
		}
		if (users[j].doing<1000) {
		    (void) sprintf(stringo,"%s - %s %s\n",ltype,users[j].name,users[j].title);
		} else {
		    (void) sprintf(stringo,"%s - ***** %s *****\n",ltype,users[j].name);
		}
		if ((users[j].flags&1)!=0) stringo[5]='+';
		if ((stringo[78]!=0)&&(stringo[78]!='\n')) {
		    stringo[78]='\n';
		    stringo[79]=0;};
		rshow(i,stringo);
	    }
	}
    if (done==0) return -2;
    (void) sprintf(stringo,"Users %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'who %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

fastwho(i)
int i;
{
    int j,k;
    k=0;
    rshow(i,"The following users are logged on at present...\n");
    stringo[0]=0;
    for(j=Firstuser;j!=-1;j=users[j].nextuser)
	if ((users[j].doing<1000)||((users[i].flags&0x200)!=0)) {
	    k++;
	    if (users[j].doing<1000) {
		sprintf(stringp," %s",users[j].name);
	    } else {
		sprintf(stringp,"*%s*",users[j].name);
	    }
	    if ((k&3)!=0) {
		sprintf(stringq,"%s%-18s  ",stringo,stringp);
		strcpy(stringo,stringq);
	    } else {
		sprintf(stringq," %s%-18s\n",stringo,stringp);
		rshow(i,stringq);
		stringo[0]=0;
	    }
	}
    if ((k&3)!=0) {
	sprintf(stringp," %s\n",stringo);
	rshow(i,stringp);
    }
    sprintf(stringo,"If you cant be bothered counting, thats a grand total of %d.\n",k);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

title(i)
int i;
{
    if (parv[1][0]=='?') {
	if (users[i].title[0]==0) {
	    rshow(i,"You have no title.\n");
	    return 0;};
	(void) sprintf(stringo,"You're currently known as:\n%s %s\n",users[i].name,users[i].title);
	rshow(i,stringo);
	return 0;
    }
    (void) strncpy(users[i].title,parr[1],79);
    if (users[i].title[0]==0) {
	rshow(i,"You now have no title.\n");
	return 0;};
    (void) sprintf(stringo,"You're now known as:\n%s %s\n",users[i].name,users[i].title);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

ltitle(i)
int i;
{
    if (parv[1][0]=='?') {
	if (users[i].ltitle[0]==0) {
	    rshow(i,"You have no local title.\n");
	    return 0;};
	(void) sprintf(stringo,"You're currently known locally as:\n%s %s\n",users[i].name,users[i].ltitle);
	rshow(i,stringo);
	return 0;
    }
    (void) strncpy(users[i].ltitle,parr[1],79);
    if (users[i].ltitle[0]==0) {
	rshow(i,"You now have no local title.\n");
	return 0;};
    (void) sprintf(stringo,"You're now known locally as:\n%s %s\n",users[i].name,users[i].ltitle);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

desc(i)
int i;
{
    if (parv[1][0]=='?') {
	if (users[i].desc[0]==0) {
	    rshow(i,"You have no description.\n");
	    return 0;};
	(void) sprintf(stringo,"Your description is:\n%s\n",users[i].desc);
	rshow(i,stringo);
	return 0;
    }
    if ((parv[1][0]=='+')&&(parv[1][1]==0)) {
	int l,ll;
	if (parr[2][0]==0) return 0;
	l=strlen(parr[2]);
	ll=strlen(users[i].desc);
	if ((l+ll) > 238) {
	    rshow(i,"Cant add that, its too long.\n");
	    return 0;
	}
	(void) strcat(users[i].desc,"\n");
	(void) strcat(users[i].desc,parr[2]);
	(void) sprintf(stringo,"Your description now is:\n%s\n",users[i].desc);
	rshow(i,stringo);
	return 0;
    }
    (void) strncpy(users[i].desc,parr[1],239);
    if (users[i].desc[0]==0) {
	rshow(i,"You now have no description.\n");
	return 0;};
    (void) sprintf(stringo,"Your description now is:\n%s\n",users[i].desc);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

prompt(i)
int i;
{
    if (parv[1][0]=='?') {
        (void) sprintf(stringo,"Your prompt is:%s\n",users[i].prompt);
        rshow(i,stringo);
        return 0;
    }
    (void) strncpy(users[i].prompt,parr[1],63);
    (void) sprintf(stringo,"Your prompt now is:%s\n",users[i].prompt);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

cprompt(i)
int i;
{
    if (parv[1][0]=='?') {
        (void) sprintf(stringo,"Your converse prompt is:%s\n",users[i].cprompt);
        rshow(i,stringo);
        return 0;
    }
    (void) strncpy(users[i].cprompt,parr[1],63);
    (void) sprintf(stringo,"Your converse prompt now is:%s\n",users[i].cprompt);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

status(i)
int i;
{
    (void) sprintf(stringo,"\nYou are known as:\n%s %s\n",users[i].name,users[i].title);
    rshow(i,stringo);
    if (users[i].ltitle[0]!=0) {
	(void) sprintf(stringo,"Locally:%s %s\n",users[i].name,users[i].ltitle);
	rshow(i,stringo);
    }
    (void) sprintf(stringo,"You describe yourself as:\n%s\n",users[i].desc);
    rshow(i,stringo);
    (void) sprintf(stringo,"Prompt  :%s\n",users[i].prompt);
    rshow(i,stringo);
    if (users[i].cprompt[0]!=0) {
	(void) sprintf(stringo,"Converse:%s\n",users[i].cprompt);
	rshow(i,stringo);
    }
    (void)sprintf(stringo,"People who you ignore get:\n%s",users[i].ignoremsg);
    rshow(i,stringo);
    if ((users[i].flags&0x40000000)!=0)
	rshow(i,"You are in Converse mode.\n");
    if ((users[i].flags&0x800)!=0)
	rshow(i,"You are GAGGED!\n");
    if ((users[i].flags&0x8000000)!=0)
	rshow(i,"You can echoall.\n");
    if ((users[i].flags&0x10000000)!=0)
	rshow(i,"You can evict.\n");
    if ((users[i].flags&0x400)!=0)
	rshow(i,"You are trusted.\n");
    if ((users[i].flags&0x20000000)!=0)
	rshow(i,"You are loved.\n");
    if ((users[i].flags&0x200)!=0)
	rshow(i,"You can probably do anything.\n");
    rshow(i,"\nSettings.....\n");
    (void) strcpy(stringo,"     Earmuffs  : ---   Hiding    : ---   InformIn  : ---   InformOut : ---\n"); 
    doflag(users[i].flags,0x1000,"ON ","OFF",17);
    doflag(users[i].flags,0x2000,"YES","NO ",35);
    doflag(users[i].flags,0x4000,"YES","NO ",53);
    doflag(users[i].flags,0x8000,"YES","NO ",71);
    rshow(i,stringo);
    (void) strcpy(stringo,"     NoBeep    : ---   AutoPrompt: ---   PadEcho   : ---   Linewrap  : ---\n");
    doflag(users[i].flags,0x10000,"ON  ","OFF",17);
    doflag(users[i].flags,0x20000,"ON ","OFF",35);
    doflag(users[i].localecho,1,"ON ","OFF",53);
    doflag(users[i].flags,0x40000,"ON ","OFF",71);
    rshow(i,stringo);
    (void) strcpy(stringo,"     MindOpen  : ---   MindLock  : ---   Shield    : ---   SeeEcho   : ---\n");
    doflag(users[i].flags,0x80000,"YES","NO ",17);
    doflag(users[i].flags,0x100000,"YES","NO ",35);
    doflag(users[i].flags,0x200000,"UP  ","DOWN",53);
    doflag(users[i].flags,0x400000,"ON ","OFF",71);
    rshow(i,stringo);
    (void) strcpy(stringo,"     TextTag   : ---   SayShout  : ---             : ---             : ---\n");
    doflag(users[i].flags,0x0000004,"ON ","OFF",17);
    doflag(users[i].flags,0x0000010,"ON ","OFF",35);
/*    doflag(users[i].flags,0x200000,"UP  ","DOWN",53);
    doflag(users[i].flags,0x400000,"ON ","OFF",71);*/
    rshow(i,stringo);
    if (users[i].leaving)
	if (users[i].leaving<0) {
	    rshow(i,"You're going to be logged out very soon.\n");
	} else {
	    sprintf(stringo,"You're going to be logged out in about %d minutes.\n",users[i].leaving/60+1);
	    rshow(i,stringo);
	}
    if ((users[i].flags&1)==0) return 0;
    rshow(i,"\n         ** Type saved to see what if anything is saved on logout **\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

saved(i)
int i;
{
    rshow(i,"When you log out, the following will happen\n");
    if (users[i].flags&1) {
	rshow(i,"Your password and option flags are saved...\n");
	rshow(i,(users[i].flags&2)?"Your title & descn. are saved to disk.\n":"Your title & descn. will be reset.\n");
	rshow(i,(users[i].flags&8)?"Your prompt(s) are saved to disk.\n":"Your prompt(s)  will be reset.\n");
	rshow(i,(users[i].flags&64)?"Your list is saved to disk.\n":"Your list will be cleared.\n");
	rshow(i,(users[i].flags&128)?"Your mindscape is saved to disk.\n":"Your mindscape will be reset.\n");
    } else {
	rshow(i,"Your data will be lost, you're not a resident.\n");
    }
    return 0;
}


/****************************************************************************
 ****************************************************************************/

log(i)
int i;
{
    int j,k,li,lo;
    if (parv[1][0]==0) return -5;
    if ((j=findfile(parv[1]))<0) return j;
    if ((k=locateusernum(j))<0) {
	li=filelog(j,&li,&lo);
	if (li<1) {
	    (void) sprintf(stringo,"%s hasnt logged in before.\n",cnames[j]);
	} else {
	    timestr(li,stringp);
	    timestr(lo,stringq);
	    ltimestr(lo-li,stringr);
	    (void) sprintf(stringo,"User: %s\nLast logged in:  %s\nLast logged out: %s\nLogged in for a total of %s\n",cnames[j],stringp,stringq,stringr);
	}
	rshow(i,stringo);
	return 0;
    } else {
	sprintf(stringo,"%s is logged in RIGHT NOW!!!!\n",cnames[j]);
	rshow(i,stringo);
	timestr(users[k].lastlogin,stringp);
	sprintf(stringo,"And has been since %s.\n",stringp);
	rshow(i,stringo);
	return 0;
    }
}

/****************************************************************************
 ****************************************************************************/

mytime(i)
int i;
{
    (void) time((time_t*)&ti);
    timestr(ti,stringp);
    (void) sprintf(stringo,"The time & date in the UK is now %s.\n",stringp);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

pass(i)
int i;
{
    users[i].olddoing=users[i].doing;
    if (users[i].passwd[0]==0) {
	users[i].doing=5;
	return 0;
    }
    users[i].doing=4;
    return 0;
}

/****************************************************************************
 ****************************************************************************/

idle(i)
int i;
{
    int j;
    if ((parv[1][0]==0)||(atoi(parv[1])!=0))
	return midle(i);
    if ((j=scanuser(parv[1]))<0) return j;
    (void) time((time_t*)&ti);
    ltimestr(ti-users[j].idletime,stringp);
    (void) sprintf(stringo,"%s last pressed return %s ago.\n",users[j].name,stringp);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

midle(i)
int i;
{
    int j,count,start,done,k;
    (void) time((time_t*)&ti);
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=Firstuser;j!=-1;j=users[j].nextuser)
	if (users[j].doing<1000) {
	    count++;
	    if ((count>start)&&(done<users[i].ttyheight)) {
		done++;
		strcpy(capstr,users[j].name);
		k=ti-users[j].idletime;
		(void) imesg(k);
		rshow(i,stringo);
	    }
	}
    if (done==0) return -2;
    (void) sprintf(stringo,"Users %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'idle %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

imesg(k)
int k;
{
    if (k==0) {(void) sprintf(stringo,"%-16s : Still touching return?!\n",capstr);return 0;};
    if (k<10) {(void) sprintf(stringo,"%-16s : Hands still above the keyboard?\n",capstr);return 0;};
    if (k<20) {(void) sprintf(stringo,"%-16s : Interacting nicely.\n",capstr);return 0;};
    if (k<40) {(void) sprintf(stringo,"%-16s : Probably typing.\n",capstr);return 0;};
    if (k<60) {(void) sprintf(stringo,"%-16s : Typing slowly?\n",capstr);return 0;};
    if (k<90) {(void) sprintf(stringo,"%-16s : Thinking?\n",capstr);return 0;};
    if (k<160) {(void) sprintf(stringo,"%-16s : Daydreaming?\n",capstr);return 0;};
    if (k<240) {(void) sprintf(stringo,"%-16s : At the snack machine??!?\n",capstr);return 0;};
    if (k<300) {(void) sprintf(stringo,"%-16s : Nodding off?\n",capstr);return 0;};
    if (k<600) {(void) sprintf(stringo,"%-16s : Falling asleep perchance?\n",capstr);return 0;};
    if (k<1200) {(void) sprintf(stringo,"%-16s : Eyes starting to close I wonder?\n",capstr);return 0;};
    if (k<1800) {(void) sprintf(stringo,"%-16s : Asleep?\n",capstr);return 0;};
    if (k<2400) {(void) sprintf(stringo,"%-16s : Dreaming merrily away? \n",capstr);return 0;};
    if (k<3000) {(void) sprintf(stringo,"%-16s : Comatose?\n",capstr);return 0;};
    if (k<3600) {(void) sprintf(stringo,"%-16s : Dead?\n",capstr);return 0;};
    if (k<4200) {(void) sprintf(stringo,"%-16s : Is rigor setting in?\n",capstr);return 0;};
    if (k<4800) {(void) sprintf(stringo,"%-16s : Going green?\n",capstr);return 0;};
    if (k<5400) {(void) sprintf(stringo,"%-16s : Starting to rot?\n",capstr);return 0;};
    if (k<6000) {(void) sprintf(stringo,"%-16s : Decomposing nicely?\n",capstr);return 0;};
    if (k<6600) {(void) sprintf(stringo,"%-16s : Nothing left but a skeleton?\n",capstr);return 0;};
    if (k<7200) {(void) sprintf(stringo,"%-16s : Reduced to a pile of dust?\n",capstr);return 0;};
    (void) sprintf(stringo,"%-16s : Rotted completely away.\n",capstr);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

hosts(i)
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
	    (void) sprintf(stringo," %-16s:%3d: %s\n",users[j].name,j,users[j].host);
	    rshow(i,stringo);
	}
    }
    if (done==0) return -2;
    (void) sprintf(stringo,"Users %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'hosts %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

nums(i)
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
	    (void) sprintf(stringo," %-16s:%3d: %.3d.%.3d.%.3d.%.3d %d\n",users[j].name,j,users[j].ina[0],users[j].ina[1],users[j].ina[2],users[j].ina[3],users[j].ina[4]);
	    rshow(i,stringo);
	}
    }
    if (done==0) return -2;
    (void) sprintf(stringo,"Users %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'nums %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

where(i)
int i;
{
    int j,count,start,done;
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=Firstuser;j!=-1;j=users[j].nextuser)
	if (users[j].doing<1000) {
	    if (((users[j].flags&0x2000)==0)||((users[i].flags&0x200)!=0)||(i==j)&&(users[j].room!=0)) {
		count++;
		if ((count>start)&&(done<users[i].ttyheight)) {
		    done++;
		    (void) roomname(users[j].room,stringp);
		    (void) sprintf(stringo,"%s is in %s\n",users[j].name,stringp);
		    rshow(i,stringo);
		}
	    }
	}
    if (done==0) return -2;
    (void) sprintf(stringo,"Users %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more users on 'where %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

find(i)
int i;
{
    int j;
    if ((j=scanuser(parv[1]))<0) return j;
    (void) roomname(users[j].room,stringp);
    if (i==j) {
	(void) sprintf(stringo,"You are in %s\n",stringp);
    } else {
	if (users[i].room==users[j].room) {
	    (void) sprintf(stringo,"%s is here in the same room as you!!\n",users[j].name);
	} else {
	    if ((users[j].flags&0x2000)==0) {
		(void) sprintf(stringo,"%s is in %s\n",users[j].name,stringp);
	    } else {
		if ((users[i].flags&0x200)==0) {
		    (void) sprintf(stringo,"You cannot find %s.\n",users[j].name);
		} else {
		    (void) sprintf(stringo,"%s is hiding in %s.\n",users[j].name,stringp);
		}
	    }
	}
    }
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

examine(i)
int i;
{
    int j;
    if ((j=scanuser(parv[1]))<0) return j;
    if (users[j].room!=users[i].room) {
	(void) sprintf(stringo,"You cannot see %s.\n",users[j].name);
	rshow(i,stringo);
	return 0;
    } else {
	if (users[j].ltitle[0]!=0) {
	    (void) sprintf(stringo,"%s %s\n%s\n",users[j].name,users[j].ltitle,users[j].desc);
	} else {
	    (void) sprintf(stringo,"%s %s\n%s\n",users[j].name,users[j].title,users[j].desc);
	}
    } 
    rshow(i,stringo);
    if ((users[j].flags&0x800)!=0) {
	(void) sprintf(stringo,"%s is gagged and cannot speak.\n",users[j].name);
	rshow(i,stringo);
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

mindstate(i)
int i;
{
    (void) sprintf(stringo,"Your mindscape is %s\n",users[i].scapename);
    rshow(i,stringo);
    if ((users[i].flags&0x80)!=0) rshow(i,"It is saved when you log out.\n");
    if ((users[i].flags&0x100000)!=0) {
	rshow(i,"Its locked from being visited.\n");
    } else {
	if ((users[i].flags&0x80000)!=0) rshow(i,"Its open for anyone to visit (if not barred).\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

initedit(user,what,name,doing)
int user,doing;
char *what,*name;
{
    int n;
    for (n=0;n<MAXEDITS;n++) if (edits[n].user==-1) {
	edits[n].user=user;
	edits[n].editing=what;
	(void) strcpy(edits[n].buffer,what);
	edits[n].editname=name;
	(void) strcpy(edits[n].name,name);
	users[user].ednum=n;
	users[user].olddoing=users[user].doing;
	users[user].doing=doing;
	return n;
    }
    return -1;
}

/****************************************************************************
 ****************************************************************************/

edit(i)
int i;
{
    if (initedit(i,users[i].scapedesc,users[i].scapename,200)==-1) {
	rshow(i,"There are too many people editing now.. try again later.\n");
	return 0;
    }
    rshow(i,"Entering mindscape edit mode...\n");
    rshow(i,"Type /help for a list of edit commands...\n");
    rshow(i,"Type /abandon to exit without saving changes.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

roomedit(i)
int i;
{
    if ((users[i].room<1)||(users[i].room>=MAXROOMS)) {
	rshow(i,"You cant edit THIS room.\n");
	return 0;
    }
    if (initedit(i,Roomdesc[users[i].room],Roomname[users[i].room],201)==-1) {
	rshow(i,"There are too many people editing now.. try again later.\n");
	return 0;
    }
    rshow(i,"Entering room edit mode...\n");
    rshow(i,"Type /help for a list of edit commands...\n");
    rshow(i,"Type /abandon to exit without saving changes.\n");
    rshow(i,"Type /use to exit and save changes.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

mailedit(i)
int i;
{
    int j,jj;
    if (overmaillimit(users[i].number)) {
	rshow(i,"You have too many items of mail in the boxes...\n");
	return 0;};
    if ((j=findfile(parv[1]))<0) return j;
    jj=202;
    if (strcmp(parv[2],"anon")==0) jj=203;
    if (initedit(i,"",parv[1],jj)==-1) {
	rshow(i,"There are too many people editing now.. try again later.\n");
	return 0;
    }
    if (users[i].olddoing>999) users[i].doing+=1000;
    if (jj==202) {
	rshow(i,"Entering mail edit mode...\n");
	rshow(i,"Type /help for a list of edit commands...\n");
	rshow(i,"Type /abandon to exit without sending mail.\n");
	rshow(i,"Type /send to exit and send mail.\n");
	return 0;
    } else {
	rshow(i,"Entering anonymous mail edit mode...\n");
	rshow(i,"Type /help for a list of edit commands...\n");
	rshow(i,"Type /abandon to exit without sending mail.\n");
	rshow(i,"Type /send to exit and send mail.\n");
	return 0;
    }
}

/****************************************************************************
 ****************************************************************************/

replyedit(i,j,ff)
int i,j,ff;
{
    int jj;
    if (overmaillimit(users[i].number)) {
	rshow(i,"You have too many items of mail in the boxes...\n");
	return 0;};
    jj=202;
    if (strcmp(parv[2],"anon")==0) jj=203;
    if (initedit(i,"",unames[j],jj)==-1) {
	rshow(i,"There are too many people editing now.. try again later.\n");
	return 0;
    }
    if (users[i].olddoing>999) users[i].doing+=1000;
    if (jj==202) {
	rshow(i,"Entering mail reply edit mode...\n");
	rshow(i,"Type /help for a list of edit commands...\n");
	rshow(i,"Type /abandon to exit without sending mail.\n");
	rshow(i,"Type /send to exit and send mail.\n");
	return 0;
    } else {
	rshow(i,"Entering anonymous mail reply edit mode...\n");
	rshow(i,"Type /help for a list of edit commands...\n");
	rshow(i,"Type /abandon to exit without sending mail.\n");
	rshow(i,"Type /send to exit and send mail.\n");
	return 0;
    }
}

/****************************************************************************
 ****************************************************************************/

board(i)
int i;
{
    int j,count,start,done;
    start=atoi(parv[1]);
    if (start<1) start=1;
    count=0;done=0;
    start=(start-1)*(users[i].ttyheight-3);
    for(j=0;(j<MAXNOTICES)&&(noticenum[j]!=-1);j++) {
	count++;
	if ((count>start)&&(done<users[i].ttyheight)) {
	    done++;
	    (void) sprintf(stringo,"%-15s:%s\n",cnames[noticenum[j]],noticemsg[j]);
	    rshow(i,stringo);
	}
    }
    if (done==0) return -2;
    (void) sprintf(stringo,"Notices %d-%d from a total of %d.\n",start+1,start+done,count);
    rshow(i,stringo);
    if (count>(start+done)) {
	(void) sprintf(stringo,"There will be more notices on 'board %d'\n",start/(users[i].ttyheight-3)+2);
	rshow(i,stringo);}
    return 0; 
};

/****************************************************************************
 ****************************************************************************/

notice(i)
int i;
{
    int j,k;
    if ((users[i].flags&1)==0) {
	rshow(i,"You are not resident so unfortunately cannot write to the notice board.\n");
	return 0;
    }
    if (parv[1][0]==0) {
	for(j=0;(j<MAXNOTICES)&&(noticenum[j]!=-1);j++) {
	    if (noticenum[j]==users[i].number) {
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
	return 0;
    } else {
	for(j=0;(j<MAXNOTICES)&&(noticenum[j]!=-1)&&(noticenum[j]!=users[i].number);j++);
	if (j==MAXNOTICES) j=MAXNOTICES-1;
	for (k=j;k>0;k--) {
	    noticenum[k]=noticenum[k-1];
	    strcpy(noticemsg[k],noticemsg[k-1]);
	}
	noticenum[0]=users[i].number;
	strncpy(noticemsg[0],parr[1],63);
	sprintf(stringo,"You set your notice to:%s\n",noticemsg[0]);
	rshow(i,stringo);
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

ignoremsg(i)
int i;
{
    strncpy(users[i].ignoremsg,parr[1],78);
    if (parr[1][0]!=0) strcat(users[i].ignoremsg,"\n");
    sprintf(stringo,"You set your ignore message to:\n%s",users[i].ignoremsg);
    rshow(i,stringo);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

recap(i)
int i;
{
    if (parv[1][0]==0) {
	rshow(i,"You need to type 'recap name' as you want it capitalised. eg: recap FloP\n");
	return 0;
    }
    parr[1][strlen(parv[1])]=0;
    if (strcasecmp(parv[1],unames[users[i].number])!=0) {
	rshow(i,"That doesnt match your name..\n");
	return 0;
    }
    strcpy(users[i].name,parr[1]);
    rshow(i,"Ok, done.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

with(i)
int i;
{
    int j,r,k,l;
    if (parv[1][0]==0) return 0;
    if ((j=scanuser(parv[1]))<0) return j;
    r=users[j].room;
    if (((users[j].flags&0x2000)!=0)&&((users[i].flags&512)==0)&&(users[i].room!=r)) {
	rshow(i,"They are hiding!!!\n");
	return 0;
    }
    if (r<1000000) {
	k=Roomuser[r];
    } else {
	k=users[r-1000000].scapeuser;
    }
    l=0;
    sprintf(stringo,"%s is with....\n",users[j].name);
    rshow(i,stringo);
    if (r==users[i].room) {
	for (;k!=-1;k=users[k].nextroom) {
	    if (j!=k) {
		if (i==k) {
		    sprintf(stringo,"Yourself, %s %s\n",users[i].name,users[i].title);
		} else {
		    sprintf(stringo,"%s %s\n",users[k].name,users[k].title);
		}
		l++;
		rshow(i,stringo);
	    }
	}
    } else {
	for (;k!=-1;k=users[k].nextroom) {
	    if (j!=k) {
		if (((users[k].flags&0x2000)==0)||((users[i].flags&512)!=0)) {
		    if ((users[k].flags&0x2000)==0) {
			sprintf(stringo,"%s %s\n",users[k].name,users[k].title);
		    } else {
			sprintf(stringo,"%s %s (Hiding)\n",users[k].name,users[k].title);
		    }
		    rshow(i,stringo);
		    l++;
		}
	    }
	}
    }
    if (l==0) rshow(i,".... Nobody (awww)\n");
    return 0;
}
	
/****************************************************************************
 ****************************************************************************/

leavein(i)
int i;
{
    int j,k;
    j=atoi(parv[1]);
    if ((parv[2][0]!=0)&&(users[i].flags&0x200)) {
	if ((k=scanuser(parv[2]))<0) return k;
    } else {
	k=i;
    }
    if (j<=0) {
	rshow(i,"You need to specify a time of 1 or more minutes.\n");
	return 0;
    }
    j=j*60;
    if (users[k].leaving) {
	if (users[k].leaving<j) {
	    rshow(i,"Once the timer has started it cannot be slowed. 8-)\n");
	    return 0;
	}
    }
    users[k].leaving=j;
    sprintf(stringo,"Timer set to %d mins.\n",j/60);
    rshow(i,stringo);
}

/****************************************************************************
 ****************************************************************************/

ttyheight(i)
int i;
{
    int j,k;
    if (parv[1][0]==0) {
	sprintf(stringo,"Your ttyheight is set to %d.\n",users[i].ttyheight+6);
	rshow(i,stringo);
	return 0;
    }
    j=atoi(parv[1]);
    if (j<8) {
	rshow(i,"ttyheight needs a height of at least 8.\n");
	return 0;
    }
    if (j>8000) {
	rshow(i,"ttyheight needs a height of at most 8000.\n");
	return 0;
    }
    users[i].ttyheight=j-6;
    sprintf(stringo,"Your ttyheight is set to %d.\n",users[i].ttyheight+6);
    rshow(i,stringo);
}

/****************************************************************************
 ****************************************************************************/

ttywidth(i)
int i;
{
    int j,k;
    if (parv[1][0]==0) {
	sprintf(stringo,"Your ttywidth is set to %d.\n",users[i].ttywidth);
	rshow(i,stringo);
	return 0;
    }
    j=atoi(parv[1]);
    if (j<10) {
	rshow(i,"ttywidth needs a width of at least 10.\n");
	return 0;
    }
    if (j>300) {
	rshow(i,"ttywidth needs a height of at most 300.\n");
	return 0;
    }
    users[i].ttywidth=j;
    sprintf(stringo,"Your ttywidth is set to %d.\n",users[i].ttywidth);
    rshow(i,stringo);
}

