#include "header.h"
#include "telnet.h"

/****************************************************************************
  FUNCTION - rprint(ch,string);
  Simply writes string to channel ch
 ****************************************************************************/

rprint(i,s)
int i;
char *s;
{
    write(i,s,strlen(s));
}


/****************************************************************************
 ****************************************************************************/

hideprompt(i)
int i;
{
    int l;
    users[i].plevel++;
    if (!(users[i].flags&0x000020000)) return;
    if (users[i].plevel==1) {
	if (tflags[i]&1) {
	    l=strlen(users[i].myprompt)+bufpos[i];
	    if (l>79) l=79;
	    write(i,"\015",1);
	    write(i,"                                                                                           ",l);
	    rprint(i,"\015");
	} else {
	    rprint(i,"\r\n");
	}
    }
}

/****************************************************************************
 ****************************************************************************/

showprompt(i)
int i;
{
    --users[i].plevel;
    if (!(users[i].flags&0x000020000)) return;
    if (users[i].plevel==0) {
	rprint(i,users[i].myprompt);
	if (bufpos[i])
	    write(i,buffer[i],bufpos[i]);
    }
}
    
/****************************************************************************
 ****************************************************************************/

/****************************************************************************
  FUNCTION - rshow(ch,string);
  Displays the string to user on channel ch, Copes merrily with autoprompt and
  other such things, such as linewrap. All code to handle a/p and l/w MUST be
  built into this function.
 ****************************************************************************/

rshow(i,j)
int i;
char j[];
{
    char outline[2048];
    int sp,ls,pt,lp;
    if (i<1) return;
    if (j==NULL) return;
    if (j[0]==0) return;
    hideprompt(i);

    if (users[i].flags&0x40000) {
	sp=0;ls=1;pt=0;lp=0;
	for(pt=0;;pt++) {
	    switch(j[pt]) {
	    case   0 : 
		if (lp) {
		    outline[lp]='\r';
		    outline[lp+1]='\n';
		    write(i,outline,lp+2);
		}
		showprompt(i);
		return ;
	    case '\n':
		outline[lp]='\r';
		outline[lp+1]='\n';
		write(i,outline,lp+2);
		lp=0;
		sp=0;ls=1;
		break;
	    case ' ' :
		if (ls==0) {
		    sp=lp;
		    ls=1;
		}
	    default:
		if (j[pt]!=' ') ls=0;
		outline[lp++]=j[pt];
		if (lp==(users[i].ttywidth)) {
		    if (sp) {
			outline[sp]='\r';
			outline[sp+1]='\n';
			write(i,outline,sp+2);
			pt-=(lp-sp);
		    } else {
			outline[lp]='\r';
			outline[lp+1]='\n';
			write(i,outline,lp+2);
		    }
		    sp=0;
		    outline[0]=' ';
		    outline[1]=' ';
		    lp=2;
		    for(;j[pt+1]==' ';pt++);
		    ls=1;
		}
	    }
	}
    } else {
	lp=0;
	for (pt=0;;pt++) {
	    switch(j[pt]) {
	    case 0:
		if (lp) {
		    outline[lp]='\r';
		    outline[lp+1]='\n';
		    write(i,outline,lp+2);
		}
		showprompt(i);
		return;
	    case '\n':
		outline[lp]='\r';
		outline[lp+1]='\n';
		write(i,outline,lp+2);
		lp=0;
		break;
	    default:
		outline[lp++]=j[pt];
	    }
	}
    }
}
	   
