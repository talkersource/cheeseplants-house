#include "header.h"

/****************************************************************************
  FUNCTION - help(chan)
  COMMAND SYNTAX - [page]
  Shows the user on channel chan the relevant help page by calling the correct
  function. I will not bother documenting them individually. If an invalid
  page is given, an error code is returned.
 ****************************************************************************/

help(i)
int i;
{
    int pge;
    if (strcmp(parv[1],"rules")==0) return rules(i);
    pge=atoi(parv[1]);
    if (pge<0) pge=0;
    switch (pge) {
    case 0 : return helpt(i);
    case 1 : return help1(i);
    case 2 : return help2(i);
    case 3 : return help3(i);
    case 4 : return help4(i);
    case 5 : return help5(i);
    case 6 : return help6(i);
    case 7 : return help7(i);
    case 8 : return help8(i);
    case 9 : return help9(i);
    case 10: return help10(i);
    }
    if ((users[i].flags&0x200)==0) return -3;
    switch (pge) {
    case 11 : return help11(i);
    case 12 : return help12(i);
    case 13 : return help13(i);
    }
    return -3;
}

helpt(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
    rshow(i,"\n");
    rshow(i,"                              Cheeseplant's House.\n");
    rshow(i,"                             Index of help pages....\n");
    rshow(i,"\n");
    rshow(i,"                     help 1  - Communicating.\n");
    rshow(i,"                     help 2  - Getting around.\n");
    rshow(i,"                     help 3  - Generally useful commands.\n");
    rshow(i,"                     help 4  - Your own character.\n");
    rshow(i,"                     help 5  - Optional things.\n");
    rshow(i,"                     help 6  - Your personal user list.\n");
    rshow(i,"                     help 7  - Your mindscape.\n");
    rshow(i,"                     help 8  - Mail & Other things (TM).\n");
    rshow(i,"                     help 9  - Read this notice!!!!!\n");
    rshow(i,"                     help 10 - Then read this one!!!!!\n");
    if ((users[i].flags&0x200)!=0) {
	rshow(i,"                     help 11 - Obtaining information.\n");
	rshow(i,"                     help 12 - Administrating users.\n");
	rshow(i,"                     help 13 - More of MY things.\n");
    }
    rshow(i,"\n");
    rshow(i,"                 Any problems, Email Cheesey@dcs.warwick.ac.uk\n");
    return 0;
}

help1(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                            help 1  - Communicating\n");
rshow(i,"\n");
rshow(i,"   say <thing>         - You say \"thing\" to everyone in your room.\n");
rshow(i,"   emote <thing>       - You emote thing. (ie. emote smiles.)\n");
rshow(i,"   te[ll] <who> <wot>  - Tells user who \"wot\".\n");
rshow(i,"   whisper <who> <wot> - Whispers 'wot' to user who.\n");
rshow(i,"   remote <who> <wot>  - Does an emote of wot to user who only.\n");
rshow(i,"   echo <wot>          - Just prints wot. (Dont abuse it.)\n");
if ((users[i].flags&0x8000000)!=0)
rshow(i,"   echoall <wot>       - Just prints wot to everyone.\n");
if ((users[i].flags&0x00000200)!=0)
{
rshow(i,"   echoat <who> <wot>  - Just prints wot in who's location.)\n");
rshow(i,"   echoto <who> <wot>  - Just prints wot to who.\n");
}
rshow(i,"   shout <wot>         - Shouts \"wot\" to everyone.\n");
rshow(i,"\n");
rshow(i,"               You can shorten some commands: say     ' \" `\n");
rshow(i,"                                              tell    >\n");
rshow(i,"                                              emote   : ;\n");
rshow(i,"                                              remote  <\n");
rshow(i,"                                              whisper =\n");
return 0;
}

help2(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                            help 2  - Getting around");
rshow(i,"\n");
rshow(i,"   l[ook]              - Shows you who's in the same room as you.\n");
rshow(i,"   exits               - See what exits there are from here.\n");
rshow(i,"   go <exit>           - Goes through the exit if unlocked.\n");
rshow(i,"   mindscape           - Enters your mindscape.\n");
rshow(i,"   leave               - Leaves your mindscape.\n");
rshow(i,"   visit <who>         - Visits who's mindscape (If you can.)\n");
return 0;
}

