#include  <fcntl.h>                              
#include  <stdio.h>                              
#include  <stdlib.h>                             
#include  <string.h>                             
#include  <sys/types.h>                          
#include  <sys/wait.h>                           
#include  <sys/stat.h>                           
#include  <termios.h>                            
#include  <unistd.h>                             
#define MAX 100000
#define INPUT_END 1                         
#define OUTPUT_END 0      



#include <errno.h>

#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
  
#define SA struct sockaddr 
char command[10000]={'\0'};
char newcmd[10000]={'\0'};
extern int errno ;
char ConFile[500] ;
void removeSpaces(char *str) 
{ 
    int count = 0; 
	int i=0;
    while(str[i]!='\0') 
		i++;
    if(str[i-1]==' ')
		str[i-1] = '\0'; 
	if(str[i-1]=='\n')
		str[i-1]='\0';
	} 

char clr(char *c,int len)
{
	int i=0;
	for(int i=0;i<len;i++)
		*(c+i)='\0';
}

int parse(char *line,char **argv)
{
	int i=0;
	int j=0;
	while (*line != '\0') 
	{   
		if(*line=='\n')
		{	
			*line++='\0';
			return i;
		}
        while (*line == ' ' || *line == '\t')
		{
            *line++ = '\0';
			if(*line=='\n')
			{	
				*line++='\0';
				return i;
			}
		}
		*argv++= line;
		i++;
        while (*line != '\0' && *line != ' ' && *line != '\t' )
		{
            line++;
			if(*line=='\n')
			{	
				*line++='\0';
				return i;
			}
		}
    }
    *argv = '\0';  
	return i;
}


int chkpip(char *line,char *buff)
{
	char tes[10000];
	strcpy(tes,line);
	char *test[1000];
	int count=parse(tes,test);
	if(count<=0)
	{
		printf("parse error error");
		return 4;
	}
	if(strcmp(test[0],"cd")==0)
	{
		char  *gdir;
		char  *dir;
		char  *to;
		char buf[100];
		if(!test[1])
		{
			chdir("/");
			sprintf(buff,"changed directory to / \n");
			return 4;
		}
		if(test[1][0]=='/' || test[1][0]=='~')
		{
			
			if(chdir(test[1])!=0)
			{
				int errnum = errno;
				sprintf(buff, "Error no %d : %s\n",errnum, strerror( errnum ));

			}
			else
				printf(buff,"changed directory to%s \n",test[1]);
			return 4;
		}
		gdir = getcwd(buf, sizeof(buf));
		dir = strcat(gdir, "/");
		to = strcat(dir, test[1]);
		if(chdir(to)!=0)
		{
			int errnum = errno;
			sprintf(buff, "Error no %d : %s\n",errnum, strerror( errnum ));
		}
		else
			sprintf(buff,"changed directory to%s \n",to);
		return 4;
	}
	return 0;
}

int findcmd(char **cmd,char **ar) 
{
	char *dup = strdup(getenv("PATH"));
	char *s = dup;
	char *p = NULL;
	do {
		p = strchr(s, ':');
		if (p != NULL) {
			p[0] = 0;
		}
		char d[1000];
		// printf(" $PATH: %s\n", s);
		strcpy(d,s);
		strcat(d,"/");
		strcat(d,cmd[0]);
		// printf("chcecking in $PATH: %s\n", d);
		FILE *file;
		if( access( d, F_OK ) != -1 ) 
		{
			// printf("found :%s\n ",d);
			strcat(newcmd,d);
			int j=0;
			for(j=1;*(cmd+j);j++)
			{
				;
			}
			if(j>1)
				strcat(newcmd," ");
			int i=1;
			for(i=1;*(cmd+i);i++)
			{
				strcat(newcmd,*(cmd+i));
				if(i!=j-1)
					strcat(newcmd," ");
			}
			int count=parse(newcmd,ar);
			return 1;
			// file exists
		} else {
			// file doesn't exist
		}
		s = p + 1;
	} while (p != NULL);

	free(dup);
	return -1;
}


