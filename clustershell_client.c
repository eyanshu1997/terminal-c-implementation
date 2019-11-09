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
#define SA struct sockaddr 
int func(char *buff,int sockfd,char *buf) 
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

//Structure for node---------//
struct node 
{
    int port;
    char name[20];
    struct node *next;
};
int ReturnPort(struct node *temp,char name1[20])
{
    int port=-1;
    while(temp!=NULL)
    {
        if(strcmp(temp->name,name1)==0)
        {
            port = temp->port;
            break;
        }
        else
        {
        temp = temp->next;
        }
    }
    return port;
}

struct node * AddData(struct node* root,int port,char name[20])
{
    struct node *n1 = (struct node *)malloc(sizeof(struct node));

    strcpy(n1->name,name);
    n1->port = port;
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



int client(char *buff,int port,char * re)
{
    
    //--------------------client connection ---------------------//

    int sockfd, connfd; 
    struct sockaddr_in servaddr, cli; 
  
    // socket create and varification 
    sockfd = socket(AF_INET, SOCK_STREAM, 0); 
    if (sockfd == -1) { 
        // printf("socket creation failed...\n"); 
        return 3; 
    } 
    else
        // printf("Socket successfully created..\n"); 
    bzero(&servaddr, sizeof(servaddr)); 
  
    // assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(port); 
  
    // connect the client socket to server socket 
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) { 
        // printf("connection with the server failed...\n"); 
        return 2; 
    } 
    // else
        // printf("connected to the server..\n"); 

    // function for chat 
    int ret = func(buff,sockfd,re); 
  
    // close the socket 
    close(sockfd); 
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
    int i=0;
    int flag=0;
    int j=0;
    while(!feof(fp))
    {   
        memset(name,0,20);
        port=0;
        fscanf(fp,"%s %d\n",name,&port);  
        if(port!=0)
        {
            root=AddData(root,port,name);
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
		*command++='\0';
	}
}
int main(char argc,char *argv[]) 
{
	if(argc!=3)
	{
		printf("enter correct args <configfilepath name>");
	}
	char re[MAX];
    struct node *root  = CreatePortFile(argv[1]);
    int port=-1,res;
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
            printf("exiting\n");
            exit(0);
            return 1;
        }
		 if(strncmp(buff,"nodes",5)==0)
        {
            struct node *temp=root;
            while(temp!=NULL)
            {
                if(fork()==0)
                {
					// printf("checking %s\n",temp->name);
                    // res=client("ls\0",temp->port,re);
					if(res==0)
						printf("Node %s active at %d \n",temp->name,temp->port);
					exit(0);
                }
                else
                {
                    wait(NULL);
                    temp=temp->next;
					// sleep(10);
                }
                
            }
        }
		else
		{
				
			// printf("entered command %s\n",buff);	
			 if(strncmp(buff,"n*",2)==0)
			{
				struct node *temp=root;
				while(temp!=NULL)
				{
					if(fork()==0)
					{
						c = strchr(buff,'.');
						*c++='\0';
						char command[1000];
						int i;
						for(i=0;*(c+i);i++)
						{
							command[i]=*(c+i);
							*(c+i)='\0';
						}
						command[i]='\0';
						// printf("Calling %s:At port %d\n",command,temp->port);
						res=client(command,temp->port,re);
						if(res==0)
							printf("result from %s : %s\n",temp->name,re);
						exit(0);
					}
					else
					{
						wait(NULL);
						temp=temp->next;
					}
					
				}
			}
			else
			{
				if(strchr(buff,'|')!=NULL)
				{
					// printf("splitting\n");
					char *bu[MAX];
					sparse(buff,bu,'|');
					

					for(int i=0;bu[i];i++)
					{
						// printf("bu[%d]:%s\n",i,bu[i]);
					}
								
						c=strchr(bu[0],'.');	
						*c++='\0';
						char command[1000];
						int i;
						for(i=0;*(c+i);i++)
						{
							command[i]=*(c+i);
							*(c+i)='\0';
						}
						command[i]='\0';
				
						port = ReturnPort(root,bu[0]);
						if(port==-1)
						{
							printf("No such node active...\n");
						}
						else
						{
						   res = client(command,ReturnPort(root,bu[0]),re);
						  if(res!=0)
						  {
							  printf("error in server\n");
							  exit(0);
						  }
						}
						// printf("recieved result from 1 [%s] \n",re);
						int j;
						for(j=0;re[j];j++)
							;
						if(re[j-1]=='\n')
							re[j-1]='\0';
						// printf("recieved result from 1after mani [%s] \n",re);
						for(int i=1;bu[i];i++)
						{
									char b[MAX];
							
							int k=0;
							int j=0;
							while(bu[i][k]!=' ')
								k++;
							for(k=0;bu[i][k];k++)
									b[j++]=bu[i][k];
					
								c=strchr(b,'.');	
								
								*c++='\0';
								bzero(command,1000);

								for(i=0;*(c+i);i++)
								{
									command[i]=*(c+i);
									*(c+i)='\0';
								}
								command[i]='\0';
								char cm[100000];
								strcat(cm,"output:");
								strcat(cm,re);
								strcat(cm,"|");
								strcat(cm,command);
								bzero(re,MAX);
								port = ReturnPort(root,b);
								if(port==-1)
								{
									printf("No such node active...(use pipe and node name together eg n1.ls|n2.wc)\n");
								}
								else
								{
								   res = client(cm,ReturnPort(root,b),re);
								  if(res!=0)
								  {
									  printf("error in server\n");
									  exit(0);
								  }
								}
						}
				}
				else
				{
					if((c=strchr(buff,'.'))!=NULL)
					{
						*c++='\0';
						char command[1000];
						int i;
						for(i=0;*(c+i);i++)
						{
							command[i]=*(c+i);
							*(c+i)='\0';
						}
						command[i]='\0';
				
						port = ReturnPort(root,buff);
						if(port==-1)
						{
							printf("No such node active...\n");
						}
						else
						{
						   res = client(command,ReturnPort(root,buff),re);
						  if(res!=0)
						  {
							  printf("error in server\n");
							  main(argc,argv);
						  }
						}
					}
					else
					{
						port = ReturnPort(root,argv[2]);
						if(port==-1)
						{
							printf("No such node active...\n");
						}
						else
						{
						   res = client(buff,ReturnPort(root,argv[2]),re);
						   if(res!=0)
						   {
							   printf("error in server\n");
							   main(argc,argv);
						   }
						}
					}
				}
				
				printf(" result: %s\n",re);
			}
		}
	}
    
    
}