help3(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                      help 3  - Generally useful commands\n");
rshow(i,"\n");
rshow(i,"   who [<page>|-]      - Shows who's logged in (- for short form).\n");
rshow(i,"   quit                - Obvious really, You leave the house.\n");
rshow(i,"   idle <page|who>     - Tells you how idle everyone/who is.\n");
rshow(i,"   log <who>           - Shows when who last logged on.\n");
rshow(i,"   time                - Tells you the house (UK) time.\n");
rshow(i,"   where <page>        - Tells you where everyone is.\n");
rshow(i,"   find <who>          - Finds user who (If not hidden.)\n");
rshow(i,"   with <who>          - Finds who's with who (if not hiding.)\n");
rshow(i,"   examine <who>       - Examines who (Can shorten to exam, exa)\n");
return 0;
}

help4(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                          help 4  - Your own character\n");
rshow(i,"\n");
rshow(i,"   pass                - Change your password.\n");
rshow(i,"   recap [name]        - Change the capitalisation of your name.\n");
rshow(i,"   title <text|?       - Set your title to text.\n");
rshow(i,"   ltitle <text|?>     - Set your local title to text.\n");
rshow(i,"   describe <t|?|+ <t>>- Set your description to t, or adds t.\n");
rshow(i,"   prompt <whatever|?  - Set your prompt to whatever.\n");
rshow(i,"   cprompt <whatever|?>- Set your converse prompt to whatever.\n");
rshow(i,"   status              - Tells you what you can do.\n");
rshow(i,"   ignoremsg [message] - Sets the message people get when ignored by you.\n");
return 0;
}

help5(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                           help 5  - Optional things\n");
rshow(i,"\n");
rshow(i,"   earmuffs [tog]      - Controls if you hear shouts.\n");
rshow(i,"   hide [tog]          - You can hide from 'where' and 'find'.\n");
rshow(i,"   informin [tog]      - You are told of new logins.\n");
rshow(i,"   informout [tog]     - You are told of logouts.\n");
rshow(i,"   nobeep [tog]        - To control the inform beep.\n");
rshow(i,"   autoprompt [tog]    - Tries to keep cursor at bottom (LASS only)\n");
rshow(i,"   padecho [tog]       - Controls if the program echoes characters.\n");
rshow(i,"   linewrap [tog]      - Stops lines getting cut off.\n");
rshow(i,"   seeecho [tog]       - See who echoes things.\n");
rshow(i,"   shield [tog]        - To shield you from tells & remotes.\n");
rshow(i,"   converse [tog]      - To put you into conversation mode.\n");
rshow(i,"   texttag [tog]       - Tells difference between [r]emotes etc.\n");
rshow(i,"   sayshout [tog]      - Makes local shouts bypass earmuffs.\n");
rshow(i,"              [tog] can be: <nothing>  Toggle on/off\n");
rshow(i,"                            on         Set On\n");
rshow(i,"                            off        Set Off\n");
rshow(i,"                            ?          Show Current Setting\n");
rshow(i," In converse mode, put a / or \\ in front of commands (or use abbrevs.)\n");
return 0;
}

help6(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                       help 6  - Your personal user list\n");
rshow(i,"\n");
rshow(i,"   list <page>         - Shows you your list.\n");
rshow(i,"   inform <who> [tog]  - Do you know about who coming'n'going.\n");
rshow(i,"   ignore <who> [tog]  - Are you ignoring who.\n");
rshow(i,"   grabme <who> [tog]  - Can you be grabbed by who.\n");
rshow(i,"   noshield <who> [tog]- Is who immune to your mindshield.\n");
rshow(i,"   invite <who> [tog]  - Is who invited to your mindscape.\n");
rshow(i,"   bar <who> [tog]     - Is who barred from your mindscape.\n");
rshow(i,"   friend <who> [tog]  - Is who marked as a friend.\n");
rshow(i,"   listclear           - Clears your list.\n");
rshow(i,"   listdo <who> <flg>  - Allows you to set the status of a user.\n");
rshow(i,"   qw <page>           - Quick who of friends.\n");
rshow(i,"   qf <page>           - Quick find of friends.\n\n");
rshow(i," Format of flg for listdo is a list of 1 (set) 0 (clr) - (leave) ! toggle\n");
rshow(i," for example 'listdo thingy 10--1-1' which changes the entry for thingy\n");
rshow(i," and if thingy isnt there, adds them.\n");
return 0;
}

