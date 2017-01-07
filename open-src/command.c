#include "header.h"
FILE     *fopen();

/****************************************************************************
 ****************************************************************************/

getusername(i)
int i;
{
    int j;
    char c;
    if (strcmp(parv[0],"connect")==0) {
	/* Strange tiny connection..... */
	rshow(i,"Using tiny connection!\n");
	for (j=0;j<15;j++) {
	    c=parv[1][j];
	    if ((c<'a' || c>'z') && (c<'A' || c>'Z') && (c!=0))
		parv[1][j]=0;
	}
	parv[1][15]=0;
	if (parv[1][0]==0) {
	    rshow(i,"Oh well, Another time maybe?\n");
	    KillConnect(i);
	    return;};
	if (banished(parv[1])) {
	    rshow(i,"That name has been banished.\n");
	    KillConnect(i);
	    return;};
	if (finduser(parv[1])>=0) {
	    rshow(i,"That person is already here!\n");
	    rshow(i,"If you're stuck on, log in without connect to clear call.\n");
	    rshow(i,"I dont like tinyfuge much you see... *sigh*\n\n");
	    KillConnect(i);
	    return;};
	users[i].number=findfile(parv[1]);
	if (users[i].number<0) {
	    users[i].number=-1;
	    rshow(i,"That name is not a resident here.\n");
	    KillConnect(i);
	    return;
	} else {
	    users[i].name=cnames[users[i].number];
	}
	loaduser(i,0);
	if (strcmp(users[i].passwd,crypt(parr[2],"CP"))==0) {
	    rshow(i,"Login Correct, Welcome to the house.\n");
	    doentry(i);
	    return;
	}
	rshow(i,"I KNOW that isnt the right password.\n");
	KillConnect(i);
	return;
    } else {
	for (j=0;j<15;j++) {
	    c=parv[0][j];
	    if ((c<'a' || c>'z') && (c<'A' || c>'Z') && (c!=0))
		parv[0][j]=0,parr[0][j]=0;
	}
	parv[0][15]=0;
	parr[0][strlen(parv[0])]=0;
	if (parv[0][0]==0) {
	    return;};
	if (banished(parv[0])) {
	    rshow(i,"That name has been banished.\n");
	    KillConnect(i);
	    return;};
	if ((j=finduser(parv[0]))>=0) {
	    (void) strcpy(users[i].desc,parv[0]);
	    users[i].olddoing=j;
	    users[i].oldpasswd=users[j].oldpasswd;
	    strcpy(users[i].passwd,users[j].passwd);
	    users[i].doing=8;
	    return;
	}
	/* Sod barring for now.... */
	users[i].number=findfile(parv[0]);
	if (users[i].number>=0) {
	    users[i].name=cnames[users[i].number];
	    (void) sprintf(stringo,"Welcome %s.\n",users[i].name);
	    rshow(i,stringo);
	    loaduser(i,0);
	} else  {
	    long uini;
	    int k;
	    char *uin;
	    uin=(char*)&uini;
	    uin[0]=(char) users[i].ina[0];
	    uin[1]=(char) users[i].ina[1];
	    uin[2]=(char) users[i].ina[2];
	    uin[3]=(char) users[i].ina[3];
	    for (k=0;
		 (hostlist[k].s_addr!=0)&&
		     (hostlist[k].s_addr!=uini)&&
		     (k<MAXHOSTS);
		 k++);
	    if (k!=MAXHOSTS)
		if ((hostlist[k].s_addr==uini)&&(hoststatus[k]&1)) {
		    rshow(i,"That site is closed to non-residents.\n");
		    KillConnect(i);
		    return;
		}
	    if ((housestatus&1)&&(users[i].logintype==1)) {
		rshow(i,"I'm afraid the house is closed to non-residents at the moment..\n");
		KillConnect(i);
		return;
	    }
	    if ((housestatus&2)&&(users[i].logintype==2)) {
		rshow(i,"I'm afraid the house is closed to non-residents at the moment..\n");
		KillConnect(i);
		return;
	    }
	    if ((housestatus&4)&&(users[i].logintype==0)) {
		rshow(i,"I'm afraid the house is closed to non-residents at the moment..\n");
		KillConnect(i);
		return;
	    }
	    (void) strcpy(users[i].desc,parr[0]);
	    if (strcmp(parv[0],parr[0])==0) {
		users[i].desc[0]|=32;
		users[i].desc[0]^=32;
		sprintf(stringo,"Welcome %s\nIf you want your name all in lowercase, use the 'recap' command once inside.\n",users[i].desc);
	    } else {
		sprintf(stringo,"Welcome %s\n",parr[0]);
	    }
	    rshow(i,stringo);
	    rshow(i,"You've not got a resident character, Do you want to log in anyway?\n");
	    users[i].doing=3;
	    return;};
    }
    if (users[i].passwd[0]==0) {
	if (users[i].oldpasswd==0) {
	    rshow(i,"You seem to have no password, please SET ONE!\n");
	    doentry(i);
	    return;};
	users[i].doing=7;
	return;
    }
    users[i].doing=2;
    return;
}

