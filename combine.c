#include<stdio.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include <errno.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <fcntl.h>
#include <arpa/inet.h> 
		
#define MAX 100000 
#define INPUT_END 1                         
#define OUTPUT_END 0     
#define SA struct sockaddr 
int ret;
char newcmd[10000]={'\0'};
char current[100];
extern int errno ;
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
	char file[100]={'\0'};
	strcat(file,current);
	strcat(file,"x.txt");
	// printf("23hello \n");
	// printf("get output executed with command [%s] and buff [%s]\n",command,x);
	int y=open(file,O_RDWR|O_CREAT,777);
	int i=0;
	for(i=0;x[i];i++)
		;
	// printf("hello4\n");
	write(y,x,i);
	close(y);
	bzero(buff,MAX);
	pid_t pid1;              
    int fd[2];                      
                      
    pipe(fd);
// printf("21hello\n");	
    pid1 = fork();                   
    // printf("%d process \n",pid1);                          
    if(pid1==0)                              
    {                         
            close(fd[OUTPUT_END]);
			// printf("%s\n",command);
            dup2(fd[INPUT_END], STDOUT_FILENO);  
            dup2(fd[INPUT_END], STDERR_FILENO);  
            close(fd[INPUT_END]); 
			char cmmd[100];
			strcat(cmmd,"<");
			strcat(cmmd,file);
			
            strcat(command,cmmd);
			system(command);
			exit(0);
    }                                  
    else                            
    {   
		// printf("hello\n");
        close(fd[INPUT_END]);
		read(fd[OUTPUT_END],buff,MAX);

		// printf("oi buff%s\n",buff);
        close(fd[OUTPUT_END]);     
		
        waitpid(-1, NULL, 0);                 
		remove(file);
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
	  // if(strcmp(command,"cp")==0||strcmp(command,"rmdir")==0||strcmp(command,"mv")==0
	  system(command);
	  // exe(command,envp);
	  //perror("execl");
	  _exit(1);
	}
	close(filedes[1]);
	char buffer[MAX];
	int i=0;
	int su=0;
	while (1) 
	{
		char bu[MAX];
		bzero(bu,MAX);
		ssize_t count = read(filedes[0], bu, MAX);
		su=su+count;
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
	if(su==0)
	{
		// printf("successfull\n");
		strcat(buff,"successfull\n");
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
				// i++;
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
		// while(count==0)
		// {
			// l++;
			// sleep(0.5);
			// count=read(sockfd, buff, sizeof(buff)); 
			// if(l==1000000)
				// exit(0);
		// }
		      // if msg contains "Exit" then server exit and chat ended. 
        if (strncmp("exit", buff, 4) == 0) { 
            printf("Server Exit...\n"); 
            exit(0); 
        } 
		// printf("read %d  charachters\n",count);
        // print buffer which contains the client contents 
        // printf("From client: %s\n",buff); 
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
 int server(char *Confile,char *node,char **env)
 {

    FILE *fp = fopen(Confile,"r");
    if(fp==NULL)
    {
        perror("File opening Error : ");
        exit(0);
    }

    int port;
    char name[20];
    char ip[20];
    int i=0;
    int flag=0;
    while(!feof(fp))
    {   
        memset(name,0,20);
        memset(ip,0,20);
        fscanf(fp,"%s %s %d\n",name,ip,&port);    
        if(strcmp(name,node)==0)
        {
            flag=1;
            break;
        }
    }
    fclose(fp);
    if(!flag)
    {
        printf("No such node found please check config file ...\n");
		kill(ret,SIGINT);
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
    // else
        // printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port); 
  
    // Binding newly created socket to given IP and verification 
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) { 
        printf("socket bind failed...\n"); 
		kill(ret,SIGINT);
        exit(0); 
    } 
    // else
        // printf("Socket successfully binded at port %d ..\n",port); 
  
    // Now server is ready to listen and verification 
    if ((listen(sockfd, 5)) != 0) { 
        printf("Listen failed...\n"); 
		kill(ret,SIGINT);
		exit(0); 
    } 
    // else
        // printf("Server listening..\n"); 
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
		// printf("supplied ans\n");
	}
	// printf("client disconnected");
    // Function for chatting between client and server 
  
    // After chatting close the socket 
    close(sockfd); 
 }
 int funcc(char *buff,int sockfd,char *buf) 
{ 
    int n; 
    //for (;;) {
		strcat(buff,"\0");
		for(n=0;buff[n];n++)
			;
		// printf("write %d characters\n",n);
		strcat(buff,"\n");
        write(sockfd, buff, n); 
		if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            return 1;
        } 
        bzero(buf, MAX);
		// printf("read\n");
        read(sockfd, buf, MAX); 
        //printf("From Server : %s", buf); 
        return 0;
    //} 
} 
struct node 
{
    int port;
	char ip[20];
	
