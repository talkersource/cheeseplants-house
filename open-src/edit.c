#include "header.h"

/****************************************************************************
 ****************************************************************************/

editcommand(i)
int i;
{
    if (parv[0][0]==0) return 0;
    if (parv[0][0]=='.') parv[0][0]=' ';
    if (parr[0][0]=='.') parr[0][0]=' ';
    if (parv[0][0]=='\\') parv[0][0]='/';
    if (parv[0][0]=='/') {
	/* Deal with commands in here. */
	if (strcmp("/help",parv[0])==0) return (edit_help(i));
	if (strcmp("/abandon",parv[0])==0) return (edit_abandon(i));
	if (strcmp("/look",parv[0])==0) return (edit_look(i));
	if (strcmp("/wipe",parv[0])==0) return (edit_wipe(i));
	if (strcmp("/delete",parv[0])==0) return (edit_delete(i));
	if (((users[i].doing%1000)==202)||((users[i].doing%1000)==203)) {
	    if (strcmp("/send",parv[0])==0) return (edit_send(i));
	} else {
	    if (strcmp("/use",parv[0])==0) return (edit_use(i));
	    if (strcmp("/name",parv[0])==0) return (edit_name(i));
	    if (strcmp("/store",parv[0])==0) return (edit_store(i));
	    if (strcmp("/undo",parv[0])==0) return (edit_undo(i));
	}
	rshow(i,"What? See /help for commands.\n");
    } else {
	/* Try to append line. */
	return (edit_append(i));
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

edit_help(i)
int i;
{
    switch (users[i].doing%1000) {
    case 200 : rshow(i,"                   Cheeseplant's house edit mode (Mindscape.)\n");break;
    case 201 : rshow(i,"                   Cheeseplant's house edit mode (Room.)\n");break;
    case 202 : rshow(i,"                    Cheeseplant's house mail editing mode\n");break;
    case 203 : rshow(i,"                    Cheeseplant's house mail editing mode (anon)\n");break;
    }
    rshow(i,"\n");
    rshow(i,"         /help      - Gives you this page!.\n");
    rshow(i,"         /look      - Shows you the contents of the buffer.\n");
    rshow(i,"         /wipe      - Clears the buffer completely.\n");
    rshow(i,"         /delete <n>- Deletes last n lines from the buffer.\n");
    if ((users[i].doing%1000)!=202) {
    rshow(i,"         /abandon   - Exit leaving the data intact, unedited.\n");
    rshow(i,"         /use       - Exit using the new version of the buffer.\n");
    rshow(i,"         /name <nam>- Sets the name to nam.\n");
    rshow(i,"         /store     - Stores the buffer (like use).\n");
    rshow(i,"         /undo      - Undoes whatever has been done since storage.\n");
    }else{
    rshow(i,"         /abandon   - Exit abandoning the mail\n");
    rshow(i,"         /send      - Sends the mail.....\n");
    }
    rshow(i,"\n");
    rshow(i,"               You can use \\ instead of / in front of commands.\n");
    rshow(i,"         Lines starting with a . are treated as if the . was a space.\n");
    return 0;
}
    
/****************************************************************************
 ****************************************************************************/

edit_look(i)
int i;
{
    rshow(i,"  ===========================================================================\n");
    rshow(i,edits[users[i].ednum].buffer);
    rshow(i,"\n  ===========================================================================\n");
    if (((users[i].doing%1000)!=202)&&((users[i].doing%1000)!=203)) {
	(void) sprintf(stringo,"Name: %s\n",edits[users[i].ednum].name);
	rshow(i,stringo);
    }
    return 0;
}

/****************************************************************************
 ****************************************************************************/

edit_send(i)
int i;
{
    if (edits[users[i].ednum].buffer[0]==0) {
	rshow(i,"No message... abandoning...\n");
	edits[users[i].ednum].user=-1;
	users[i].doing=users[i].olddoing;
	users[i].ednum=-1;
	return 0;
    };
    if ((users[i].doing%1000)==203) {
	sendmail(i,edits[users[i].ednum].buffer,edits[users[i].ednum].name,
		 0x00020000);
    } else {
	sendmail(i,edits[users[i].ednum].buffer,edits[users[i].ednum].name,0);
    }
    edits[users[i].ednum].user=-1;
    users[i].doing=users[i].olddoing;
    users[i].ednum=-1;
    return 0;
}

/****************************************************************************
 ****************************************************************************/

edit_use(i)
int i;
{
    (void) strcpy(edits[users[i].ednum].editing,edits[users[i].ednum].buffer);
    (void) strcpy(edits[users[i].ednum].editname,edits[users[i].ednum].name);
    edits[users[i].ednum].user=-1;
    users[i].doing=users[i].olddoing;
    users[i].ednum=-1;
    rshow(i,"Using edit buffers, saving all changes.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

edit_abandon(i)
int i;
{
    edits[users[i].ednum].user=-1;
    users[i].doing=users[i].olddoing;
    users[i].ednum=-1;
    rshow(i,"Abandoning edit mode, keeping no changes.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

edit_append(i)
int i;
{
    int l,ll;
    l=strlen(parr[0]);
    ll=strlen(edits[users[i].ednum].buffer);
    parr[0][78]=0;
    if ((ll+l+1)>1023) {
	(void) sprintf(stringo,"That line is %d characters too long...\n",(ll+l+1)-1023);
	rshow(i,stringo);
	return 0;
    }
    if (ll!=0) {
	edits[users[i].ednum].buffer[ll]='\n';
	edits[users[i].ednum].buffer[ll+1]=0;};
    (void) strcat(edits[users[i].ednum].buffer,parr[0]);
    rshow(i,"Line appended.\n");
    return 0;
}

/****************************************************************************
 ****************************************************************************/

edit_wipe(i)
int i;
{
    rshow(i,"You wipe the buffer.\n");
    edits[users[i].ednum].buffer[0]=0;
    return 0;
}

/****************************************************************************
 ****************************************************************************/

edit_delete(i)
int i;
{
    int n,j;
    rshow(i,"Deleting lines.\n");
    if ((n=atoi(parv[1]))<=0) n=1;
    j=strlen(edits[users[i].ednum].buffer);
    if (j==0) return 0;
    for (;j!=0;j--) {
	if (edits[users[i].ednum].buffer[j]=='\n') {
	    n--;
	    if (n==0) {edits[users[i].ednum].buffer[j]=0;
		       return 0;
		   }
	}
    }
    edits[users[i].ednum].buffer[0]=0;
    return 0;
}

/****************************************************************************
 ****************************************************************************/

edit_undo(i)
int i;
{
    rshow(i,"Undone.\n");
    (void) strcpy(edits[users[i].ednum].buffer,edits[users[i].ednum].editing);
    (void) strcpy(edits[users[i].ednum].name,edits[users[i].ednum].editname);
    return 0;
}

/****************************************************************************
 ****************************************************************************/

edit_store(i)
int i;
{
    rshow(i,"Stored.\n");
    (void) strcpy(edits[users[i].ednum].editing,edits[users[i].ednum].buffer);
    (void) strcpy(edits[users[i].ednum].editname,edits[users[i].ednum].name);
    return 0;
}
    
/****************************************************************************
 ****************************************************************************/

edit_name(i)
int i;
{
    (void) strncpy(edits[users[i].ednum].name,parr[1],63);
    (void) sprintf(stringo,"Name now: %.63s\n",parr[1]);
    rshow(i,stringo);
    return 0;
}