/****************************************************************************
 ****************************************************************************/

getpassword(i)
int i;
{
    if (users[i].passwd[0]==0) {
	rshow(i,"You seem to have no password, please SET ONE!\n");
	doentry(i);
	return;};
    if (strcmp(users[i].passwd,crypt(parr[0],"CP"))==0) {
	rshow(i,"Password Correct, Welcome to the house.\n");
	doentry(i);
	users[i].tempflags&=0xfffffffe;
	return;
    }
    rshow(i,"Dunno what you think, But I KNOW that isnt the right password.\n");
    if (users[i].tempflags&1) {
	KillConnect(i);
	return;
    }
    rshow(i,"You have one more chance to get it right...\n");
    users[i].tempflags|=1;
    return;
}

/****************************************************************************
 ****************************************************************************/

ejectmyself(i)
int i;
{
    if (users[i].passwd[0]==0) {
	if (users[i].oldpasswd==0) {
	    rshow(i,"That user has no password so you cannot do this.\n");
	    KillConnect(i);
	    return;
	} else {
	    if (sillycrypt(parr[0])!=users[i].oldpasswd) {
		rshow(i,"Thats not the correct password.\n");
		KillConnect(i);
		return;};
	    rshow(i,"Password Correct, Clearing both calls.\n");
	    if (strcasecmp(users[i].desc,unames[users[users[i].olddoing].number])==0)
		LostConnect(users[i].olddoing);
	    KillConnect(i);
	    return;
	}
    };
    if (strcmp(users[i].passwd,crypt(parr[0],"CP"))==0) {
	rshow(i,"Password Correct, Clearing both calls.\n");
	if (strcasecmp(users[i].desc,unames[users[users[i].olddoing].number])==0)
	    LostConnect(users[i].olddoing);
	KillConnect(i);
	return;
    }
    rshow(i,"Dunno what you think, But I KNOW that isnt the right password.\n");
    KillConnect(i);
    return;
}

/****************************************************************************
 ****************************************************************************/

wantoldpass(i)
int i;

{
    char opass[16];
    (void) strcpy(opass,crypt(parr[0],"CP"));
    if (strcmp(opass,users[i].passwd)!=0) {
	rshow(i,"INCORRECT PASSWORD! Abandoning change attempt.\n");
	users[i].doing=users[i].olddoing;
	return;
    }
    users[i].doing=5;
    return;
}

/****************************************************************************
 ****************************************************************************/

wantnewpass(i)
int i;
{
    if (parv[0][0]==0) {
	rshow(i,"Abadoning attempt.\n");
	users[i].doing=users[i].olddoing;
	return;
    }
    (void) strcpy(users[i].npasswd,crypt(parr[0],"CP"));
    users[i].doing=6;
    return;
}

/****************************************************************************
 ****************************************************************************/

wantnewpassa(i)
int i;
{
    char opass[16];
    if (parv[0][0]==0) {
	rshow(i,"Abadoning attempt.\n");
	users[i].doing=users[i].olddoing;
	return;
    }
    (void) strcpy(opass,crypt(parr[0],"CP"));
    if (strcmp(opass,users[i].npasswd)!=0) {
	rshow(i,"PASSWORDS DO NOT MATCH! Abandoning attempt.\n");
	users[i].doing=users[i].olddoing;
	return;
    }
    (void) strcpy(users[i].passwd,opass);
    rshow(i,"Password set... \n");
    users[i].doing=users[i].olddoing;
    return;
}

