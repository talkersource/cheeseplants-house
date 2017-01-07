#include "header.h"

/****************************************************************************
 ****************************************************************************/

look(i,r)
int i;
{
    if (r>=1000000) {
	/* Must be a mindscape.... */
	rshow(i,users[r-1000000].scapedesc);
	rshow(i,"\n");
    } else {
	if (r==0) {
	    rshow(i,"WHERE THE HELL ARE YOU???!!!\n");
	    return;
	} else {
	    if ((r<0)||(r>=MAXROOMS)) {
		rshow(i,"Erk! Something is certainly UP!\n");
		return;
	    } else {
		rshow(i,Roomdesc[r]);
		rshow(i,"\n");
	    }
	}
    }
    (void) lookusers(i,r);
    if (((users[i].flags&1)!=0)&&(users[i].passwd[0]==0)&&(users[i].oldpasswd==0)) 
	rshow(i,"\007     YOU NEED TO SET A PASSWORD! PLEASE DO SO USING THE 'PASS' COMMAND!!!\n");
    return;
}

/****************************************************************************
 ****************************************************************************/

lookusers(i,r)
int i,r;
{
    int j;
    if (r==0) return;
    if (r<1000000) {
	j=Roomuser[r];
    } else {
	j=users[r-1000000].scapeuser;
    }
    if (j==-1) {
	rshow(i,"\nIn this room you see noone.\n");
    } else {
	rshow(i,"\nIn this room you see...\n");
	for (;j!=-1;j=users[j].nextroom) {
	    if (users[j].ltitle[0]!=0) {
		if (j!=i) {
		    (void) sprintf(stringo,"%s %s\n",users[j].name,users[j].ltitle);
		} else {
		    (void) sprintf(stringo,"Yourself, %s %s\n",users[j].name,users[j].ltitle);
		}
	    } else {
		if (j!=i) {
		    (void) sprintf(stringo,"%s %s\n",users[j].name,users[j].title);
		} else {
		    (void) sprintf(stringo,"Yourself, %s %s\n",users[j].name,users[j].title);
		}
	    }
	    rshow(i,stringo);
	}
    }
    return;
}

/****************************************************************************
 ****************************************************************************/

moveuser(i,j,a,b)
int i,j;
char *a,*b;
{
    if (users[i].room==j) return;
    users[i].ltitle[0]=0;
    (void) outroom(i,users[i].room);
    rsay(i,users[i].room,a,0,i,-1,-1);
    rsay(i,j,b,0,i,-1,-1);
    (void) inroom(i,j);
    if (j!=0) look(i,users[i].room);
    return;
}

/****************************************************************************
 ****************************************************************************/

inroom(i,j)
int i,j;
{
    users[i].room=j;
    if (j==0) return;
    if (j>=1000000) {
	if (users[j-1000000].scapeuser!=-1)
	    users[users[j-1000000].scapeuser].lastroom=i;
	users[i].nextroom=users[j-1000000].scapeuser;
	users[i].lastroom=-1;
	users[j-1000000].scapeuser=i;
	return;
    } else {
	if (Roomuser[j]!=-1)
	    users[Roomuser[j]].lastroom=i;
	users[i].nextroom=Roomuser[j];
	users[i].lastroom=-1;
	Roomuser[j]=i;
	return;
    }
}
    
/****************************************************************************
 ****************************************************************************/

outroom(i,j)
int i,j;
{
    if (j==0) return;
    if (j>=1000000) {
    if (users[i].lastroom!=-1)
        users[users[i].lastroom].nextroom=users[i].nextroom;
    if (users[i].nextroom!=-1)
        users[users[i].nextroom].lastroom=users[i].lastroom;
    if (users[j-1000000].scapeuser==i) {
        users[j-1000000].scapeuser=users[i].nextroom;};
    return;
    } else {
    if (users[i].lastroom!=-1)
        users[users[i].lastroom].nextroom=users[i].nextroom;
    if (users[i].nextroom!=-1)
        users[users[i].nextroom].lastroom=users[i].lastroom;
    if (Roomuser[j]==i)
        Roomuser[j]=users[i].nextroom;
    return;
    }
}

/****************************************************************************
 ****************************************************************************/

