#include "header.h"

/****************************************************************************
 ****************************************************************************/

prelogmenu(i)
int i;
{
    normalerr(i,parsemenu(i));
}

/****************************************************************************
 ****************************************************************************/

parsemenu(i) 
int i;
{
    if ((parv[0][0]=='\\')||(parv[0][0]=='/')) {
	strcpy(stringo,parv[0]);
	strcpy(parv[0],&stringo[1]);
    }
    if (strcmp(parv[0],"who")==0) return who(i);
    if (users[i].flags&0x40)
    if (strcmp(parv[0],"qw")==0) return qw(i);
    if (strcmp(parv[0],"quit")==0) {LostConnect(i);return 0;}
    if (strcmp(parv[0],"help")==0) return displaymenu(i);
    if (strcmp(parv[0],"mind")==0) return gotohouse(i,i+1000000);
    if (strcmp(parv[0],"in")==0) return gotohouse(i,1);
    if (strcmp(parv[0],".")==0) return gotohouse(i,1);
    if (strcmp(parv[0],"mail")==0) return frontmail(i);
    if (strcmp(parv[0],"reply")==0) return mailreply(i);
    if (strcmp(parv[0],"read")==0) return readmail(i);
    if (strcmp(parv[0],"delete")==0) return deletemail(i);
    rshow(i,"Unknown option, Type 'help' for commands.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

displaymenu(i)
int i;
{
    rshow(i,"\n\n                 ***** Welcome to Cheeseplant's House. *****\n\n");
    rshow(i,"                          Please select an option:\n\n");
    rshow(i,"        who    - List the current users. ('who -' for quick)\n");
    if (users[i].flags&0x40)
    rshow(i,"        qw     - List your friends amongst the current users.\n");
    rshow(i,"        quit   - Log out here and now.\n");
    rshow(i,"        help   - Shows this screen.\n");
    rshow(i,"        mail   - List your items of mail (or send mail).\n");
    rshow(i,"        reply  - Replies to an item of mail.\n");
    rshow(i,"        read   - Read a piece of mail.\n");
    rshow(i,"        delete - Delete a piece of mail.\n");
    rshow(i,"        mind   - Enter your mindscape.\n");
    rshow(i,"        in or .- Enter the gathering place.\n");
    return 0;
}

/****************************************************************************
  FUNCTION - gotohouse(chan,room);
  Finally thuds the user on channel chan into the house into the room given.
  Calls announce to tell everyone they have arrived too! (oh goody i hear you
  say)
 ****************************************************************************/

gotohouse(i,r)
int i,r;
{
    users[i].doing=100;
    sprintf(stringo,"%s enters from outside the house.\n",users[i].name);
    moveuser(i,r,"",stringo);
    announce(i);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

frontmail(i)
int i;
{
    return domail(i);
}

/****************************************************************************
 ****************************************************************************/

prehitreturn(i)
int i;
{
    users[i].doing--;
    switch (users[i].doing) {
    case 1002:displaydisclaim(i);break;
    case 1001:displaydisclaimb(i);break;
    }
    if (users[i].doing==1000)
	if (users[i].flags&0x1) {
	    displaymenu(i);
	    if (getmailrcvd(users[i].number)>0)
		if (getnewmailrcvd(users[i].number)) {
		    rshow(i,"\n       *****  You have NEW mail!! Type 'mail' to see your mailbox.  *****\n");
		} else {
		    rshow(i,"\n         *****  You have mail. Type 'mail' to see your mailbox.  *****\n");
		}
	    return;
	} else {
	    users[i].doing=100;
	    sprintf(stringo,"%s enters from outside the house.\n",
		    users[i].name);
	    moveuser(i,1,"",stringo);
	    announce(i);
	    return;
	}
}

/****************************************************************************
 ****************************************************************************/

displaydisclaim(i)
int i;
{
    rshow(i,"                 Cheeseplant's House: 137.205.192.5 2001\n");
    rshow(i,"                 ---------------------------------------\n");
    rshow(i,"Recent changes/Additions:\n");
    rshow(i,"     18/01/92:  Added recently: texttag, listclear, listdo.\n");
    rshow(i,"                                with, ttyheight, ttywidth.\n");
    rshow(i,"                Help rules also updated.\n");
    rshow(i,"     21/01/92:  Removed echoto.\n");
    rshow(i,"     24/01/92:  Removed echoat.\n");
    rshow(i,"                Restricted echo in the gathering place.\n");
    rshow(i,"                Added 'sayshout' (help 5).\n");
    rshow(i,"     27/01/92:  Residents should see help 8.\n");
    rshow(i,"\n");
    return;
}

displaydisclaimb(i)
int i;
{
    rshow(i,"NOTE: There has been a lot of hassle recently due to people\n");
    rshow(i,"      Logging in under stupid names, Unfortunately, there is\n");
    rshow(i,"      No automatic way to find a username when that user logs\n");
    rshow(i,"      in so all i can do is restrict sites to residents only.\n");
    rshow(i,"      It gats worse where lass is concerned since lass has to\n");
    rshow(i,"      be treated as if it were ONE site. If the house is closed\n");
    rshow(i,"      then it is the result of someone being a jerk, And theres\n");
    rshow(i,"      nothing MORE i can do about it other than the restriction.\n");
    rshow(i,"\n");
    return;
}