/****************************************************************************
 ****************************************************************************/

wantresident(i)
int i;
{
    int j;
    if (parv[0][0]=='n') {
	rshow(i,"Ok... byeee!\n");
	LostConnect(i);
	return;}
    if (parv[0][0]!='y') return;
    for (j=Firstuser;j!=-1;j=users[j].nextuser)
	if (j!=i)
	    if (strcasecmp(users[i].desc,unames[users[j].number])==0) {
		rshow(i,"That user is *ALREADY* logged in!\n");
		KillConnect(i);
		return;
	    }
    for (j=0;(j<MAXUSERS)&&(unames[j][0]!=0);j++);
    if (j==MAXUSERS) {
	rshow(i,"Sorry, Theres no space for any new users... 8-(\n");
	LostConnect(i);
	return;};
    rshow(i,"Ok then, creating user...\n");
    (void) strcpy(cnames[j],users[i].desc);
    {
	int k;
	for (k=0;cnames[j][k];k++) unames[j][k]=cnames[j][k]|32;
	unames[j][k]=0;
    }
    renhash(j);
    users[i].number=j;
    users[i].name=cnames[j];
    users[i].desc[0]=0;
    users[i].flags==0;
    rshow(i,"Okey dokey, You're in... 8-)\n");
    (void) strcpy(users[i].title,"the newly created.");
    (void) sprintf(users[i].scapename,"%s's mindscape.",users[i].name);
    (void) sprintf(users[i].scapedesc,"You are in the mindscape of %s.",users[i].name);
    (void) strcpy(users[i].prompt,"-=> ");
    shouts[users[i].number]=SHOUTMAX;
    (void) time((time_t *)&lastshout[users[i].number]);
    doentry(i);
    return;
}

/****************************************************************************
 ****************************************************************************/

wanthousepass(i)
int i;
{
    if (sillycrypt(parr[0])!=users[i].oldpasswd) {
	rshow(i,"Thats not the correct password.\n");
	users[i].number=-1;
	users[i].passwd[0]=0;
	KillConnect(i);
	return;};
    rshow(i,"Please set a new password soon (or redo the old one) using the 'pass' command.\nThank you.\n");
    doentry(i);
    return;
}

/****************************************************************************
 ****************************************************************************/

normalcommand(i)
int i;
{
    normalerr(i,parsecommand(i));
    return;
}

/****************************************************************************
  FUNCTION - normalerr(chan,error)
  Shows the appropriate error message for that code to channel chan.
 ****************************************************************************/

normalerr(i,e)
int i,e;
{
    switch(e) {
    case 0: return;
    case -1: rshow(i,"Pardon?\n");return;
    case -2: rshow(i,"Theres not enough for that page.\n");return;
    case -3: rshow(i,"That help page has not been written *yet?*\n");return;
    case -4: rshow(i,"What sort of toggle is that?\n");return;
    case -5: rshow(i,"A name would be kinda useful!\n");return;
    case -6: rshow(i,"Non-unique match.. Be more specific?\n");return;
    case -7: rshow(i,"Who?\n");return;
    case -8: rshow(i,"No such room!!\n");return;
    case -9: rshow(i,"Where?\n");return;
    }
    return;
}

/****************************************************************************
 ****************************************************************************/

