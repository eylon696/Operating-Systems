#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>


void shell1();
void shell2();
void shell3();

#define MAX 1024


int main()
{
    char input[MAX];
    char c;
    while(1)
    {
        printf("Enter Shell1 to run linux commands without parameters\n");
        printf("Enter Shell2 to run linux commands with parameters\n");
        printf("Enter Shell3 to run commands to file editing\n");
        printf("Shell$** ");
        scanf("%s",input);
        scanf("%c",&c);
        if(strcmp(input,"Shell1")==0)
        {
            if(fork()==0)
                shell1();
            else
                wait();
        }
        else if(strcmp(input,"Shell2")==0)
        {
            if(fork()==0)
                shell2();
            else
                wait();
        }
        else if(strcmp(input,"Shell3")==0)
        {
            if(fork()==0)
                shell3();
            else
                wait();
        }
         else if(strcmp(input,"Exit")==0)
            break;
        else
            printf("Not Supported\n");
    }
return 0;
}
//Supports linux commands without arguments like: ls,ps,date etc
void shell1()
{
char input[MAX];
char path[]="/bin/";
while(1)
{
    printf("Shell1$**");	
    scanf("%s",input);
    if(strcmp(input,"Esc")==0)
	    {
	    printf("END\n");
	    exit(1);
	    }
    strcat(path,input);//updating the path with the wanted command
    if(fork()==0)
    {
	    execl(path,input,NULL);	//executing the wanted command
	    printf("ERROR ERROR ERROR\n");
	    exit(1);
    }		
    else
	    wait();
    strcpy(path,"/bin/");//reseting the path		
    }
}

//Supports linux commands with max 3 arguments and min 1 argument like: ls -l,man grep etc
void shell2()
{
char path[]="/bin/";
char **args;
char *token;
int count=0;
int i=0;
int j=0;
char input[MAX];
while(1)
{	
	printf("Shell2$** ");	
	fgets(input,sizeof(input),stdin);
    while(strcmp(input,"\n")==0||input[0]==' ')//scan again if the input is illegal
        {
            printf("illegal input! try again\n");
            printf("shell2$** ");
            fgets(input,sizeof(input),stdin);
        }
	if(strcmp(input,"Esc\n")==0)//close shell2
		exit(1);
	while(input[i]!='\0')//counting arguments
	{
		if(input[i]==' '&&input[i+1]!=' ')
			count++;
		i++;
	}
	i=0;
	j=0;
	token = strtok(input, "\n");//token has the entire sentence without the "\n"
	strcpy(input,token);//now input is updated to be without the "\n"
	args=(char**)malloc(sizeof(char*)*(count+2));
    if(args == NULL)                     
    {
        printf("Error! memory not allocated.");
        exit(0);
    }
	token=strtok(input," ");
    //getting the sentence word by word and allocating memory to each one
	while( token != NULL ) {
      	   args[j]=(char*)malloc(sizeof(char)*strlen(token)+1);
 			 if(args[j] == NULL)                     
   			 {
      		  	printf("Error! memory not allocated.");
        		exit(0);
    		 }
		   strcpy(args[j],token);
		   j++;
		   token=strtok(NULL," ");
  	 }
	args[j]=NULL;//filling the last word with null to use execv command
	strcat(path,args[0]);//updating the path with the wanted command	
	if(count>0&&count<4)//execute the command if it has 1-3 arguments
	{
		if(fork()==0)
		{
		    execv(path,args);
		    printf("ERROR\n");
		    exit(1);
		}
		    else
			    wait();
	 }
    	else
	    	printf("Not Supported\n");
    for(j=0;j<count+2;j++)//freeing the memory allocated
	    free(args[j]);
    free(args);//freeing the memory allocated
    count=0;
    i=0;
    j=0;
    strcpy(path,"/bin/");//reseting the path
    }
}
//Supports "copy" and "delete" commands 
void shell3()
{
char input[MAX];
char *token;
char**args;
int i=0;
int fd1,fd2;
int count=0;
int rd1;
char buff1[2];
    while(1)
    {
        printf("shell3$** ");
        fgets(input,sizeof(input),stdin);
        while(strcmp(input,"\n")==0||input[0]==' ')//scan again if the input is illegal
        {
            printf("illegal input! try again\n");
            printf("shell3$** ");
            fgets(input,sizeof(input),stdin);
        }
        if(strcmp(input,"Esc\n")==0)
		    exit(1);
        while(input[i]!='\0')//counting arguments
	     {
		    if(input[i]==' '&&input[i+1]!=' ')
		    	count++;
		    i++;
	     }
         i=0;
         token = strtok(input, "\n");
	     strcpy(input,token);//string without enter
         args=(char**)malloc(sizeof(char*)*(count+1));
         if(args == NULL)                     
            {
             printf("Error! memory not allocated.");
             exit(0);
            }
         token=strtok(input," ");
         //getting the sentence word by word and allocating memory to each one
	     while( token != NULL ) 
         {
      	     args[i]=(char*)malloc(sizeof(char)*strlen(token)+1);
 			 if(args[i] == NULL)                     
   			 {
      		  	printf("Error! memory not allocated.");
        		exit(0);
    		 }
		   strcpy(args[i],token);
		   i++;
		   token=strtok(NULL," ");
         }
         if((strcmp(args[0],"Copy")==0||strcmp(args[0],"Delete")==0)&&(count==1||count==2))//The input is llegal
         {
            if(strcmp(args[0],"Copy")==0)
            {
                if(count==1)
                {
                    fd1=open(args[1],O_RDONLY);
                    if(fd1<0)
                        exit(1);
                    rd1=read(fd1,buff1,1);
                    while(rd1!=0)//as long as we did not reach the end of the file
	            	{	
                        write(1,buff1,1);//printing to the screen the entire file content char by char	
                        rd1=read(fd1,buff1,1);  
	            	}
                    close(fd1);
                }
                else
                {
                    fd1=open(args[1],O_RDONLY);
                    if(fd1<0)
                        exit(1);
                    fd2=open(args[2],O_WRONLY | O_CREAT,0664);
                    if(fd2<0)
                        exit(1);
                    rd1=read(fd1,buff1,1);
                    while(rd1!=0)//as long as we did not reach the end of the file
	            	{	
                        write(fd2,buff1,1);	//printing to fd2 the entire file content char by char
                        rd1=read(fd1,buff1,1);  
	            	}
                    close(fd1);
                    close(fd2);
                }
            }
            else if(strcmp(args[0],"Delete")==0)
            {
                if(count==1)
                    remove(args[1]);
            }

         }
         else
            printf("Not Supported\n");
        for(i=0;i<count+1;i++)
	        free(args[i]);//freeing the memory allocated
        free(args);//freeing the memory allocated
        count =0;
        i=0;
    }
}