    char name[20];
    struct node *next;
};
struct node* ReturnPort(struct node *temp,char name1[20])
{
	// printf("tempport %s %d\n",	temp->ip,temp->port);
    int port=-1;
	struct node* p=(struct node *)malloc(sizeof(struct node));
    while(temp!=NULL)
    {
		// printf("checking [%s] [%s]\n",temp->name,name1);
        if(strcmp(temp->name,name1)==0)
        {
			// printf("set\n");
            p->port = temp->port;
            strcpy(p->ip , temp->ip);
            strcpy(p->name , temp->name);
			port=1;
            break;
        }
        else
        {
        temp = temp->next;
        }
    }
	if(port!=1)
	{
		// printf("not set\n");
		struct node* te=NULL;
		return te;
	}
	// printf("%s returning %s %d \n",p->name,p->ip,p->port); 
    return p;
}

struct node * AddData(struct node* root,int port,char name[20],char ip[20])
{
    struct node *n1 = (struct node *)malloc(sizeof(struct node));

    strcpy(n1->name,name);
    n1->port = port;
    strcpy(n1->ip,ip);
    n1->next = NULL;

    struct node *temp = root;

    if(temp==NULL)
    {
        temp = n1;
        return temp;
    }   
    else
    {
        while(temp->next!=NULL)
        {
            temp=temp->next;
        }
        temp->next = n1;
     
        return root;
    }
}

int client(char *buff,int port,char ip[20],char * re)
{
    
    //--------------------client connection ---------------------//

    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        return 3; 
    } 
    // else
        // printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(ip); 
    servaddr.sin_port = htons(port); 
	// printf("port %d, ip [%s]\n",port,ip);
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        return 2; 
    } 
    // else
        // printf("connected to the server..\n"); 

    // function for chat 
    int ret = funcc(buff,sockfd,re); 
  
    // close the socket 
    close(sockfd); 
	// printf("ret %d %s\n",ret,re);
    return ret;
} 
struct node * CreatePortFile(char *addr)
{
	FILE *fp = fopen(addr,"r");
	if(fp==NULL)
	{
		perror("File opening Error : ");
		exit(0);
	}
    struct node *root = NULL;
    int port;
    char name[20];
    char ip[20];
    int i=0;
    int flag=0;
    int j=0;
    while(!feof(fp))
    {   
        memset(name,0,20);
        memset(ip,0,20);
        port=0;
        fscanf(fp,"%s %s %d\n",name,ip,&port);  
        if(port!=0)
        {
            root=AddData(root,port,name,ip);
        }
        // printf("***********\n");
    }   
    fclose(fp);
    return root;
}

