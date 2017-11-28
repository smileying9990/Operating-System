/*
Name:Yingxue Zhu
Professor: John Carroll
Class: CS570
Date:11/23/2016
Description: p2.c parses the command line to newargv using getword.c,
execute newargv, with the exeception of the built in command cd and environ
,handle mutiple pipelines and proper I/O redirect*/


#include "p2.h"

int aFlag,lFlag,rFlag;
int p[MAXITEM];
char *env;
char s[STORAGE];
char inArray[MAXITEM];
char outArray[MAXITEM];
char *newargv[MAXITEM];
struct stat sb;
struct stat sb1;

int x,c;
int pipefd[20][2];
int input_fd;
int output_fd;
int inputRedirect;
int isBackground=0;
int leftFlag =0;
int rightFlag = 0;
int doubleLeft = 0;
int doubleRight=0;
int noInput =0;
int noOutput =0;
int pipeFlag =0;
int metaFlag=0;
int envFlag=0;
int isDollor=0;
int fildes[2];

pid_t pid2;
pid_t child_pid[20];
int pid;
int num_of_pipes;
char *devNull;
void parse(char *s,char **ptrArray, char *io, char *out);
void execute(char **execArray, char *ioArray, char *outArray);
void pipeExec(char **ptrArray, char *ioArray, char *outArray);
void myhandler(int signum);
/*main function*/
main()
{
/*catch the SIGTERM signal, let p2 print terminated*/ 
 signal(SIGTERM, myhandler);
 for(;;)
 {
  printf("p2: ");
/*reset Flag to 0*/   
  isBackground=0;
  leftFlag =0;
  rightFlag = 0;
  doubleLeft = 0;
  doubleRight = 0;
  noInput =0;
  noOutput =0;
  pipeFlag =0;
  metaFlag=0;
  envFlag=0;
  isDollor=0;

/*reset the arrays to NULL*/
  memset( newargv, '\0', sizeof(newargv) );
  memset( inArray, '\0', sizeof(inArray) );
  memset( outArray, '\0', sizeof(outArray) );
  
 /*call parse function*/
  parse(s, newargv,inArray,outArray);
  lFlag=0;
  rFlag=0;
  aFlag=0;

  /*deal with the errors*/
 
 if(newargv[0] == NULL)
   {
    continue;
   }

  if(noInput ==1)
   {
    perror("You are trying to redirect input with out specifying a file.");
    continue;
   }
  if(noOutput ==1)
   {
    perror("You are trying to redirect output with out specifying a file.");
    continue;
   }

  if(doubleLeft ==1)
   {
    perror("You are assigning multiple files for input");
    continue;
   }
  if(doubleRight ==1)
   {
    perror("You are assigning multiple files for output");
    continue;
   }
  if(rightFlag == 1)
   {
     if (stat(outArray, &sb) == 0)
       {
        perror("File Exists.");
        continue;
       }
   }
 
   if(envFlag == 1)
   {
      perror("Undefined Variable");
      continue;}
  if(metaFlag == 1){
    perror("You are Assigning redirection to a metacharacter.");
    continue;
   }
 
/*****************buildin cd function****************/

  if(strcmp(newargv[0],"cd")==0){

/*a lone cd is treated like cd $HOME*/
    if(newargv[1] == NULL)
    {
       char *goHome = getenv("HOME");
       if((x = chdir(goHome))==0)

        {
        continue;
        }
        else
        {
        perror("Could not change to requested Directory.");
        continue;
        }
    }


    if(newargv[2] == NULL && newargv[1] != NULL)
    {


      if((x=chdir(newargv[1]))==0)
      {
      continue;
      }

      else
      {
      perror("Could not change to requested Directory.");
      continue;
      }
    }

    else
    {
     perror("cd is designed to accept 0 or 1 argument.");
     continue;
    }

  }
/*************buildin environ function***************/
    if(strcmp(newargv[0],"environ")==0){
	
   if(newargv[1] != NULL &&newargv[2] != NULL&&newargv[3] != NULL)
      {
      perror("Incorrect number of arguements");
      }
        
   if(newargv[1] != NULL && newargv[2] == NULL)
      {
      char *environment = getenv(newargv[1]);
      if(environment == NULL)
         {
         printf("\n");
         continue;
         }
                
      printf("%s\n", environment);
      }
	if(newargv[1] != NULL && newargv[2] != NULL&&newargv[3] == NULL)
      {
	  int q = setenv( newargv[1], newargv[2], 1);
      if (q == -1)
         {
         perror("Set environment failed");
         }
	  continue;	
	  }  
     if(newargv[1] == NULL)	 { 
	 
	    perror("no environment argument");
		
	   } 
	  continue;    	  	  	  
  }
/*start excute newargv*/
  if (pipeFlag == 1)
   {
   pipeExec(newargv, inArray, outArray);
   fflush(NULL);
   if (isBackground==1)
   {continue;}
   if(isDollor==1){break;}
   }
  else
   {
   execute(newargv, inArray, outArray); 
   fflush(NULL);	
   if (isBackground==1)
   {continue;}
   if(isDollor==1)
   {break;}
   }
 }

   killpg(getpid(),SIGTERM);
   printf("p2 terminated.\n");
   exit(0);
 
 }
 
 