int execfunc(char **argv,char **envp)
{
	if(execve(*argv, argv,envp)<0)
	{

		fprintf(stderr, "Error\n");
		exit(0);
	}
}
int chld(char *cmd,char **envp)
{
	
	if(strchr(cmd,'.')!=NULL)
	{
		char  *oargv[10000]={'\0'};
		int count=parse(cmd,oargv);
		execfunc(oargv,envp);
	}
	else
	{
		char  *oargv[10000]={'\0'};
		int count=parse(cmd,oargv);
		char *argv[1000];
		int check=findcmd(oargv,argv);
		if(check==-1)
		{
			printf("command not found\n");
			exit(0);
		}
		else
		{

			execfunc(argv,envp);
		}
	}
}
   
int chk(char *cmd,char **envp)
{
	char  *argv[10000]={'\0'};
	int count=parse(cmd,argv);
	int i=0;
	if(strcmp(argv[0],"quit")==0||strcmp(argv[0],"Quit")==0||strcmp(argv[0],"Exit")==0||strcmp(argv[0],"EXIT")==0||strcmp(argv[0],"exit")==0||strcmp(argv[0],"QUIT")==0)
	{
		printf("exiting\n");
		exit(0);
		return 1;
	}
}


int exe(char *cmd,char **envp)
{
	char arg[1000];
	strcpy(arg,cmd);
	if(chk(cmd,envp)==1)
		return 0;
	int ret;
	if((ret=fork())==0)
	{

		chld(arg,envp);
		// fgbg(arg,envp);
		exit(0);
	}

	return ret;
}
          
int getioutput(char *command,char *buff,char **envp,char *x)
{
	// printf("get output executed with command [%s] and buff [%s]\n",command,x);
	int y=open("a.txt",O_RDWR|O_CREAT);
	int i=0;
	for(i=0;x[i];i++)
		;
	write(y,x,i);
	close(y);
	bzero(buff,MAX);
	pid_t pid1;              
    int fd[2];                      
                      
    pipe(fd);                           
    pid1 = fork();                   
                                              
    if(pid1==0)                              
    {                                            
            close(fd[OUTPUT_END]);         
            dup2(fd[INPUT_END], STDOUT_FILENO);  
            close(fd[INPUT_END]);               
            strcat(command,"<a.txt");
			system(command);
			exit(0);
    }                                  
    else                            
    {   
        close(fd[INPUT_END]);
		read(fd[OUTPUT_END],buff,MAX);

	
        close(fd[OUTPUT_END]);                  
        waitpid(-1, NULL, 0);                 
    }           
}