mindscape(i)
int i;
{
    if (users[i].room==1000000+i) {
	rshow(i,"You're whisked from your mindscape by a cloud of pink fish!\n");
	(void) sprintf(stringp,"%s vanishes in a pink whirl.\n",users[i].name);
	(void) sprintf(stringq,"%s appears, resolving slowly in spinning pink dots.\n",users[i].name);
	moveuser(i,1,stringp,stringq);
	return 0;
    }
    rshow(i,"You transfer yourself to your mindscape.\n");
    if (users[i].room<1000000) {
	(void) sprintf(stringp,"%s dissolves and shoots skywards as a gold spark.\n",users[i].name);
	(void) sprintf(stringq,"%s forms from a throbbing golden light.\n",users[i].name);
	moveuser(i,1000000+i,stringp,stringq);
    } else {
	(void) sprintf(stringp,"%s vanishes with a resounding thud.\n",users[i].name);
	(void) sprintf(stringq,"%s appears startlingly fast! *Pop!*\n",users[i].name);
	moveuser(i,1000000+i,stringp,stringq);
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

leave(i)
int i;
{
    if (users[i].room<1000000) {
	rshow(i,"You cant leave from here!\n");
	return 0;};
    rshow(i,"You're pulled back down by a powerful force.\n");
    (void) sprintf(stringp,"%s vanishes through the floor leaving a faint trail of blue feathers.\n",users[i].name);
    (void) sprintf(stringq,"%s floats down accompanied by some blue birdlike creatures.\n",users[i].name);
    moveuser(i,1,stringp,stringq);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

visit(i)
int i;
{
    int j,l;
    if (parv[1][0]==0) return -7;
    if ((j=scanuser(parv[1]))<0) return j;
    if ((users[i].flags&0x200)==0) {
	l=onlist(j,users[i].number);
	if (l!=-1) {
	    if ((users[j].inviteflags[l]&32)!=0) {
		rshow(i,"You're barred from that mindscape.\n");
		return 0;}
	}
	if ((users[j].flags&0x100000)!=0) {
	    rshow(i,"That mindscape is locked.\n");
	    return 0;
	}
	if ((users[j].flags&0x80000)==0) {
	    if (l==-1) {
		rshow(i,"You're not invited to that mindscape.\n");
		return 0;}
	    if ((users[j].inviteflags[l]&16)==0) {
		rshow(i,"You're not invited to that mindscape.\n");
		return 0;}
	}
    }
    (void) sprintf(stringo,"You whizz off to %s's mindscape.....\n",users[j].name);
    rshow(i,stringo);
    if (users[i].room<1000000) {
	(void) sprintf(stringp,"%s dissolves and shoots skywards as a silver spark.\n",users[i].name);
	(void) sprintf(stringq,"%s forms from a pulsating silver light.\n",users[i].name);
	moveuser(i,1000000+j,stringp,stringq);
    } else {
	(void) sprintf(stringp,"%s disappears 'sideways'??!?.\n",users[i].name);
	(void) sprintf(stringq,"%s condenses with a nice 'phut' sound.\n",users[i].name);
	moveuser(i,1000000+j,stringp,stringq);
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

roomname(r,s)
int r;
char *s;
{
    if (r>=1000000) {
	(void) strcpy(s,users[r-1000000].scapename);
    } else {
	if (r==0) {(void) strcpy(s,"trouble!! <VOID!>");return;}
	if ((r<0)||(r>=MAXROOMS)) {(void) strcpy(s,"somewhere ODD.");return;};
	(void) strcpy(s,Roomname[r]);
    }
    return;
}


/****************************************************************************
 ****************************************************************************/

eject(i)
int i;
{
    int j,k,l;
    if (parv[1][0]=='*') {
	k=scanuser(parv[2]);
	for(j=users[i].scapeuser;j!=-1;j=l) {
	    l=users[j].nextroom;
	    if ((j!=i)&&(j!=k)) {
		rshow(j,"You are ejected from the mindscape.\n");
		(void) sprintf(stringo,"%s vanishes in a burst of orange flame.\n",users[j].name);
		(void) sprintf(stringp,"%s arrives, smoking ever so slightly.\n",users[j].name);
		moveuser(j,j+1000000,stringo,stringp);
	    }
	}
	rshow(i,"Ejection complete.\n");
	return 0;
    }
    if ((j=scanuser(parv[1]))<0) return j;
    if (users[j].room!=(1000000+i)) {
	(void) sprintf(stringo,"%s is not in your mindscape!\n",users[j].name);
	rshow(i,stringo);
	return 0;
    }
    if (j==i) {
	rshow(i,"Dont be silly, try leave instead.\n");
	return 0;
    }
    rshow(j,"You are ejected from the mindscape.\n");
    (void) sprintf(stringo,"%s vanishes in a burst of orange flame.\n",users[j].name);
    (void) sprintf(stringp,"%s arrives, smoking ever so slightly.\n",users[j].name);
    moveuser(j,j+1000000,stringo,stringp);
    if (users[i].room!=(1000000+i)) {
	(void) sprintf(stringo,"%s has been ejected from your mindscape.\n",users[j].name);
	rshow(i,stringo);
    }
    return 0;
}
		
/****************************************************************************
 ****************************************************************************/

whichroom(a)
char *a;
{
    int r;
    if (a[0]=='.') {
	int j;
	if ((j=scanuser(&a[1]))<0) return j;
	return (j+1000000);
    }
    if ((atoi(a)>0)&&(atoi(a)<MAXROOMS)) return(atoi(a));
    if (strcmp(a,"void")==0) return 0;
    for (r=0;r<MAXROOMS;r++) if (strcmp(a,Roomexit[r])==0) return r;
    return -8;
}
    
/****************************************************************************
 ****************************************************************************/

loadrooms()
{
    FILE *rptr;
    int r,i;
    char c;
    if ((rptr=fopen(ROOMFILE,"rb"))==NULL) return;
    for (r=1;r<MAXROOMS;r++) {
	c=42;
	for (i=0;c!=0;i++) if ((c=(Roomname[r][i]=fgetc(rptr)))==EOF) {(void) fclose(rptr);return;};
	c=42;
	for (i=0;c!=0;i++) if ((c=(Roomexit[r][i]=fgetc(rptr)))==EOF) {(void) fclose(rptr);return;};
	c=42;
	for (i=0;c!=0;i++) if ((c=(Roomdesc[r][i]=fgetc(rptr)))==EOF) {(void) fclose(rptr);return;};
    }
    return;
}

/****************************************************************************
 ****************************************************************************/

saverooms()
{
    FILE *rptr;
    int r,i;
    if ((rptr=fopen(ROOMFILE,"wb"))==NULL) return;
    for (r=1;r<MAXROOMS;r++) {
	for (i=0;i!=strlen(Roomname[r])+1;i++) (void) fputc(Roomname[r][i],rptr);
	for (i=0;i!=strlen(Roomexit[r])+1;i++) (void) fputc(Roomexit[r][i],rptr);
	for (i=0;i!=strlen(Roomdesc[r])+1;i++) (void) fputc(Roomdesc[r][i],rptr);
    }
    (void) fclose(rptr);
    return;
}

/****************************************************************************
 ****************************************************************************/

roominfo(i)
int i;
{
    int r;
    r=users[i].room;
    if (r<1000000) {
	if (r==0) {
	    rshow(i,"You're in the damn void!!!\n");
	    return;
	} else {
	    (void) sprintf(stringo,"Room number :%d\nRoom name   :%s\nExit Name   :%s\n\n",r,Roomname[r],Roomexit[r]);
	    rshow(i,stringo);
	    rshow(i,Roomdesc[r]);
	    rshow(i,"\n");
	    return;
	}
    } else {
	rshow(i,"I'll do this some other time\n");
	return;
    }
}
		    
/****************************************************************************
 ****************************************************************************/

exitname(i)
int i;
{
    if ((users[i].room<1)||(users[i].room>=MAXROOMS)) {
	rshow(i,":You cant do that here!!!!\n");
	return;}
    if (parv[1][0]==0) {
	(void) sprintf(stringo,"Exitname set to: %s\n",Roomexit[users[i].room]);
	rshow(i,stringo);
    } else {
	(void) strncpy(Roomexit[users[i].room],parr[1],15);
	(void) sprintf(stringo,"Exitname now set to: %s\n",Roomexit[users[i].room]);
	rshow(i,stringo);
    }
    return;
}

/****************************************************************************
 ****************************************************************************/

listrooms(i)
int i;
{
    int r;
    int s;
    s=0;
    r=atoi(parv[1]);
    if (r<=0) r=1;
    for (;r<MAXROOMS;r++) {
	if (Roomexit[r][0]!=0) {
	    s++;
	    (void) sprintf(stringo,"%2d %-16s:%s\n",r,Roomexit[r],Roomname[r]);
	    rshow(i,stringo);
	    if (s==20) return;
	}
    }
    return;
}

/****************************************************************************
 ****************************************************************************/

showexits(i)
int i;
{
    int j;
    int k;
    if (users[i].room==0) {
	rshow(i,"There are no exits from the VOID!\n");
	return 0;
    }
    if (users[i].room>=1000000) {
	rshow(i,"There are no exits from a mindscape\n");
	return 0;
    }
    if ((users[i].room<1)||(users[i].room>=MAXROOMS)) return 0;
    k=0;
    for (j=0;j<MAXEXITS;j++) {
	if (Exitfrom[j]==users[i].room) {
	    if ((Exitflags[j]&1)==0) {
		(void) sprintf(stringo,"go %-16s-> Goto %s\n",Roomexit[Exitto[j]],Roomname[Exitto[j]]);
	    } else {
		(void) sprintf(stringo,"go %-16s-> Goto %s (LOCKED)\n",Roomexit[Exitto[j]],Roomname[Exitto[j]]);
	    }
	    k++;
	    rshow(i,stringo);
	} else {
	    if (Exitto[j]==users[i].room) {
	    if ((Exitflags[j]&1)==0) {
		(void) sprintf(stringo,"go %-16s-> Goto %s\n",Roomexit[Exitfrom[j]],Roomname[Exitfrom[j]]);
	    } else {
		(void) sprintf(stringo,"go %-16s-> Goto %s (LOCKED)\n",Roomexit[Exitfrom[j]],Roomname[Exitfrom[j]]);
	    }
	    rshow(i,stringo);
	    k++;
	    }
	}
    }
    if (k==0) {
	rshow(i,"There are no exits from here.\n");
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

loadexits()
{
    FILE *rptr;
    int r,i;
    char c;
    char ebuf[6];
    if ((rptr=fopen(EXITFILE,"rb"))==NULL) return;
    for (r=1;r<MAXEXITS;r++) {
	c=42;
	for (i=0;c!=':';i++) if ((c=(ebuf[i]=fgetc(rptr)))==EOF) {(void) fclose(rptr);return;};
	ebuf[i-1]=0;
	Exitfrom[r]=atoi(ebuf);
	c=42;
	for (i=0;c!=':';i++) if ((c=(ebuf[i]=fgetc(rptr)))==EOF) {(void) fclose(rptr);return;};
	ebuf[i-1]=0;
	Exitto[r]=atoi(ebuf);
	c=42;
	for (i=0;c!=10;i++) if ((c=(ebuf[i]=fgetc(rptr)))==EOF) {(void) fclose(rptr);return;};
	ebuf[i-1]=0;
	Exitflags[r]=atoi(ebuf);
    }
    return;
}

/****************************************************************************
 ****************************************************************************/

saveexits()
{
    FILE *rptr;
    int r;
    if ((rptr=fopen(EXITFILE,"wb"))==NULL) return;
    for (r=1;r<MAXEXITS;r++)
	if (Exitfrom[r]!=0) 
	    (void) fprintf(rptr,"%d:%d:%d\n",Exitfrom[r],Exitto[r],Exitflags[r]);
    (void) fclose(rptr);
    return;
}

/****************************************************************************
 ****************************************************************************/

enter(i)
int i;
{
    int r,j;
    if (parv[1][0]==0) return -9;
    for (r=1;r<MAXROOMS;r++)
	if (strcmp(parv[1],Roomexit[r])==0) {
	    for (j=0;j<MAXEXITS;j++) {
		if ((Exitfrom[j]==users[i].room)&&(Exitto[j]==r)) {
		    if ((Exitflags[j]&1)!=0) {
			rshow(i,"That exit is locked!\n");
			return 0;
		    } else {
			(void) sprintf(stringo,"You enter %s\n",Roomname[r]);
			(void) sprintf(stringp,"%s leaves for %s\n",users[i].name,Roomname[r]);
			(void) sprintf(stringq,"%s enters.\n",users[i].name);
			rshow(i,stringo);
			moveuser(i,r,stringp,stringq);
			return 0;
		    }
		} else {
		    if ((Exitto[j]==users[i].room)&&(Exitfrom[j]==r)) {
			if ((Exitflags[j]&1)!=0) {
			    rshow(i,"That exit is locked!\n");
			    return 0;
			} else {
			    (void) sprintf(stringo,"You enter %s\n",Roomname[r]);
			    (void) sprintf(stringp,"%s leaves for %s\n",users[i].name,Roomname[r]);
			    (void) sprintf(stringq,"%s enters.\n",users[i].name);
			    rshow(i,stringo);
			    moveuser(i,r,stringp,stringq);
			    return 0;
			}
		    }
		}
	    }
	}
    return -9;
}