/*Parse function parse the commandline to newargv[]*/
void parse(char *s,char **ptrArray, char *io, char *out)
{
int i=0;
int j=0;
num_of_pipes=0;
int c;
int x;


while((c=getchar())==' ');
  /* empty input */
if(c == '\n')
  {
   ptrArray[0]=NULL;
   return;
  }
if(c ==-1)
   {
   printf("p2 terminated.\n");
   exit(0);
   }
     
c = ungetc(c,stdin);
ptrArray[0] = s ;

for(;;)
{
c = getword(s);
if(c ==-1&&*s == '$')
   {
   ptrArray[j]=s;   
   j=j+1;   
   ptrArray[j]= NULL;
  /*a lone $ treated as termination*/
   isDollor=1;
   c = ungetc(c,stdin);
   return;}
if(c ==-1)
   {
   ptrArray[j]= NULL;
   return;}
if(c<=-2){
	c=abs(c);		
	env =getenv(s+1);
   if(env == NULL)
      {
      envFlag = 1;
      return;
      }
   else
      ptrArray[j] = env;
   }
if(*s == '$')
   {
   c=abs(c);   
   }
if(c ==0)
   {
   ptrArray[j]= NULL;
 /*check the last &,the aFlag is a global variable for \&*/ 
   j=j-1;
   if(strcmp(ptrArray[j],"&")== 0&&aFlag!=1){isBackground=1;ptrArray[j]=NULL;}
   return;
   }


if(c ==1)
   {
   
   if(*s == '|')
      {      
      ptrArray[j] = NULL;
      pipeFlag = 1;
      s=s+2;
      j=j+1;
      p[i++] = j;
	  num_of_pipes++;
      ptrArray[j]  =s;
	  
      continue;
      }

   if(*s == '<'&&lFlag!=1)
      {
      if(leftFlag ==1)
         {
         doubleLeft =1;
         }
	  
      leftFlag = 1;
      x = getword(io);
	  if (stat(io, &sb1) != 0)
	    {
		 perror("File not Exist.");
         return;
		
		 }
      continue;
   }

   if(*s == '>'&&rFlag!=1)
      {
	  
	  if(rightFlag ==1)
         {
         doubleRight =1;
         }
      rightFlag = 1;
	
      x = getword(out);
	  if(x ==0)
        {
         noOutput = 1;
         return;
        }
      if(x ==1)
         {
         if(out[0] =='&'||out[0] =='>')
            {
            metaFlag = 1;
            }
         }
	   if(x<=-2){
	   		
	   env =getenv(out+1);
       strcpy(outArray, env);
      	
      }
   	   
      continue;
      }

}

s = s  + c +1;
j++;
ptrArray[j] = s;

}
}
 
