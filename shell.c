#include<stdio.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include <errno.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
extern int errno ;
int pip[1000][2];
char dir[100];
char command[10000]={'\0'};
char newcmd[10000]={'\0'};
char envi[10000]={'\0'};
char en[10000]={'\0'};


//----------------variable for foreground background -----//
int currpid,turn=1,shellId,InBack=0;
int ProInBack;
int ArrProInBack[1000][2];
//----------------Variable for foreground background ends--//
void AttachSignal();
void RemoveBSignal();
//----------------functions for foreground background -----//
void KillAllPending()
{
	for(int i=0;i<ProInBack;i++)
	{
		kill(ArrProInBack[i][0]*(-1),SIGQUIT);
	}
}
void print()
{
	if(ProInBack==0)
	{
		printf("No Process in Background\n");
		return;
	}
	for(int i=0;i<ProInBack;i++)
	{
		printf("%d %d %d\n",i,ArrProInBack[i][0],ArrProInBack[i][1]);
	}
}
void BackProDied(int id)
{
	int k=-1;
	for(int i=0;i<ProInBack;i++)
	{
		if(ArrProInBack[i][0]==id)
		{
			k=i;
		}
	}
	if(k!=-1)
	{
		ArrProInBack[k][0]=ArrProInBack[ProInBack][0];
		ProInBack--;
	}
}
void MoveProToback(int id)
{
	ArrProInBack[ProInBack][0]=id;
	ArrProInBack[ProInBack++][1]=0;
}
void SwitchPro(int id,int B2F)
{
	for(int i=0;i<ProInBack;i++)
	{
		if(ArrProInBack[i][0]==id)
		{	
			ArrProInBack[i][1]=B2F;
			kill(ArrProInBack[i][0]*(-1),18);
			break;
		}
	}
}
void SwitchTerminal(int pid)
{
	if(tcsetpgrp(STDIN_FILENO,pid)!=0)
	{
		perror("Error while transfering terminal\n");
	}
	//printf("Terminal with %d\n",tcgetpgrp(STDIN_FILENO));
	waitpid(pid,NULL,WUNTRACED);
	AttachSignal();
	if(tcsetpgrp(STDIN_FILENO,shellId)!=0)
	{
		perror("Error while transfering terminal\n");
	}
}
void AttachSignal()
{
	signal(SIGTTOU,SIG_IGN);
}
void RemoveBSignal()
{
	signal(SIGTTOU,SIG_DFL);
}

//----------------functions for foreground background ends--//


void removeSpaces(char *str) 
{ 
    int count = 0; 
	int i=0;
    while(str[i]!='\0') 
		i++;
    if(str[i-1]==' ')
		str[i-1] = '\0'; 
} 

