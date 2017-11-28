#include "getword.h"
int aFlag,lFlag,rFlag;
int getword (char *s)
{
/*declare variable io to receive getchar(), count as the length of word*/
int io;
int count= 0;
char *ss=s;
int Flag=0;

/*ignore the leading blanks*/
while ((io=getchar())==' ');
/*if EOF return -1*/

if(io == EOF){
  *ss = '\0';
  return -1;
  }

/*if newline return 0*/

if(io =='\n')
  {
  *ss = '\0';
  return count;
  }
/*meta characters are treated as a "word".*/
if(io =='<'||io=='>'||io=='|'||io=='&')
  {
  *ss++=io;
  *ss = '\0';
  return 1;
  }
/* treat for the specific character $ */
if(io == '$')
  {Flag=1;} 


/*the leading special case are done,the while loop will continue to place characters into array until a word  is '\n' , EOF or meta character is reached.
*/

while(1)
{
if(io==' ')
  {
   *ss = '\0';
   return count;
  }
/* if start with slash, treat the character as normal character*/
while(io == '\\')
  {
   io=getchar();
    if(io=='\n')
   {io = ungetc(io,stdin);
    *ss = '\0';
    return count;
   }

    if(io=='&')
    {
    *ss++ = io;
    count++;
    io=getchar();
    if (io=='\n')
    {aFlag=1;
     io = ungetc(io,stdin);
    *ss = '\0';
    return count;
    }
    if(io==' ')
    {
     *ss = '\0';
     return count;
    }
    else {*ss++ = io;
      count++;
      io=getchar();}
   }


else{
   if(io=='>'){rFlag=1;}
   if(io=='<'){lFlag=1;}
   *ss++ = io;
   count++;
   io=getchar();
   if(io==' ')
    {
     *ss = '\0';
     return count;
    }
  }

   }


if(io=='\n'||io=='<'||io=='>'||io=='|'||io=='&'||io==EOF)
  {io = ungetc(io,stdin);
  *ss = '\0';
  return count;
  }

*ss++ = io;
if(Flag==1)
{count--;}
else{
count++;
}
/*handle the case when the word is longer than Storage -1*/

if (count == 254||count==-254)
   {
   *ss = '\0';
   return count;
   }
 io=getchar();
}

}
