#include "header.h"

/* This is for my stat command, give it a .c file all of its own... */

/****************************************************************************
  FUNCTION - stat(chan)
  COMMAND SYNTAX - [name] <option>
  If no name is given, returns global house statistics.
  If ? is given, returns the stat help page.
  Otherwise checks if that name is valid, if not returns error code.
  Calls statfile() or statuser() dependent on whether the user is logged in.
 ****************************************************************************/

mystat(i)
int i;
{
    int n,r;
    if (parv[1][0]=='?') return stathelp(i);
    if (parv[1][0]==0) return generalstat(i);
    if ((n=findfile(parv[1]))<0) return n;
    if ((r=locateusernum(n))<0) return statfile(i,n);
    return statuser(i,r);
}

/****************************************************************************
  FUNCTION - statfile(chan,user);
  Loads in the the user to channel buffer 255 then calls statuser(chan,255).
  Once that is done, clears the reference number for that user and returns.
 ****************************************************************************/

statfile(i,n)
int i,n;
{
    int j;
    loadtempuser(n);
    j=statuser(i,255);
    users[255].number=-1;
    return j;
}

/****************************************************************************
  FUNCTION - statuser(chan,schan)
  Checks parv[2][0] and if it is....
  Nothing : Calls basicstat(chan,schan);
  f       : Calls flagstat(chan,schan);
  ?       : Calls stathelp(chan);
  Otherwise returns an error message...
 ****************************************************************************/

statuser(i,r)
int i,r;
{
    switch(parv[2][0]) {
    case 0: return basicstat(i,r);
    case 'f': return flagstat(i,r);
    case 'o': return otherstat(i,r);
    case '?': return stathelp(i);
    default:
	rshow(i,"Option not supported (yet?)\n");
	return 0;
    }
}

/****************************************************************************
 ****************************************************************************/