help7(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
    rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                            help 7  - Your mindscape\n");
rshow(i,"\n");
rshow(i,"   edit                - Puts you into mindscape edit mode.\n");
rshow(i,"   mindstate           - Tells you the status of your mindscape.\n");
rshow(i,"   mindseye            - Looks into your mindscape.\n");
rshow(i,"   mindopen [tog]      - Opens your mindscape to all.\n");
rshow(i,"   mindlock [tog]      - Locks your mindscape from all.\n");
rshow(i,"   eject <who|* [who]> - Throws who from your mindscape (or all but who).\n");
return 0;
}

help8(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                           help 8  - Other things (TM)\n");
rshow(i,"\n");
rshow(i,"   invites             - Shows who you have invites from.\n");
rshow(i,"   grabs               - Shows who you can grab.\n");
rshow(i,"   grab <who>          - Grabs who to your location (if you can).\n");
if (users[i].flags&0x200) {
rshow(i,"   leavein [time] <who>- Throws you/who out in approx. time minutes.\n");
} else {
rshow(i,"   leavein [time]      - Throws you out in approx. time minutes.\n");
}
if ((users[i].flags&1)!=0)
rshow(i,"   ttyheight [siz]     - Sets/Checks your terminal height.\n");
rshow(i,"   ttywidth [siz]      - Sets/Checks your terminal width.\n");
rshow(i,"   notice <stuff>      - Sets your notice (63chars) to stuff.\n");
rshow(i,"   board [page]        - Reads the set of notices.\n");
if ((users[i].flags&1)!=0) {
rshow(i,"   mail [who [anon]]   - Show/Send items of mail (perhaps anon).\n");
rshow(i,"   read <msg>          - Reads mail message msg.\n");
rshow(i,"   reply <msg> [anon]  - Reply's to mail message msg.\n");
rshow(i,"   delete <msg>        - Removes mail message msg.\n");
rshow(i,"   object [who <msg>]  - Set/clear your objection to a non resident.\n");
rshow(i,"                         You MUST give a reason.\n");
}
if ((users[i].flags&0x10000000)!=0)
rshow(i,"   evict <who> <why>   - Evicts who and logs the eviction.\n");
if ((users[i].flags&0x00800000)!=0)
rshow(i,"   trace <who> <why>   - Traces who and logs the reason.\n");
if ((users[i].flags&0x00000400)!=0)
rshow(i,"   openhouse [fh][lwi] - Set door full/half open (from where).\n");
return 0;
}


help9(i)
int i;
{
rshow(i,"                         help 9  - Some information.\n");
rshow(i,"\n");
rshow(i,"                       Cheeseplant's House version 3.0\n");
rshow(i,"\n");
rshow(i,"    Welcome to cheeseplant's house version 3.0, for those who were not around\n");
rshow(i,"for version 1, you can skip to the next paragraph. Ive been asked why the need\n");
rshow(i,"for the change.. Well, the original version started small and became large, so\n");
rshow(i,"had become inefficient and cumbersome, This version streamlines and tidies up\n");
rshow(i,"things, I'm aware it does have some disadvantages over the old version, but the\n");
rshow(i,"advantages far outweigh these.\n");
rshow(i,"    The house is a place where you can talk and interact (spot the buzz word)\n");
rshow(i,"with other people. Most people get on fine.. but i'm afraid that the world isnt\n");
rshow(i,"perfect and there are the odd troublemakers... if you have any complaints,\n");
rshow(i,"tell me and i'll try to deal with them.\n");
rshow(i,"    Although i hate hierarchy's, they seem to be in integral part of life so\n");
rshow(i,"there is a sort of 'level' system within the house... When people first login\n");
rshow(i,"they are non-residents, This means that when they log out they are reset, and\n");
rshow(i,"nothing is saved to disk... Now, people who use the program regularly can\n");
rshow(i,"become residents, which allows them to set a password and keep their identity\n");
rshow(i,"to themselves. This however is all an initial resident gets to save, anything\n");
rshow(i,"else they set up is lost when they log out.\n");
rshow(i,"                             Continued on help 10 ->\n");
return 0;
}

