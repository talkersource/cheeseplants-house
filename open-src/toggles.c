#include "header.h"

/****************************************************************************
 ****************************************************************************/

toggle(s,b)
char s[];
int b;
{
    if (s[0]==0) {
	togand=-1;
	togeor=b;
	return 0;}
    if (strcmp(s,"on")==0) {
	togand=-1^b;
	togeor=b;
	return 0;}
    if (strcmp(s,"off")==0) {
	togand=-1^b;
	togeor=0;
	return 0;}
    togand=-1;
    togeor=0;
    if (strcmp(s,"?")==0) {
	return 1;}
    return -4;
}

/****************************************************************************
 ****************************************************************************/

padecho(i)
int i;
{
    (void) toggle(parv[1],1);
    users[i].localecho&=togand;
    users[i].localecho^=togeor;
    if (users[i].localecho==0) {
	rshow(i,"You have pad echoing turned OFF.\n");
    } else {
	rshow(i,"You have pad echoing turned ON.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

earmuffs(i)
int i;
{
    (void) toggle(parv[1],0x1000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x1000)==0) {
	rshow(i,"You are NOT wearing any earmuffs.\n");
    } else {
	rshow(i,"You ARE wearing some fluffy earmuffs.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

hide(i)
int i;
{
    (void) toggle(parv[1],0x2000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x2000)==0) {
	rshow(i,"You are NOT hiding from find.\n");
    } else {
	rshow(i,"You ARE hiding from find.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

informin(i)
int i;
{
    (void) toggle(parv[1],0x4000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x4000)==0) {
	rshow(i,"You are NOT getting informed of logins.\n");
    } else {
	rshow(i,"You ARE getting informed of logins.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

informout(i)
int i;
{
    (void) toggle(parv[1],0x8000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x8000)==0) {
	rshow(i,"You are NOT getting informed of logouts.\n");
    } else {
	rshow(i,"You ARE getting informed of logouts.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

nobeep(i)
int i;
{
    (void) toggle(parv[1],0x10000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x10000)==0) {
	rshow(i,"You ARE getting beeps on informs.\n");
    } else {
	rshow(i,"You are NOT getting beeps on informs.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

autoprompt(i)
int i;
{
    (void) toggle(parv[1],0x20000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x20000)==0) {
	rshow(i,"You are NOT using autoprompt.\n");
    } else {
	rshow(i,"You ARE using autoprompt.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

linewrap(i)
int i;
{
    (void) toggle(parv[1],0x40000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x40000)==0) {
	rshow(i,"You are NOT getting the house to linewrap.\n");
    } else {
	rshow(i,"You ARE getting the house to linewrap.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

texttag(i)
int i;
{
    (void) toggle(parv[1],0x00000004);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x00000004)==0) {
	rshow(i,"You are NOT getting the house to tag output.\n");
    } else {
	rshow(i,"You ARE getting the house to tag output.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

sayshout(i)
int i;
{
    (void) toggle(parv[1],0x00000010);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x00000010)==0) {
	rshow(i,"You are NOT treating local shouts like says.\n");
    } else {
	rshow(i,"You ARE treating local shouts like says.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

seeecho(i)
int i;
{
    (void) toggle(parv[1],0x400000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x400000)==0) {
	rshow(i,"You are NOT seeing who echoes what.\n");
    } else {
	rshow(i,"You ARE seeing who echoes what.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

shield(i)
int i;
{
    (void) toggle(parv[1],0x200000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x200000)==0) {
	rshow(i,"You are NOT shielded from tells & remotes.\n");
    } else {
	rshow(i,"You ARE shielded from tells and remotes.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

mindopen(i)
int i;
{
    (void) toggle(parv[1],0x80000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x80000)==0) {
	rshow(i,"Your mindscape is NOT open to all.\n");
    } else {
	rshow(i,"Your mindscape IS open to all.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

mindlock(i)
int i;
{
    (void) toggle(parv[1],0x100000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x100000)==0) {
	rshow(i,"Your mindscape is NOT locked against visits.\n");
    } else {
	rshow(i,"Your mindscape IS locked against visits.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

converse(i)
int i;
{
    (void) toggle(parv[1],0x40000000);
    users[i].flags&=togand;
    users[i].flags^=togeor;
    if ((users[i].flags&0x40000000)==0) {
	rshow(i,"You are NOT in converse mode.\n");
    } else {
	rshow(i,"You are in converse mode Type '/converse off'to exit..\n");
    }
    return 0;
}