parsecommand(i)
int i;
{
    users[i].cposn=0;
    if (parv[0][0]==0) return 0;
    /* Rightyho, deal with commands.... */
    if (strcmp(parv[0],"quit")==0) {LostConnect(i);return 0;};
    if (strcmp(parv[0],"who")==0) {return(who(i));};
    if (strcmp(parv[0],"where")==0) {return(where(i));};
    if (strcmp(parv[0],"find")==0) {return(find(i));};
    if (strcmp(parv[0],"with")==0) {return(with(i));};
    if (strcmp(parv[0],"log")==0) {return(log(i));};
    if (strcmp(parv[0],"l")==0) {look(i,users[i].room);return 0;};
    if (strcmp(parv[0],"look")==0) {look(i,users[i].room);return 0;};
    if (strcmp(parv[0],"mindseye")==0) {look(i,i+1000000);return 0;};
    if (strcmp(parv[0],"say")==0) {return(say(i));};
    if (strcmp(parv[0],"eject")==0) {return(eject(i));};
    if (strcmp(parv[0],"emote")==0) {return(emote(i));};
    if (strcmp(parv[0],"help")==0) {return(help(i));};
    if (strcmp(parv[0],"recap")==0) {return(recap(i));};
    if (strcmp(parv[0],"title")==0) {return(title(i));};
    if (strcmp(parv[0],"ltitle")==0) {return(ltitle(i));};
    if (strcmp(parv[0],"describe")==0) {return(desc(i));};
    if (strcmp(parv[0],"prompt")==0) {return(prompt(i));};
    if (strcmp(parv[0],"cprompt")==0) {return(cprompt(i));};
    if (strcmp(parv[0],"tell")==0) {return(mytell(i));};
    if (strcmp(parv[0],"te")==0) {return(mytell(i));};
    if (strcmp(parv[0],"echo")==0) {return(echo(i));};
    if (strcmp(parv[0],"remote")==0) {return(remote(i));};
    if (strcmp(parv[0],"shout")==0) {return(shout(i));};
    if (strcmp(parv[0],"mindscape")==0) {return(mindscape(i));};
    if (strcmp(parv[0],"mind")==0) {return(mindscape(i));};
    if (strcmp(parv[0],"mindstate")==0) {return(mindstate(i));};
    if (strcmp(parv[0],"leave")==0) {return(leave(i));};
    if (strcmp(parv[0],"time")==0) {return(mytime(i));};
    if (strcmp(parv[0],"visit")==0) {return(visit(i));};
    if (strcmp(parv[0],"accept")==0) {return(visit(i));};
    if (strcmp(parv[0],"status")==0) {return(status(i));};
    if ((strcmp(parv[0],"iamcp")==0)&&(strcmp(unames[users[i].number],"cheeseplant")==0)) {users[i].flags|=0x3ff;return 0;};
    if (strcmp(parv[0],"padecho")==0) {return(padecho(i));};
    if (strcmp(parv[0],"earmuffs")==0) {return(earmuffs(i));};
    if (strcmp(parv[0],"hide")==0) {return(hide(i));};
    if (strcmp(parv[0],"idle")==0) {return(idle(i));};
    if (strcmp(parv[0],"exits")==0) {return(showexits(i));};
    if (strcmp(parv[0],"pass")==0) {return(pass(i));};
    if (strcmp(parv[0],"examine")==0) {return(examine(i));};
    if (strcmp(parv[0],"exam")==0) {return(examine(i));};
    if (strcmp(parv[0],"exa")==0) {return(examine(i));};
    if (strcmp(parv[0],"go")==0) {return(enter(i));};
    if (strcmp(parv[0],"edit")==0) {return(edit(i));};
    if (strcmp(parv[0],"informin")==0) {return(informin(i));};
    if (strcmp(parv[0],"informout")==0) {return(informout(i));};
    if (strcmp(parv[0],"nobeep")==0) {return(nobeep(i));};
    if (strcmp(parv[0],"autoprompt")==0) {return(autoprompt(i));};
    if (strcmp(parv[0],"linewrap")==0) {return(linewrap(i));};
    if (strcmp(parv[0],"texttag")==0) {return(texttag(i));};
    if (strcmp(parv[0],"sayshout")==0) {return(sayshout(i));};
    if (strcmp(parv[0],"seeecho")==0) {return(seeecho(i));};
    if (strcmp(parv[0],"shield")==0) {return(shield(i));};
    if (strcmp(parv[0],"leavein")==0) {return(leavein(i));};
    if (strcmp(parv[0],"mindopen")==0) {return(mindopen(i));};
    if (strcmp(parv[0],"mindlock")==0) {return(mindlock(i));};
    if (strcmp(parv[0],"converse")==0) {return(converse(i));};
    if (strcmp(parv[0],"inform")==0) {return(inform(i));};
    if (strcmp(parv[0],"ignore")==0) {return(ignore(i));};
    if (strcmp(parv[0],"grabme")==0) {return(grabme(i));};
    if (strcmp(parv[0],"noshield")==0) {return(noshield(i));};
    if (strcmp(parv[0],"invite")==0) {return(invite(i));};
    if (strcmp(parv[0],"bar")==0) {return(bar(i));};
    if (strcmp(parv[0],"friend")==0) {return(friend(i));};
    if (strcmp(parv[0],"listclear")==0) {return(listclear(i));};
    if (strcmp(parv[0],"listdo")==0) {return(listdo(i));};
    if (strcmp(parv[0],"qw")==0) {return(qw(i));};
    if (strcmp(parv[0],"qf")==0) {return(qf(i));};
    if (strcmp(parv[0],"list")==0) {return(list(i));};
    if (strcmp(parv[0],"grab")==0) {return(grab(i));};
    if (strcmp(parv[0],"grabs")==0) {return(grabs(i));};
    if (strcmp(parv[0],"invites")==0) {return(invites(i));};
    if (strcmp(parv[0],"ttyheight")==0) {return(ttyheight(i));};
    if (strcmp(parv[0],"ttywidth")==0) {return(ttywidth(i));};
    if (strcmp(parv[0],"ignoremsg")==0) {return(ignoremsg(i));};
    if (strcmp(parv[0],"notice")==0) {return(notice(i));};
    if (strcmp(parv[0],"board")==0) {return(board(i));};
    if (strcmp(parv[0],"saved")==0) {return(saved(i));};
    if (strcmp(parv[0],"whisper")==0) {return(whisper(i));};
    if ((users[i].flags&1)!=0) {
    if (strcmp(parv[0],"mail")==0) {return(domail(i));};
    if (strcmp(parv[0],"read")==0) {return(readmail(i));};
    if (strcmp(parv[0],"reply")==0) {return(mailreply(i));};
    if (strcmp(parv[0],"delete")==0) {return(deletemail(i));};
    if (strcmp(parv[0],"object")==0) {return(objectto(i));};
    }
    if (strcmp(parv[0],"echoall")==0)
	if ((users[i].flags&0x8000000)!=0) {return(echoall(i));};
    if (strcmp(parv[0],"evict")==0)
	if ((users[i].flags&0x10000000)!=0) {return(evict(i));};
    if (strcmp(parv[0],"openhouse")==0)
	if ((users[i].flags&0x00000400)!=0) {return(openhouse(i));};
    if (strcmp(parv[0],"trace")==0)
	if ((users[i].flags&0x00800000)!=0) {return(dotrace(i));};
    if ((users[i].flags&0x20000000)!=0) {
	if (strcmp(parv[0],"hosts")==0) {return(hosts(i));};
	if (strcmp(parv[0],"beep")==0) {return(beep(i));};
    }
    if ((users[i].flags&0x200)==0) return -1;
    if (strcmp(parv[0],"echoat")==0) {return(echoat(i));};
    if (strcmp(parv[0],"echoto")==0) {return(echoto(i));};
    if (strcmp(parv[0],"crash")==0) {crashme(i);};
    if (strcmp(parv[0],"hostinfo")==0) {return(hostinfo(i));};
    if (strcmp(parv[0],"hostadd")==0) {return(hostadd(i));};
    if (strcmp(parv[0],"hostdo")==0) {return(hostdo(i));};
    if (strcmp(parv[0],"patch")==0) {return(patch(i));};
    if (strcmp(parv[0],"nums")==0) {return(nums(i));};
    if (strcmp(parv[0],"editors")==0) {return(editors(i));};
    if (strcmp(parv[0],"stats")==0) {return(stats(i));};
    if (strcmp(parv[0],"emails")==0) {return(emails(i));};
    if (strcmp(parv[0],"comments")==0) {return(comments(i));};
    if (strcmp(parv[0],"email")==0) {return(email(i));};
    if (strcmp(parv[0],"comment")==0) {return(comment(i));};
    if (strcmp(parv[0],"allow")==0) {return(allow(i));};
    if (strcmp(parv[0],"deny")==0) {return(deny(i));};
    if (strcmp(parv[0],"bestow")==0) {return(bestow(i));};
    if (strcmp(parv[0],"ban")==0) {return(ban(i));};
    if (strcmp(parv[0],"goto")==0) {return(mega_goto(i));};
    if (strcmp(parv[0],"ungag")==0) {return(ungag(i));};
    if (strcmp(parv[0],"gag")==0) {return(gag(i));};
    if (strcmp(parv[0],"jump")==0) {return(jump(i));};
    if (strcmp(parv[0],"arrive")==0) {return(quietjump(i));};
    if (strcmp(parv[0],"move")==0) {return(move(i));};
    if (strcmp(parv[0],"info")==0) {return(fileinfo(i));};
    if (strcmp(parv[0],"throat")==0) {return(throat(i));};
    if (strcmp(parv[0],"loadrooms")==0) {loadrooms();return 0;};
    if (strcmp(parv[0],"saverooms")==0) {saverooms();return 0;};
    if (strcmp(parv[0],"loadexits")==0) {loadexits();return 0;};
    if (strcmp(parv[0],"saveexits")==0) {saveexits();return 0;};
    if (strcmp(parv[0],"roominfo")==0) {roominfo(i);return 0;};
    if (strcmp(parv[0],"roomedit")==0) {return(roomedit(i));};
    if (strcmp(parv[0],"roomexit")==0) {exitname(i);return 0;};
    if (strcmp(parv[0],"roomlist")==0) {listrooms(i);return 0;};
    if (strcmp(parv[0],"maxlass")==0) {return(maxlass(i));};
    if (strcmp(parv[0],"chpass")==0) {return(chpass(i));};
    if (strcmp(parv[0],"ttys")==0) {return(ttys(i));};
    if (strcmp(parv[0],"sendchar")==0) {return(sendchar(i));};
    if (strcmp(parv[0],"rename")==0) {return(myrename(i));};
    if (strcmp(parv[0],"eradicate")==0) {return(eradicate(i));};
    if (strcmp(parv[0],"mailstat")==0) {return(mailstat(i));};
    if (strcmp(parv[0],"mailfind")==0) {return(mailfind(i));};
    if (strcmp(parv[0],"maillimit")==0) {return(maillimit(i));};
    if (strcmp(parv[0],"stat")==0) {return(mystat(i));};
    if (strcmp(parv[0],"hash")==0) {return(showhash(i));};
    if (strcmp(parv[0],"files")==0) {return(files(i));};
    if (strcmp(parv[0],"users")==0) {return(allusers(i));};
    if (strcmp(parv[0],"cleanmail")==0) {return(mailredo(i));};
    if (strcmp(parv[0],"delnotice")==0) {return(delnotice(i));};
    if (strcmp(parv[0],"control")==0) {return(housecontrol(i));};
    if (strcmp(parv[0],"locate")==0) {return(locate(i));};
    if (strcmp(parv[0],"through")==0) {return(throughput(i));};
    if (strcmp(parv[0],"whopop")==0) {return(whopop(i));};
    return -1;
}