int sparse(char *command,char **argv,char x)
{
	while(*command!='\0')
	{
		*argv++=command;
		while(*command!=x&&*command!='\n'&&*command!='\0')
			*command++;
		if(*(command-1)==' ')
			*(command-1)='\0';
		*command++='\0';
		if(*command==x||*command=='\n'||*command==' ')
			*command++='\0';
	}
}
int cli(char *pfile,char *po) 
{

	char re[MAX];
    struct node *root  = CreatePortFile(pfile);
	// printf("root port %s %d",root->ip,root->port);
    int res;
	struct node *p;
    while(1)
    {
		bzero(re,MAX);
        char buff[MAX];
        bzero(buff, sizeof(buff)); 
        printf("Enter the string : "); 
        fgets(buff,sizeof(buff),stdin);
        char *c;
        if(strncmp(buff,"quit",4)==0||strncmp(buff,"Quit",4)==0||strncmp(buff,"Exit",4)==0||strncmp(buff,"EXIT",4)==0||strncmp(buff,"exit",4)==0||strncmp(buff,"QUIT",4)==0)
        {
			p = ReturnPort(root,po);
			// printf("p port %s %d\n",p->ip,p->port);
			// if(p==NULL)
			// {
				// printf("No such node active...\n");
			// }
			// else
			// {
			   res = client(buff,p->port,p->ip,re);
			   // if(res!=0)
			   // {
				   // printf("error in server4\n");
				   // cli(pfile,po);
			   // }
			// }
            printf("exiting\n");
            exit(0);
            return 1;
        }
		if(strchr(buff,'|')!=NULL)
		{
			// printf("splitting\n");
			char *bu[MAX];
			sparse(buff,bu,'|');
			

			// for(int i=0;bu[i];i++)
			// {
				// printf("bu[%d]:%s\n",i,bu[i]);
			// }
						
			
			
			for(int i=0;bu[i];i++)
			{
				char command[1000]={'\0'};
				char b[MAX]={'\0'};
				
				int k=0;
				int j=0;
				while(bu[i][k]!=' ')
					k++;
				for(k=0;bu[i][k];k++)
						b[j++]=bu[i][k];
		
				c=strchr(b,'>');
				bzero(command,1000);
				if(c!=NULL)
				{
					if(*(c-1)==' ')
						*(c-1)='\0';
					*c++='\0';

					for(j=0;*(c+j);j++)
					{
						command[j]=*(c+j);
						*(c+j)='\0';
					}
					command[j]='\0';
					p = ReturnPort(root,b);
				}
				else
				{
					p = ReturnPort(root,po);
					strcpy(command,b);
				}

				char cm[100000]={'\0'};
				// printf("at %d re is [%s] command is [%s]\n",i,re,command);
				if(i!=0)
				{
					strcat(cm,"output:");
					strcat(cm,re);
					strcat(cm,"|");
				}
				
				strcat(cm,command);
				bzero(re,MAX);
				
				if(p==NULL)
				{
					printf("No such node active...(use pipe and node name together eg n1.ls|n2.wc)\n");
					cli(pfile,po);
				}
				else
				{
					// printf("sending comand to client %d [%s]\n",i,cm);
					res = client(cm,p->port,p->ip,re);
					// printf("recieved result fro	m %d after mani [%s] \n",i,re);
					if(res!=0)
					{
						printf("error in server\n");
						cli(pfile,po);
					}
				}
			}
		}
		else
		{
			c=strchr(buff,'>');
			if(c!=NULL)
			{
				if(*(c-1)==' ')
					*(c-1)='\0';
				*c++='\0';
				char command[1000];
				int i;
				for(i=0;*(c+i);i++)
				{
					command[i]=*(c+i);
					*(c+i)='\0';
				}
				command[i]='\0';
		
				p = ReturnPort(root,buff);
				if(p==NULL)
				{
					printf("No such node active...\n");
					cli(pfile,po);
				}
				else
				{
				   res = client(command,p->port,p->ip,re);
				  if(res!=0)
				  {
					  printf("error in server\n");
					  cli(pfile,po);
				  }
				}
			}
			else
			{
				// printf("for itself\n");
				p = ReturnPort(root,po);
				// printf("p port %s %d\n",p->ip,p->port);
				if(p==NULL)
				{
					printf("No such node active...\n");
					cli(pfile,po);
				}
				else
				{
				   res = client(buff,p->port,p->ip,re);
				   if(res!=0)
				   {
					   printf("error in server\n");
					   cli(pfile,po);
				   }
				}
			}
		}
		
		printf("%s\n",re);
		
	}
    
    
}
int main(int argc,char *argv[],char *env[])
{
	if( access( "config.file", F_OK ) == -1 )
	{		
		FILE *fp = fopen("config.file","w");
		fclose(fp);
	}

	if(argc==4)
	{
		FILE *fp = fopen("config.file","a");
		if(fp==NULL)
		{
			perror("File opening Error : ");
			exit(0);
		}
		int port=atoi(argv[3]);
		char ip[20];
		strcpy(ip,argv[2]);
		char name[20];
		strcpy(name,argv[1]);
		fprintf(fp,"%s %s %d\n",name,ip,port);  
		exit(0);
	}
	if(argc!=2&&argc!=4)
	{
		printf("please enter current node name as arguments\n");
		printf("to add a node use args as : name ip port for ex ./c n1 127.0.0.1 8080\n");
		printf("to start a node use args as : name for ex ./c n1 \n");
		exit(0);
	}
	strcpy(current,argv[1]);
	ret=fork();
	if(ret!=0)
	{
		// char re[MAX]={'\0'};
		// cli("config.file","n1");
		// client("ls",8080,re);
		// term("ls",re,env);
		// term("output:2121212 |wc ",re,env);
		// term("cd ..",re,env);
		// term("pwd",re,env);
		// printf("done\n");
		// strcat(re,"\0");
		// client("ls",8080,re);
		// puts(re);
		// int i=0;
		// while(re[i])
			// printf("%c",re[i++]);
			// wait(NULL);
			server("config.file",current,env);
			
	}
	else{
			cli("config.file",current);
			// server("config.file","n1",env);
		exit(0);
	}

	// else
	// {
		// server("config.file","n1",env);
	// }
	return 0;
}
