#include "header.h"

/****************************************************************************
  FUNCTION - rshow(ch,string);
  Displays the string to user on channel ch, Copes merrily with autoprompt and
  other such things, such as linewrap. All code to handle a/p and l/w MUST be
  built into this function.
 ****************************************************************************/

/*
rshowb(i,j)
int i;
char j[];
{
    char outstr[2048];
    int k,l,sl;
    char c;
    if (i<1) return;
    if (j==NULL) return;
    if (j[0]==0) return;
    l=0;
    sl=strlen(j);
    for (k=0;(k<sl)&&(l<2040);k++)
        {
            c=j[k];
            if (c==0) break;
            if (c=='\n')
                { outstr[l]=13;
                  outstr[l+1]=10;
                  l+=2;
		  users[i].cposn=0;
              } else {
		  if (users[i].cposn==(WIDTH-1)) {
		      if ((users[i].flags&0x40000)!=0) {
			  outstr[l]=13;
			  outstr[l+1]=10;
			  l+=2;
		      }
		      users[i].cposn=0;};
                  outstr[l]=c;
                  l++;
		  users[i].cposn++;
              };
        }
    outstr[l]=0;
    (void) write(i,outstr,l);
    return;
}
*/

/****************************************************************************
  FUNCTION - finduser(name);
  Scans all users logged in and interacting for user 'name'. Needs an exact
  match. Returns -5 if no name given or -7 if name is not found. If name IS
  found then the user's channel number is returned.
 ****************************************************************************/

finduser(i)
char *i;
{
    int j;
    if (i[0]==0) return -5;
    for (j=Firstuser;j!=-1;j=users[j].nextuser)
	if (strcmp(unames[users[j].number],i)==0)
	    if (users[j].doing<1000) {
		return j;
	    } else {
		return -7;
	    }
    return -7;
}

/****************************************************************************
  FUNCTION - findfile(name);
  Scans through ALL users with a name (ie, resident and non-resident) using
  hash tables. Returns -5 if no name given, -7 if name not found otherwise
  returns the user's filenumber.
 ****************************************************************************/

findfile(i)
char *i;
{
    int j,k;
    if (i[0]==0) return -5;
    k=(i[0]&15)+16*(i[1]&15);
    for (j=nhash[k];j!=-1;j=nnext[j])
	if (strcmp(unames[j],i)==0) return j;
    return -7;
}

/****************************************************************************
  FUNCTION - rehash(num);
  Recalculates the hash table position of the user with file number num.
 ****************************************************************************/

renhash(n)
int  n;
{
    int i;
    if (nlast[n]==-2) {
	for (i=0;i<256;i++) if (nhash[i]==n) {
	    nhash[i]=nnext[n];
	    if (nnext[n]!=-1)
		nlast[nhash[i]]=-2;};
    } else {
	if (nlast[n]!=-1) nnext[nlast[n]]=nnext[n];
    }
    if (nnext[n]!=-1) nlast[nnext[n]]=nlast[n];
    if (unames[n][0]==0) {
	nlast[n]=-1;
	nnext[n]=-1;
	return;
    }
    nlast[n]=-2;
    i=(unames[n][0]&15)+16*(unames[n][1]&15);
    nnext[n]=nhash[i];
    nlast[nhash[i]]=n;
    nhash[i]=n;
}

/****************************************************************************
  FUNCTION - findusernum(num);
  Scans through all the currently interactive users for the user with file
  number num and returns the channel number if found.  Otherwise, return -1.
 ****************************************************************************/

findusernum(i)
int i;
{
    int j;
    for (j=Firstuser;j!=-1;j=users[j].nextuser)
	if (users[j].number==i) 
	    if (users[j].doing<1000) {
		return j;
	    } else {
		return -1;
	    }
    return -1;
}

/****************************************************************************
  FUNCTION - locateusernum(num);
  Searches through all interactive and non-interactive users for the user with
  file number num. If found returns channel number, otherwise returns -1.
 ****************************************************************************/

locateusernum(i)
int i;
{
    int j;
    for (j=Firstuser;j!=-1;j=users[j].nextuser)
	if (users[j].number==i) return j;
    return -1;
}

/****************************************************************************
  FUNCTION - scanuser(string);
  Does 'matching' on string to the interactive user it points to... Returns
  -5 if no name given. -7 if no match found. -6 if non-unique. Otherwise it
  returns the user's channel number.
 ****************************************************************************/

scanuser(i)
char *i;
{
    int j,k,namesleft;
    int check[MOST];
    if (i[0]==0) return -5;
    for (j=0;j<MOST;j++) check[j]=0;
    namesleft=0;
    for (j=Firstuser;j!=-1;j=users[j].nextuser)
	if ((users[j].room!=0)&&(users[j].doing<1000)) {
	    check[j]=1;
	    namesleft++;}
    for (k=0;i[k]!=0;k++) {
	for (j=Firstuser;j!=-1;j=users[j].nextuser) {
	    if ((check[j]!=0)&&(unames[users[j].number][k]!=i[k])) {
		check[j]=0;
		namesleft--;}
	}
    }
    if (namesleft==0) return -7;
    if (namesleft==1) for (j=Firstuser;j!=-1;j=users[j].nextuser) 
	if (check[j]!=0) return j;
    for (j=Firstuser;j!=-1;j=users[j].nextuser)
	if ((check[j]!=0)&&(strcmp(unames[users[j].number],i)==0)) return j;
    return -6;
}

/****************************************************************************
  FUNCTION - capstart(string);
  Copies string to 'capstr' and then removes bit 5 (32) of the first character
  of capstr to capitalise it.
 ****************************************************************************/