int chkpipes(char *line)
{
		if(strchr(line,'|')!=NULL)
	{
		return 1;
	}
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
int chkbg(char *line)
{
	char *sh=strchr(line,'&');
	if(sh!=NULL)
	{
		*sh='\0';
		if(*(sh-1)==' ')
			*(sh-1)='\0';
		return 1;
	}
}
int chkpip(char *line)
{
	char tes[10000];
	strcpy(tes,line);
	char *test[1000];
	int count=parse(tes,test);
	if(count<=0)
	{
		printf("error");
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
			return 4;
		}
		if(test[1][0]=='/' || test[1][0]=='~')
		{
			printf("changing directory to%s \n",test[1]);
			if(chdir(test[1])!=0)
			{
				int errnum = errno;
				fprintf(stderr, "Error no %d : %s\n",errnum, strerror( errnum ));
			}
			return 4;
		}
		gdir = getcwd(buf, sizeof(buf));
		dir = strcat(gdir, "/");
		to = strcat(dir, test[1]);
		printf("changing directory to%s \n",to);
		if(chdir(to)!=0)
		{
			int errnum = errno;
			fprintf(stderr, "Error no %d : %s\n",errnum, strerror( errnum ));
		}
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


int getcmd()
{
	fflush(stdin);
	clr(command,10000);
	getcwd(dir,sizeof(dir));
	printf("%d@%s>",getpid(),dir);
	fgets(command,sizeof(command),stdin);
}


int chk(char *cmd,char **envp)
{
	char c[100000];
	strcpy(c,cmd);
	char  *argv[10000]={'\0'};
	// printf("chk 2-> %s\n",cmd);
	int count=parse(c,argv);
	int i=0;
	if(strcmp(argv[0],"quit")==0||strcmp(argv[0],"Quit")==0||strcmp(argv[0],"Exit")==0||strcmp(argv[0],"EXIT")==0||strcmp(argv[0],"exit")==0||strcmp(argv[0],"QUIT")==0)
	{
		KillAllPending();
		printf("exiting\n");
		exit(0);
		return 1;
	}
	if(strcmp(argv[0],"jobs")==0)
	{
		print();
		return 1;
	}
	if((argv[0][0]=='f')&&(argv[0][1]=='g'))
	{
		int num=0;
		char *c;
		if(count>1)
		if((c=strchr(argv[1],'%'))!=NULL)
		{
			*c++;
			num = atoi(c);
		}
		RemoveBSignal();
		int pid = ArrProInBack[num][0];
		ArrProInBack[num][0] = ArrProInBack[ProInBack-1][0];
		ArrProInBack[num][1] = ArrProInBack[ProInBack-1][1];
		ProInBack--;
		printf("Process with pid : %d continued...\n",pid);
		SwitchTerminal(pid);
		return 1;
	}
}

int execfunc(char **argv,char **envp)
{
	// for(int i=0;argv[i];i++)
	// {
		// printf("[%d] : [%s]\n",i,argv[i]);
	// }
	// for(int i=0;envp[i];i++)
	// {
		// if(strncmp(envp[i],"PWD",3)==0)
		// printf("[%d] : [%s]\n",i,envp[i]);
	// }
	if(execve(*argv, argv,envp)<0)
	{
		int errnum = errno;
		fprintf(stderr, "Error no %d : %s\n",errnum, strerror( errnum ));
		exit(0);
	}
}
int chld(char *cmd,char **envp)
{	

	char  *oargv[10000]={'\0'};
	if(strncmp(cmd,"./",2)==0)
	{
		// printf("chld for %d %s\n",getpid(),cmd);
			if(chkbg(cmd)==1)
			{
				// printf("Child call stop %d\n",getpid());
				kill(getpid()*(-1),20);
			}
				// printf("Child call 3 1stop %d\n",getpid());
		int count=parse(cmd,oargv);
		// puts(oargv[0]);
		execfunc(oargv,envp);
	}
	else
	{

		if(chkbg(cmd)==1)
		{
			// printf("Child call stop %d\n",getpid());
			kill(getpid()*(-1),20);
		}
		
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
			// puts(argv[0]);
				// printf("Child call 3 stop %d\n",getpid());
			execfunc(argv,envp);
			// execvp(*argv,argv);
		}
	}
}

int demonchild(char *cmd,char **envp,char *cwd)
{
	char  *oargv[10000]={'\0'};
	if(strchr(cmd,'.')!=NULL)
	{
		char x[10000];
		strcpy(x,cmd);
		int count=parse(x,oargv);
		strcat(cwd,oargv[0]);

		execve(cwd,oargv,envp);
	}
	else
	{
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
int sparse(char *command,char **argv,char x)
{
	*argv++=command;
	while(*command!=x)
		*command++;
	*command++='\0';
	*argv=command;
	while(*command!='\n')
		*command++;
	*command='\0';
}
int splitoredirect(char *command,char  *x)
{
	char cmd[10000];
	strcpy(cmd,command);
	char *ptr = strtok(cmd, x);
	int i=0;
	while(ptr != NULL)
	{
		ptr = strtok(NULL, x);
		i++;
	}
	return i;
}
int redirecti(char *cmd,char **envp)
{
	char *args[1000];	
	 // printf("ri for %d %s\n",getpid(),cmd);

	int count=splitoredirect(cmd,"<");
	// printf("count%d \n",count);
	// for(int i=0;i<count;i++)
		// puts(args[i]);
	if(count!=2&&count!=1)
		printf("error in syntax : %s \n ",cmd);
	else
	{
		if(count==2)
		{
			char cme[100000];
			strcpy(cme,cmd);
			strcat(cme,"\0");
			sparse(cme,args,'<');
			// for(int i=0;args[i];i++)
				// printf("%d:[%s]\n",i,args[i]);
			char filename[1000];
			 char cwd[1000];
			 getcwd(cwd,1000);
			// int j=0;
			// for(int i=0;envp[i];i++)
			// {
				// puts(envp[i]);
				// if(strncmp(envp[i],"PWD",3)==0)
				// {
					// puts(envp[i]);
					// char *x=strchr(envp[i],'=');
					// *x++;
					// if(x!=NULL)
						// while(*x!='\0')
							// cwd[j++]=*x++;
				// }
				// cmd[j]='\0';
			// }
			// strcat(filename,cwd);
			// strcat(filename,"/");
			strcat(filename,args[1]);
			// printf("filename : %s\ncwd:%s\n",filename,cwd);
			removeSpaces(filename);
			int fd = open(filename,O_RDONLY);
			printf("Redirecting input from %s file descriptor %d\n",filename,fd);
			close(0);
			if(fd<0)
			{
				// perror("erro");
				printf("error in opening file\n");
				exit(0);
			}
			if(dup2(fd, 0)!=0)
			{
				printf("error in file");
				exit(0);
			}
			// close(fd);    
			char cm[10000];
			strcpy(cm,args[0]);
			removeSpaces(cm);
			// printf("filename : %s \n ",filename);
			// printf("comamnd %s \n",cm);
			chld(cm,envp);
		}
		else
		{
			chld(cmd,envp);
		}
	}

	// printf("count %d \n ",count);
	//splitoexecute(args,count,env);
}


int redirecto(char *cmd,char **envp)
{
	char *args[1000];	
	// printf("r0 for %d %s\n",getpid(),cmd);

	int count=splitoredirect(cmd,">");
	// printf("count%d \n",count);
	// for(int i=0;i<count;i++)
		// puts(args[i]);
	if(count!=2&&count!=1)
		printf("error in syntax : %s \n ",cmd);
	else
	{
		if(count==2)
		{
			char cme[100000];
			strcpy(cme,cmd);
			strcat(cme,"\0");
			sparse(cme,args,'>');
			// for(int i=0;args[i];i++)
				// printf("%d:[%s]\n",i,args[i]);
			char filename[1000];
			strcpy(filename,args[1]);
			removeSpaces(filename);
			int fd = open(filename, O_RDWR |O_CREAT, S_IRUSR | S_IWUSR);
			printf("Redirecting output to %s file descriptor %d\n",filename,fd);
			close(1);
			if(!fd)
			{
				printf("error openning file\n");
				exit(0);
			}
			dup2(fd, 1);
			// close(fd);    
			char cm[10000];
			strcpy(cm,args[0]);
			removeSpaces(cm);
			redirecti(cm,envp);
		}
		else
		{
			redirecti(cmd,envp);
		}
	}

	// printf("count %d \n ",count);
	//splitoexecute(args,count,env);
}
//------------------------------------Signal handling in child------------------------//
void sighup() 
  
{ 
    kill(SIGUSR1,getppid());
} 
//------------------------------------Signal handling in child ends------------------------//
int exe(char *cmd,int x,int y,char **envp)
{
	char arg[1000];
	strcpy(arg,cmd);
	int ret;
	if((ret=fork())==0)
	{
	// printf("Exe for %d %s\n",getpid(),cmd);
		setpgid(STDIN_FILENO,getpid());

        signal(SIGHUP, sighup); 
        signal(SIGINT, sighup); 
        signal(SIGQUIT, sighup);


		if(x!=-1)
		{
			close(pip[x][1]);
			close(0);
			dup2(pip[x][0],0);
		}
		if(y!=-1)
		{
			close(pip[y][0]);
			close(1);
			dup2(pip[y][1],1);
		}
		redirecto(arg,envp);
		// fgbg(arg,envp);
		exit(0);
	}

	return ret;
}


int splitpipe(char *cm,char **args)
{
	char cmd[10000];
	strcpy(cmd,cm);
	char *ptr = strtok(cmd, "|");
	int i=0;
	while(ptr != NULL)
	{
		*(args+i)=(char * ) malloc(1000*sizeof(char));
		strcpy(args[i],ptr);
		ptr = strtok(NULL, "|");
		// printf("split > %s\n",args[i]);
		i++;

	}
	return i;
}

int pipeexecute(char **args,int count,char **envp)
{
	int i=0;
	pid_t ret[1000];
	int pid_bg[1000]; 
			// printf("Check.y...%s ->\n",args[0]);
	for(i=0;i<count;i++)
	{
		if(i>1)
		{
			close(pip[i-2][0]);
			close(pip[i-2][1]);
		}	
		pipe(pip[i]); 
		removeSpaces(args[i]);
		if(strchr(args[i],'&')!=NULL)
		{
			pid_bg[i] = 1;
		}	
		else 
			pid_bg[i]=0;
			
		int l=0;
		int x=-1;
		int y=-1;
		if(i!=0)
			x=i-1;
		if(i!=count-1)
			y=i;
		ret[i]=exe(args[i],x,y,envp);
	}
	if(count>1)
	{
		close(pip[i-2][0]);
		close(pip[i-1][0]);
		close(pip[i-2][1]);
		close(pip[i-1][1]);
	}
	i=0;
	for(i=0;i<count;i++)
	{
		int status;
		if(pid_bg[i]==1)
		{
			// printf("Waiting for ... %d\n",ret[i]);
			MoveProToback(ret[i]);
			char mycmd[1000]={0};
			strcpy(mycmd,args[i]);
			char *c = strchr(mycmd,'&');
			*c = 0;
			printf("%s is running in background with Pid : %d\n",mycmd,ret[i]);
			waitpid(ret[i],&status,WUNTRACED);
			kill(ret[i]*(-1),18);
			SwitchPro(ret[i],1);
		}
		else
		{
			// printf("n\n");
			RemoveBSignal();
			currpid = ret[i];
			SwitchTerminal(ret[i]);
			waitpid(ret[i],&status,WUNTRACED);

			char mycmd[1000]={0};
			strcpy(mycmd,args[i]);
			char *c = strchr(mycmd,'\n');
			*c = 0;
			printf("%s pid : %d\t",mycmd,ret[i]);
			if(WIFEXITED(status))
				printf("Normal Exit: %d\n",status);
			else if(WIFSIGNALED(status))
				printf("Abnormal Exit with signal : %d\n",WTERMSIG(status));
			else if(WIFSTOPPED(status))
				printf("Process Stopped : %d\n",WSTOPSIG(status));
			else if(WIFCONTINUED(status))
				printf("Process Continued :\n");
			SwitchTerminal(shellId);
		}
	}
	clr(command,10000);
}

int dm(char *cmd,char **envp)
{
	char cm[100000];
	char *ar[10000];
	strcpy(cm,cmd);
	parse(cm,ar);
	if(strcmp(ar[0],"daemonize")==0)
	{
		int ret=fork();
		if(ret==0)
		{			
			char cwd[10000];
			getcwd(cwd,10000);
			daemon(0,0);
			demonchild(cmd,envp,cwd);
		}
		return 1;
	}
	return 0;
}
int pipexe(char *cmd,char **env)
{
	char *args[1000];
	// printf("pipe -> %s\n",cmd);
	int count=splitpipe(cmd,args);
	// printf("count %d \n ",count);
	
	pipeexecute(args,count,env);
}
int fgbg(char *cmd,char **envp)
{
	int bg=0;
	// printf("%s\n",cmd);
	pipexe(cmd,envp);
}
int own(char *cmd,char **envp)
{

		// printf("chk -> %s\n",cmd);
	if(dm(cmd,envp)==1)
		return 0;
	else
	{
		if(chk(cmd,envp)==1)
			return 0;
		else
		{
			// printf("chk -> %s\n",cmd);
			pipexe(cmd,envp);
		}
	}	
}
void Chld_died(int sig, siginfo_t *info, void *context)
{
    int signalPid = info->si_pid;
	BackProDied(signalPid);
}

int main(int argc,char *arg[],char *envp[])
{
	// int i;
    // for (i = 0; envp[i] != NULL; i++) 
        // printf("\n%s", envp[i]); 
	shellId = getpid();
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = Chld_died;
    sigaction(SIGUSR1, &sa, NULL);
	while(1)
	{
		getcmd();
		int set=chkpip(command);
		if(set==0)
		{
			own(command,envp);
			// execute(envp);
		}
	}
	exit(0);
}