int getoutput(char *command,char *buff,char **envp)
{
	
	int filedes[2];
	if (pipe(filedes) == -1)
	{
	  perror("pipe");
	  exit(1);
	}
	pid_t pid = fork();
	if (pid == -1) {
	  perror("fork");
	  exit(1);
	} else if (pid == 0) {
	  dup2(filedes[1],2);
	  while ((dup2(filedes[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
	  close(filedes[1]);
	  close(filedes[0]);
	  exe(command,envp);
	  //perror("execl");
	  _exit(1);
	}
	close(filedes[1]);
	char buffer[MAX];
	int i=0;
	while (1) 
	{
		char bu[MAX];
		bzero(bu,MAX);
		ssize_t count = read(filedes[0], bu, MAX);
		if (count == -1) 
		{
			if (errno == EINTR) 
			{
			  continue;
			} 
			else 
			{
			  perror("read");
			  exit(1);
			}
		} 
		else 
		{
			if (count == 0) 
			{
				break;
			}
			else
			{
				strcat(buff,bu);
				// printf("%d: %s \n",i,bu);
			}
		}
		i++;
	}
	close(filedes[0]);
	waitpid(pid,NULL,WUNTRACED);
	// printf("output:%s\n",buff);
}


int term(char *command,char *buff,char **envp)
{
		// printf("\n command: %s\n",command);
		int set=chkpip(command,buff);

		if(set==0)
		{
			if(strchr(command,'|')!=NULL)
			{
				// printf("command recieved:%s\n",command);
				char cmd[100000]={'\0'};
				char res[1000000]={'\0'};
				int i=0;
				for(i=0;command[i]!='|';i++)
				{
					cmd[i]=command[i];
				}
				char rem[100000]={'\0'};
				i++;
				for(int j=0;command[i];i++)
				{
					rem[j]=command[i];
					j++;
				}
				if(strncmp(cmd,"output:",7)==0)
				{
					//output from previous
					char *y=strchr(cmd,':');
					char x[100000];
					*y++;
					for(int i=0;*(y+i);i++)
						x[i]=*(y+i);
					bzero(cmd,100000);
					char rm[100000]={'\0'};
					strcpy(rm,rem);
					bzero(rem,100000);
					if(strchr(rm,'|')==NULL)
					{
						int i=0;
						for(i=0;rm[i];i++)
						{
							cmd[i]=rm[i];
						}
						removeSpaces(cmd);
						// printf("1 cmd: [%s]\n stdin[%s]\n",cmd,x);
						getioutput(cmd,buff,envp,x);
						// sprintf(buff,"hello");
						// printf("result [%s]\n",buff);
					}
				}
			}
			else
			{
				// printf("4:command:[%s]\n ",command);
				getoutput(command,buff,envp);
			}
			// execute(envp);
		}
		else
		{
			strcat(buff,"\0");
		}
}
int func(int sockfd,char **env) 
{ 
    char buff[MAX]; 
    int n; 
    // infinite loop for chat 
        bzero(buff, MAX); 
  
        // read the message from client and copy it in buffer 
		// printf("read\n");
        int count=read(sockfd, buff, sizeof(buff)); 
		int l=0;
		while(count==0)
		{
			l++;
			sleep(0.5);
			count=read(sockfd, buff, sizeof(buff)); 
			if(l==1000000)
				exit(0);
		}
		      // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            exit(0); 
        } 
		// printf("read %d  charachters\n",count);
        // print buffer which contains the client contents 
        printf("From client: %s\n",buff); 
        // bzero(buff, MAX); 
        n = 0; 
		char cmd[MAX];
		bzero(cmd,MAX);
		strcpy(cmd,buff);
		bzero(buff,MAX);
		// printf("command: [%s]\n",cmd);
		term(cmd,buff,env);
		strcat(buff,"\0");
		// puts(buff);
        // and send that buffer to client 
		// printf("write\n");
		int i=0;
		for(i=0;buff[i];i++)
			;
        write(sockfd, buff, i); 
  
  
} 
  
// Driver function 
int main(int argc,char *arg[],char *env[]) 
{ 
	
	if(argc!=3)
    {
        printf("Please enter node name as cmd argument:\n");
        exit(0);
    }
	strcpy(ConFile,arg[1]);
    FILE *fp = fopen(ConFile,"r");
    if(fp==NULL)
    {
        perror("File opening Error : ");
        exit(0);
    }

    int port;
    char name[20];
    int i=0;
    int flag=0;
    while(!feof(fp))
    {   
        memset(name,0,20);
        fscanf(fp,"%s %d\n",name,&port);    
        if(strcmp(name,arg[2])==0)
        {
            flag=1;
            break;
        }
    }
    fclose(fp);
    if(!flag)
    {
        printf("No such node found please check config file ...\n");
		exit(0);
    }

	//--------------------Sever connection ---------------------//

    int sockfd, connfd, len; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and verification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded at port %d ..\n",port); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
	for(;;)
	{
		len = sizeof(cli); 
	  
		// Accept the data packet from client and verification 
		connfd = accept(sockfd, (SA*)&cli, &len); 
		if (connfd < 0) { 
			printf("server acccept failed...\n"); 
			exit(0); 
		} 
		else
		{
			// int ret=vfork();
			// if(ret==0)
			// {
				func(connfd,env); 		
				// exit(0);
			// }
		}
		printf("supplied ans\n");
	}
    // Function for chatting between client and server 
  
    // After chatting close the socket 
    close(sockfd); 
} 