/****************************************************************************
  FUNCTION - sillycrypt(string)
  Returns the 'crypt value' for the input string as used for the horribly
  kludgy old password system.
 ****************************************************************************/

sillycrypt(a)
char* a;
{
    int i,j,k;
    char c;
    char crypst[64];
    (void) strncpy(crypst,a,63);
    j=0;
    k=0;
    for(i=0;(c=crypst[i])!=0;i++) {
        j=j+(int) c;
        k=k ^ (int) c;}
    j=j+256*k;
    j=j*i;
    return j;
}

/****************************************************************************
  FUNCTION - banished(string)
  Returns whether the string passed constitutes a banished name or not.
  Ie, if it IS banished, return True (non-zero) otherwise false.
 ****************************************************************************/

banished(a)
char *a;
{
    char nom[16];
    int j;
    FILE *bfptr;
    if (strstr(a,"fuck")!=NULL) return -1;
    if (strstr(a,"cunt")!=NULL) return -1;
    if (strstr(a,"wank")!=NULL) return -1;
    if (strstr(a,"shit")!=NULL) return -1;
    if (strstr(&a[1],"shag")!=NULL) return -1;
    if (strstr(a,"penis")!=NULL) return -1;
    if (strstr(a,"vagina")!=NULL) return -1;
    if (strstr(a,"kcuf")!=NULL) return -1;
    if (strstr(a,"evict")!=NULL) return -1;
    if ((bfptr=fopen(BANSFILE,"rb"))==NULL) return 0;
    nom[0]=0;
    for (;;) {
	for (j=0;(j!=16);j++) {
	    if ((nom[j]=fgetc(bfptr))==EOF) {
		(void) fclose(bfptr);
		return 0;};
	    if (nom[j]=='\n') break;}
	nom[j]=0;
	if (strcmp(a,nom)==0) {
	    (void) fclose(bfptr);
	    return -2;}
    }
}
    