help10(i)
int i;
{
rshow(i,"                         help 10 - More information.\n");
rshow(i,"\n");
rshow(i,"    As much as i'd like to let everyone save everything, disk space is a very\n");
rshow(i,"rare commodity so i'm afraid i cant, the house will allow people who use the\n");
rshow(i,"program more than a certain length of time to save their title, and their\n");
rshow(i,"description, the precise length of time hasnt been decided on yet, and i wont\n");
rshow(i,"disclose it when it is. But that should be automatic, the main things that\n");
rshow(i,"people WANT to save.. but that eat up my disk space at an incredible rate are\n");
rshow(i,"lists and mindscapes... I dont know how i'll determine who can save their list\n");
rshow(i,"yet... i'll have to just assess individual cases.. But mindscapes I can make\n");
rshow(i,"sort of a quick set of rules about, people who can download scripts from where\n");
rshow(i,"they log in to set up a mindscape I will *PRAY* that you tell me so that you\n");
rshow(i,"do it when you log in and use only 1k or so of your own disk space, instead\n");
rshow(i,"of using 1k EACH of mine.. Others, if i think its good enough to save then i'm\n");
rshow(i,"likely to (but only to longish-standing residents.. sorry) I may sound mean and\n");
rshow(i,"horrid but saving ANYTHING is a privelidge.. as most people appreciate.. its\n");
rshow(i,"those who think its a god-given right i'm directing this at.\n");
rshow(i,"    Enough of the mean and nasty side of me, the house was written for pleasure\n");
rshow(i,"so i hope you get that out of it... If you're worried about something, talk to\n");
rshow(i,"me, and if i'm not being talkative... wait a day and try again... we all have\n");
rshow(i,"our bad days...\n");
rshow(i,"                  Yours, Cheeseplant (aka. Daniel Stephens)\n");
return 0;
}

help11(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                        help 11  - Obtaining information.\n");
rshow(i,"\n");
rshow(i,"   hosts <page>        - Shows which hosts people are in from.\n");
rshow(i,"   nums <page>         - Shows what inet numbers people are in from.\n");
rshow(i,"   editors             - Shows who's using the editors.\n");
rshow(i," * udata <page>        - Shows data on who's logged in.\n");
rshow(i,"   emails <page>       - Shows you people's email addresses.\n");
rshow(i,"   comments <page>     - Shows what comments people have.\n");
rshow(i," * stat <user> <-d>    - Shows user's status (optn. No descn.).\n");
rshow(i,"   stats <page>        - Shows a lists of users statuses.\n");
rshow(i,"   ttys <page>         - Shows a list of users identified ttys\n");
rshow(i,"   sendchar <who> <val>- Sends a list of ctrl chars to who.\n");
rshow(i,"   mailstat <who>      - Shows how much mail who has.\n");
rshow(i,"   mailfind <who>      - Shows where who has sent their mail.\n");
rshow(i,"   maillimit <who> [d] - Shows who's mail limit and adds d to it.\n");
rshow(i,"   stat [who] [?]      - Shows information on who (or help).\n");
rshow(i,"   locate [who]        - Attempts to find out who's hostname.\n");
rshow(i,"   users               - Dumps a list of ALL usernames\n");
return 0;
}