stathelp(i)
int i;
{
    rshow(i,"Stat can be called with the following options....\n\n");
    rshow(i,"      : Just show standard stat.\n");
    rshow(i,"    f : Show flag stat.\n");
    rshow(i,"    o : Show other stat.\n");
    rshow(i,"    ? : List stat help (ie, this page)\n");
    rshow(i,"\n                        Watch this space!!!!!\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

basicstat(i,r)
int i,r;
{
    rshow(i,"  ----------------------------------------------------------------------------\n");
    if (users[r].flags&1) {
	sprintf(stringo,"Status of resident user [%d]:\n%s %s\n",users[r].number,users[r].name,users[r].title);
    } else {
	sprintf(stringo,"Status of user [%d]:\n%s %s\n",users[r].number,users[r].name,users[r].title);
    }
    rshow(i,stringo);
    if (r!=255) {
	if (users[r].ltitle[0]==0) {
	    rshow(i,"No local title set\n");
	} else {
	    sprintf(stringo,"%s %s\n",users[r].name,users[r].ltitle);
	    rshow(i,stringo);
	}
    }
    sprintf(stringo,"Description....\n%s\n\n",users[r].desc);
    rshow(i,stringo);
    sprintf(stringo,"Email Addr:%s\nMy Comment:%s\n\n",users[r].email,users[r].comment);
    rshow(i,stringo);
    if (r!=255) {
	sprintf(stringo,"Number    :%.3d.%.3d.%.3d.%.3d %d\nHost      :%s\nTerminal  :%s\n\n",users[r].ina[0],users[r].ina[1],users[r].ina[2],users[r].ina[3],users[r].ina[4],users[r].host,users[r].tty);
	rshow(i,stringo);
    }
    (void) time((time_t*)&ti);
    ltimestr(ti-users[r].agetime,stringp);
    ltimestr(users[r].age+ti-users[r].agetime,stringq);
    if (r!=255) {
	sprintf(stringo,"Login time (this) :%s\n",stringp);
	rshow(i,stringo);
    }
    sprintf(stringo,"Login time (total):%s\n",stringq);
    rshow(i,stringo);
    ltimestr(ti-users[r].lastlogin,stringp);
    ltimestr(ti-users[r].idletime,stringq);
    if (r!=255) {
	sprintf(stringo,"Login time (log)  :%s\n",stringp);
	rshow(i,stringo);
	sprintf(stringo,"Pressed return    :%s\n",stringq);
	rshow(i,stringo);
    }
    ltimestr(SHOUTTIME-(ti-lastshout[users[r].number]),stringp);
    (void) sprintf(stringo,"Shouts %d/%d. Next shout in %s.\n",shouts[users[r].number],SHOUTMAX,stringp);
    rshow(i,stringo);
    if (shouts[users[r].number]>=SHOUTMAX) 
	rshow(i,"Time listed about is in effect... *** SORE THROAT ***\n");
    if (users[r].leaving)
	if (users[r].leaving<0) {
	    rshow(i,"They're going to be logged out very soon.\n");
	} else {
	    sprintf(stringo,"They're going to be logged out in about %d minutes.\n",users[r].leaving/60+1);
	    rshow(i,stringo);
	}
    rshow(i,"  ----------------------------------------------------------------------------\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

flagstat(i,r)
int i,r;
{
    rshow(i,"  ----------------------------------------------------------------------------\n");
    if (users[r].flags&1) {
	sprintf(stringo,"Flag status of resident user [%d]: %s\n\n",users[r].number,users[r].name);
    } else {
	sprintf(stringo,"Flag status of user [%d]: %s\n\n",users[r].number,users[r].name);
    }
    rshow(i,stringo);
    rshow(i,"Command flags settings:\n");
    (void) strcpy(stringo,"     ------    : ---   ------    : ---   Echoall   : ---   Evict     : ---\n"); 
    doflag(users[r].flags,0x04000000,"YES","NO ",17);
    doflag(users[r].flags,0x02000000,"YES","NO ",35);
    doflag(users[r].flags,0x08000000,"YES","NO ",53);
    doflag(users[r].flags,0x10000000,"YES","NO ",71);
    rshow(i,stringo);
    (void) strcpy(stringo,"     Trace     : ---   ---       : ---   Trusted   : ---   Gagged    : ---\n\n");
    doflag(users[r].flags,0x00800000,"YES","NO ",17);
    doflag(users[r].flags,0x01000000,"YES","NO ",35);
    doflag(users[r].flags,0x00000400,"YES","NO ",53);
    doflag(users[r].flags,0x00000800,"YES","NO ",71);
    rshow(i,stringo);
    rshow(i,"Saved flags settings:\n");
    (void) strcpy(stringo,"     Resident  : ---   Title     : ---   Scape     : ---   List      : ---\n");
    doflag(users[r].flags,0x00000001,"YES","NO ",17);
    doflag(users[r].flags,0x00000002,"YES","NO ",35);
    doflag(users[r].flags,0x00000080,"YES","NO ",53);
    doflag(users[r].flags,0x00000040,"YES","NO ",71);
    rshow(i,stringo);
    (void) strcpy(stringo,"     Prompts   : ---             : ---   Loved     : ---   Cheesey!  : ---\n\n");
    doflag(users[r].flags,0x00000008,"YES","NO ",17);
    doflag(users[r].flags,0x00000010,"YES","NO ",35);
    doflag(users[r].flags,0x20000000,"YES","NO ",53);
    doflag(users[r].flags,0x00000200,"YES","NO ",71);
    rshow(i,stringo);
    rshow(i,"Option flags settings:\n");
    (void) strcpy(stringo,"     Earmuffed : ---   Hiding    : ---   Shielded  : ---   Sees echo : ---\n"); 
    doflag(users[r].flags,0x00001000,"YES","NO ",17);
    doflag(users[r].flags,0x00002000,"YES","NO ",35);
    doflag(users[r].flags,0x00200000,"YES","NO ",53);
    doflag(users[r].flags,0x00400000,"YES","NO ",71);
    rshow(i,stringo);
    (void) strcpy(stringo,"     Informin  : ---   Informout : ---   No Beep   : ---   Converse  : ---\n"); 
    doflag(users[r].flags,0x00004000,"YES","NO ",17);
    doflag(users[r].flags,0x00008000,"YES","NO ",35);
    doflag(users[r].flags,0x00010000,"YES","NO ",53);
    doflag(users[r].flags,0x40000000,"YES","NO ",71);
    rshow(i,stringo);
    (void) strcpy(stringo,"     Mindopen  : ---   Mindlock  : ---   Autoprompt: ---   Linewrap  : ---\n");
    doflag(users[r].flags,0x00080000,"YES","NO ",17);
    doflag(users[r].flags,0x00100000,"YES","NO ",35);
    doflag(users[r].flags,0x00020000,"YES","NO ",53);
    doflag(users[r].flags,0x00040000,"YES","NO ",71);
    rshow(i,stringo);
    (void) strcpy(stringo,"     TextTag   : ---   SayShout  : ---             : ---             : ---\n");
    doflag(users[r].flags,0x00000004,"YES","NO ",17);
    doflag(users[r].flags,0x00000010,"YES","NO ",35);
    doflag(users[r].flags,0x00000020,"YES","NO ",53);
    doflag(users[r].flags,0x00000100,"YES","NO ",71);
    rshow(i,stringo);
    rshow(i,"  ----------------------------------------------------------------------------\n");
    return 0;
}
    
/****************************************************************************
 ****************************************************************************/

generalstat(i)
int i;
{
    rshow(i,"  ----------------------------------------------------------------------------\n");
    countresidents(i);
    rshow(i,(houseflags&1)?"Nameserver is marked as ** DISABLED **\n":
	  "Nameserver is marked as OPERATIONAL.\n");
    rshow(i,(houseflags&2)?"Telnet commands are being logged to screen.\n":
	  "Telnet commands are not being logged.\n");
    rshow(i,(houseflags&4)?"Log events are being displayed to screen.\n":
	  "Log events are not being displayed.\n");
    rshow(i,"  ----------------------------------------------------------------------------\n");
    return 0;
}    

/****************************************************************************
 ****************************************************************************/

otherstat(i,r)
int i;

{
    int j;
    rshow(i,"  ----------------------------------------------------------------------------\n");
    if (users[r].objecting!=-1) {
	sprintf(stringo,"Objecting to :%s.\n   %s\n",users[users[r].objecting].name,users[r].objection);
	rshow(i,stringo);
    }
    for (j=0;j<OBJECTNUM;j++)
	if (users[r].objected[j]!=-1) {
	    sprintf(stringo,"  Objection from:%s\n",cnames[users[r].objected[j]]);
	    rshow(i,stringo);
	}
    sprintf(stringo,"Ignore Message:%s",users[r].ignoremsg);
    rshow(i,stringo);
    rshow(i,"  ----------------------------------------------------------------------------\n");
    return 0;
}
    