/*Execute function reacts to all executes except for pipe*/
void execute(char **execArray, char *ioArray, char *outArray)
{
/********************parent precess**************/	
devNull = (char *) malloc(10);
strcpy(devNull, "/dev/null");
fflush(stdout);
pid2 = fork();
int exc;

if(pid2 == -1)
   {
   perror("Cannot Fork");
   _exit(1);
   }
/********************child precess**************/

if(pid2 ==0)
   {
   if(isBackground==1){
/*redirect the child process stdin to dev/null*/
   inputRedirect = open(devNull, O_RDONLY);
   dup2(inputRedirect, STDIN_FILENO);
   close(inputRedirect);}
   if(rightFlag ==1)
      {
      output_fd = open(outArray,O_CREAT | O_RDWR, 0400 | 0200);
      dup2(output_fd, STDOUT_FILENO);
      rightFlag = 0;
      close(output_fd);
      }

   if(leftFlag ==1) 
      {
      if((input_fd = open(ioArray, O_RDONLY))<0)
	  {
		perror("Open Failed");
	    _exit(1);  
	  }
      dup2(input_fd, STDIN_FILENO);
      leftFlag = 0;
      close(input_fd);
      }
 
 /* if execvp success, will not print the following*/
   if ((exc=execvp(execArray[0],execArray))<0)
      {
      perror("Exec Failed");
	   _exit(1);
      }
   }

/*******************parent process***************/
/* backgroud do not wait the child*/

 if(isBackground==1)
   {printf("%s [%d]\n", newargv[0], pid2);
   fflush(stdout);
   return;}
/*parent wait for the child*/
if(exc<0){return;}
for(;;)
   {
    pid_t  pidw;
    pidw = wait(NULL);
    if (pidw == pid2)
     {
       break;
     }
 }

return;
}
/*catch SIGTERM signal*/
void myhandler(int signum){}

/*the pipeExec functions creat pipe communication and the 
second child is waited*/

void pipeExec(char **ptrArray, char *ioArray, char *outArray)
{
/*******************parent process******************/
int i;
int k,kk,kkk;
devNull = (char *) malloc(10);
strcpy(devNull, "/dev/null");

/*chd0    chd1   chd2   chd3   chd4
     pipe0   pipe1  pipe2  pipe3
     [0,1]   [2,3]  [4,5]  [6,7]*/

fflush(stdout);


for (i = 0; i <=num_of_pipes; i++) {
    if(i!=num_of_pipes)
	  pipe(pipefd[i]);

      if ((child_pid[i] = fork()) == 0) 
	  {
 /*******************first child******************/ 		  
        if (i == 0) {
		  if(leftFlag == 1)
             {
              input_fd = open(ioArray, O_RDONLY);
              dup2(input_fd, STDIN_FILENO);
              leftFlag = 0;
              close(input_fd);
             }
          
          dup2(pipefd[i][1], STDOUT_FILENO);
          close(pipefd[i][1]);
		  close(pipefd[i][0]);
		  
		   if (execvp(ptrArray[0], ptrArray) <0)
           {
            perror("execvp failed");
	        _exit(1);
           }
          
		} 
 /*******************last child******************/ 		
		else if (i == num_of_pipes) {
		  if(rightFlag ==1)
             {
              output_fd = open(outArray, O_CREAT | O_RDWR, 0400 | 0200);
              dup2(output_fd, STDOUT_FILENO);
              rightFlag = 0;
              close(output_fd);
             }
          
          dup2(pipefd[i - 1][0], STDIN_FILENO);
        //  close(pipefd[i - 1][0]);
		//  close(pipefd[i - 1][1]);
		 for(kkk=0;kkk<i;kkk++)
	     {
		 
	    close(pipefd[kkk][0]);
        close(pipefd[kkk][1]);		
        }
		  
		if (execvp(ptrArray[p[i-1]], ptrArray+p[i-1]) <0)
           {
            perror("execvp failed");
	         _exit(1);
           }
          
         } 
 /*******************middle child******************/ 		 
		 else {
          dup2(pipefd[i - 1][0],STDIN_FILENO); 
          dup2(pipefd[i][1],STDOUT_FILENO);
  		
		for(kk=0;kk<=i;kk++)
	     {
		 
	     close(pipefd[kk][0]);
         close(pipefd[kk][1]);		
         }
		  if (execvp(ptrArray[p[i-1]], ptrArray+p[i-1]) <0)
           {
            perror("execvp failed");
	         _exit(1);
           }          
        }
       }
/*******************parent process******************/	  
	  if(child_pid[i]  > 0){
	
		if(i== num_of_pipes){
		    
	    for(k=0;k<i;k++)
	     {
		 
	    close(pipefd[k][0]);
        close(pipefd[k][1]);	
       		
        }
		
		if(isBackground==1) 
			{printf("%s [%d]\n", newargv[0], child_pid[i]);
             fflush(stdout);
             return;}

		for(;;)
         {
          pid_t  pidw;
          pidw = wait(NULL);
          if (pidw == child_pid[i]) break;   
	     } 
		return;
	   } 
	   continue;
     }
    }  
    return;		 
  }

  
  
  
  
  