/*
capstart(i)
char i[];
{
    (void) strcpy(capstr,i);
    capstr[0]^=32;
}
*/

/****************************************************************************
  FUNCTION - announce(chan)
  Announces to all who really can be bothered that the user on channel chan
  has logged in.
 ****************************************************************************/

announce(i)
int i;
{
    int j;
    (void)sprintf(stringo,"\007\007%s has entered the house.\n",users[i].name);
    for (j=Firstuser;j!=-1;j=users[j].nextuser) 
	if ((j!=i)&&(users[j].doing<1000)) {
	    int k;
	    if ((k=onlist(j,users[i].number))>-1)
		if ((users[j].inviteflags[k]&1)==0) k=-1;
	    if (k!=-1) {
		rshow(j,stringo);
	    } else {
		if ((users[j].flags&0x4000)!=0) {
		    if ((users[j].flags&0x10000)==0) {
			rshow(j,stringo);
		    } else {
			rshow(j,&stringo[2]);
		    }
		}
	    }
	}
}

/****************************************************************************
  FUNCTION - doentry(chan);
  Finally adds the user on channel chan into the user-linked-list. Performs
  check to make sure they arent ALREADY there... Checks their mail...
  And does the inform on all users that are interactive and really want to
  know. 8-) NEEDS REWRITING SOMETIME DANIEL!!!!!!!!!!!!!!
 ****************************************************************************/

doentry(i)
int i;
{
    int j;
    for (j=Firstuser;j!=-1;j=users[j].nextuser)
	if (j!=i)
	    if (users[i].number==users[j].number) {
		rshow(i,"That user is *ALREADY* logged in!\n");
		KillConnect(i);
		return;
	    }
    Tempfile(i);
    if (Firstuser!=-1)
	users[Firstuser].lastuser=i;
    users[i].nextuser=Firstuser;
    users[i].lastuser=-1;
    Firstuser=i;
    users[i].doing=1003;
    if (users[i].flags&0x1) {
	rshow(i,"\n\n\n\n\n\n\n\n\n\n");
	rshow(i,"\n\n\n\n\n\n\n\n\n\n");
	prehitreturn(i);
	return;
    }
    if (getmailrcvd(users[i].number)>0)
	vapemail(users[i].number);
    prehitreturn(i);
    return;
}

/****************************************************************************
  FUNCTION - timestr(time,string);
  Prints the current time into string in the following format:
  18:12-06 Tue 26/11/91
 ****************************************************************************/

timestr(t,s)
int t;
char *s;
{
    char day[4];
    whattime=localtime((time_t *)&t);
    switch (whattime->tm_wday) {
    case 0 : (void) strcpy(day,"Sun");break;
    case 1 : (void) strcpy(day,"Mon");break;
    case 2 : (void) strcpy(day,"Tue");break;
    case 3 : (void) strcpy(day,"Wed");break;
    case 4 : (void) strcpy(day,"Thu");break;
    case 5 : (void) strcpy(day,"Fri");break;
    case 6 : (void) strcpy(day,"Sat");break;
    case 7 : (void) strcpy(day,"Sun");break;
    }
    (void) sprintf(s,"%.2d:%.2d-%.2d %s %.2d/%.2d/%d",whattime->tm_hour,whattime->tm_min,whattime->tm_sec,day,whattime->tm_mday,whattime->tm_mon+1,whattime->tm_year%100);
}

/****************************************************************************
  FUNCTION - ltimestr(time,string);
  Puts the elapsed time into string in one of the following formats:
  less than a second
  n Seconds
  n Mins n Seconds
  n Hours n Mins n Secs
  n Days n Hours n Mins n Secs
 ****************************************************************************/

ltimestr(t,s)
int t;
char *s;
{
    int da,ho,mi,se;
    char ses[10],mis[10],hos[10],das[20];
    if (t<=0) {
	(void) sprintf(s,"less than a second");
	return;};
    se=t%60;
    mi=t/60;
    ho=mi/60;
    mi%=60;
    da=ho/24;
    ho%=24;
    switch (se) {
    case 0 : ses[0]=0;break;
    case 1 : strcpy(ses,"1 Second");break;
    default: sprintf(ses,"%d Seconds",se);break;
    }
    switch (mi) {
    case 0 : mis[0]=0;break;
    case 1 : strcpy(mis,"1 Minute ");break;
    default: sprintf(mis,"%d Minutes ",mi);break;
    }
    switch (ho) {
    case 0 : hos[0]=0;break;
    case 1 : strcpy(hos,"1 Hour ");break;
    default: sprintf(hos,"%d Hours ",ho);break;
    }
    switch (da) {
    case 0 : das[0]=0;break;
    case 1 : strcpy(das,"1 Day ");break;
    default: sprintf(das,"%d Days ",da);break;
    }
    sprintf(s,"%s%s%s%s",das,hos,mis,ses);
    if (s[strlen(s)-1]==' ') s[strlen(s)-1]=0;
    return;
}

/****************************************************************************
  FUNCTION - doflag(val,flag,setstring,unsetstring,pos)
  l is set to length of unsetstring
  If (val&flag) is zero then replaces the l characters from pos onwards in
  stringo with unsetstring, otherwise uses the first l characters of setstring
 ****************************************************************************/

doflag(v,f,sm,um,mp)
int v,f,mp;
char *sm,*um;
{
    int l,i;
    l=strlen(um);
    if ((v&f)==0) {
	for(i=0;i<l;i++) stringo[mp++]=um[i];
    } else {
	for(i=0;i<l;i++) stringo[mp++]=sm[i];
    }
}