help12(i)
int i;
{
/*-------012345678901234567890123456789012345678||123456789012345678901234567*/
rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                         help 12  - Administrating Users.\n");
rshow(i,"\n");
rshow(i,"   allow <who> <what>  - Allow who to save what.   [use tit pro lis sna]\n");
rshow(i,"   deny <who> <what>   - Deny who from saving what.\n");
rshow(i,"   bestow <who> <power>- Bestow a power on who.\n");
rshow(i,"   ban <who> <power>   - Remove a power from who.\n");
rshow(i,"   gag <who>           - Prevents who from talking.\n");
rshow(i,"   ungag <who>         - Removes who's gag.\n");
rshow(i,"   throat <who> [time] - Does horrid things to who.\n");
rshow(i,"   email <who> <what>  - Sets who's email address to what.\n");
rshow(i,"   comment <who> <what>- Sets who's comment to what.\n");
rshow(i," * banish <name>       - Banish name from logging in.\n");
rshow(i," * unbanish <name>     - Unbanish name.\n");
rshow(i,"   rename <old> <new>  - Changes old's name.\n");
rshow(i,"   eradicate <who|#num>- Remove all references to who.\n");
rshow(i,"   chpass <who> <pass> - Changes who's password.\n");
rshow(i,"   hostinfo [page]     - Displays the relevant host page.\n");
rshow(i,"   hostadd <inetaddr>  - Adds a host to the list.\n");
rshow(i,"   hostdo <crbD?> <num> - Edit status of a host on the list.\n");
return 0;
}

help13(i)
int i;
{
rshow(i,"                              Cheeseplant's House.\n");
rshow(i,"                        help 13  - More of MY commands.\n");
rshow(i,"\n");
rshow(i,"   beep <user> <mesg>  - Beeps a user.\n");
rshow(i,"   goto <user>         - Moves you to user.\n");
rshow(i,"   move <user> <name>  - Moves user to room name.\n");
rshow(i,"   jump <name>         - Moves you to room name.\n");
rshow(i,"   arrive <name>       - Moves you to room name silently.\n");
rshow(i,"   maxlass [num]       - Set/query maximum current LASS number.\n");
rshow(i,"   info                - Gets information on disk status.\n");
rshow(i,"   loadrooms           - Loads up a room file.\n");
rshow(i,"   saverooms           - Saves a room file.\n");
rshow(i," . roominfo            - Gives you info on your room.\n");
rshow(i,"   roomedit            - Edits your current room.\n");
rshow(i,"   roomexit [Name]     - Sets the current room's exit name.\n");
rshow(i,"   roomlist            - Dumps out all the rooms.\n");
rshow(i,"   loadexits           - Loads up a exit file.\n");
rshow(i,"   saveexits           - Saves a exit file.\n");
rshow(i,"   hash <n>            - Prints up hash table n.\n");
rshow(i,"   cleanmail           - Cleans up the mailboxes.\n");
rshow(i,"   delnotice <who>     - Removes who's notice.\n");
return 0;
}

rules(i)
int i;
{
rshow(i,"                   Cheeseplant's house. Guidelines for use.\n");
rshow(i,"       Ok, so its finally come to this. a page of guidelines for users.\n");
rshow(i,"1: Swearing/Offensive behavior- This is just NOT TOLERATED. If you feel a need\n");
rshow(i,"   to swear.. Do it in real life in a locked room, or do it in your mindscape\n");
rshow(i,"   away from everyone else. Excessive offensive behaviour can and will result\n");
rshow(i,"   in site bans, and also tends to result in the closing of the house to\n");
rshow(i,"   non-residents.\n");
rshow(i,"2: Privacy- I respect privacy and the commands on the house reflect this...\n");
rshow(i,"3: Piping data in- I'm sick to death of people piping in screens of says &\n   shouts and the like, It is annoying as hell and a complete waste of time\n");
rshow(i,"   and network resources, If it continues i'll have to start imposing stupid\n   time restrictions on frequency of says which will be a great pity.\n");
rshow(i,"4: Mail: Anyone found abusing anonymous mail will find themselves losing\n   residency.\n");
rshow(i,"          Any comments/views/ideas - Mail cheesey@dcs.warwick.ac.uk\n");
return 0;
}
