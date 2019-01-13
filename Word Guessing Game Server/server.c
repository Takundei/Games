#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <netdb.h> //getadrinfo

#include <time.h>
#include <event.h>

void challenge(char* str, char* word);
void fortune();


int setnonblock(int fd)
{   int flags;
    /*
    Unix Systems Programming: Communication, Concurrency, and Threads
    By Kay A. Robbins, Steven Robbins, Chapter 4

    */
    flags = fcntl(fd, F_GETFL);
    if (flags < 0)
        return flags;
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0)
        return -1;

    return 0;
}

struct client
{   struct event play;
    int correct;
    int total;
    char word[15];
};

void game(int fd,short what, void *arg) //g + f
{   struct client *client = (struct client *)arg;
    char buf[35];			 //text from client  - 
    char buffer[35];
    char send[150]; 		//text to client
    int ret;
    int cmp;
    char quit[] = "Q:";
    char prefix[] = "R:";
    char res[50];
    char *word;
    word = NULL;
    memset(buf,0,sizeof(buf));
    memset(send,0,sizeof(send));
    memset(res,0,sizeof(ret));

    ret = read(fd,buf,sizeof(buf));
	if(ret == -1) perror("read");
    if(ret == 0 || ret < 0)
    {   printf("[%d] client disconnected\n",fd);
        if(close(fd) < 0) perror("close");
        ret = event_del(&client->play);
        if(ret == -1) perror("event_del");
        free(client);


        return;
    }

    if((strstr(buf,quit) != NULL) && (strstr(buf,quit) - buf) == 0) //Client disconects
    {   (void) sprintf(res,"M: You mastered %d/%d challenges. Good bye!\n",client->correct,client->total -1); //-1? not counting the missed attempt
        ret = write(fd,res,strlen(res));
        if(ret == -1) perror("write");
        printf("[%d] client disconnected\n",fd);
        if(close(fd) < 0) perror("close");
        if(event_del(&client->play) < 0) perror("event_del");
        free(client);

        return;
    }

    //Check input
    if((strstr(buf,prefix) == NULL) || (strstr(buf,prefix) - buf) != 0)
    {   (void) sprintf(res,"M: Send your guess in the form 'R: word\\r\\n '.\n");
        ret = write(fd,res,strlen(res));
        if(ret == -1) perror("write");
        return;
    }
    cmp = 0; //if input is <= 3 chars, no comparison
    buf[strlen(buf) - 1] = '\0';
    word = buf;
    if(strlen(buf) > 3)
    {   word += 3; //discard 'R: '
        cmp = 1;
    }
    
    /*Remove any extra spaces the user might have entered
    char ch = ' ';
    char * rt;
    if((rt = strchr(word,ch)) != NULL){
		word[rt - word] = '\0';
	}
	*/
    
    if(cmp && strcmp(word,client->word) == 0)
    {   strcpy(res,"O: Congratulation - challenge passed!");
        client->correct ++;
    }
    else
    {   strcpy(res,"F: Wrong guess - expected: ");
        strcat(res,client->word);
    }
    client->total ++;

    memset(client->word,0,sizeof(client->word));

    fortune(buffer,sizeof(buffer));//start fortune
    challenge(buffer,client->word);//create challenge

    (void) sprintf(send,"%s\nC: %s",res,buffer); //prepare post
    ret = write(fd,send,strlen(send));
    if(ret == -1) perror("write");
}

void client_accept(int fd,short what, void *arg)
{   struct client *client;
    char buffer[35];
    char send[150];
    int ret;

    int client_fd = accept(fd,NULL,0);

    setnonblock(client_fd);
    printf("[%d] client connected\n",client_fd);
    client = calloc(1,sizeof(*client));
    if(!client)
    {   perror("calloc");
        return;
        exit(EXIT_FAILURE);
    }

    event_set(&client->play,client_fd,EV_READ|EV_PERSIST,game,client);
    ret = event_add(&client->play,NULL);
    if(ret == -1) perror("event_add");

    memset(client->word,0,sizeof(client->word));
    client->total ++;

    fortune(buffer,sizeof(buffer));
    challenge(buffer,client->word);

    (void) sprintf(send,"M: Guess the missing ____!\nM: Send your guess in the form 'R: word\\r\\n '.\nC: %s",buffer);

    ret = write(client_fd,send,strlen(send));
    if(ret == -1)
    {   perror("write");
    }
}


void fortune(char *buffer,size_t size)
{   int fd[2];

    if(pipe(fd) < 0)
    {   perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    int ret;
    ssize_t count;
    memset(buffer,0,size);

    char *fortune_args[] = {"fortune","-n","32","-s",NULL};

    pid = fork();
    if(pid == -1)
    {   perror("fork");
        exit(EXIT_FAILURE);
    }
    else if(!pid)
    {   //in child
        close(fd[0]);
        ret = dup2(fd[1],STDOUT_FILENO);
        if(ret == -1)
        {   perror("dup2");
            _exit(EXIT_FAILURE);
        }
        close(fd[1]);
        execvp("fortune",fortune_args);
        perror("execl");
        _exit(EXIT_FAILURE);

    }
    else
    {   //in parent
        close(fd[1]);
        ret = dup2(fd[0],STDIN_FILENO);

        if(ret == -1)
        {   perror("dup2");
            exit(EXIT_FAILURE);
        }
        close(fd[0]);

        while(1)
        {   count = read(STDIN_FILENO,buffer,size);
            if(count == -1)
            {   if(errno = EINTR)
                    continue;
                perror("read");
                exit(EXIT_FAILURE);
            }
            if(count == 0)
                break;
        }
        wait(0);

    }
}
void challenge(char* str, char* word)
{

    int idx[16]; //position of spaces
    const char ch = ' ';
    int i,j;
    char* ret;
    char *temp = str;
    int k =0;
    i = 1;

		
    idx[0] = 0;
    while((ret = strchr(temp,ch)) != NULL)   //get idx of all spaces
    {   idx[i] = ret - str+1;
        temp =ret+1;
        i++;

    }
    j =i;
    while(str[j])   //find end of string
    {   j++;
    }
    idx[i] =j-1; //last idx i is the end of str & removes puntuation mark
    //number of valid spaces
    j = rand() % i; //chose random space
    k = 0;
    for(int i = idx[j]; i < idx[j+1]-1 ; i++,k++) //hide word
    {   if(str[i] == ',' || str[i] == ':') break;
        word[k] = str[i];
        str[i] = '_';

    }
    word[k+1]='\0';

}

int main(int argc, char **argv)
{   int listen_fd;
    int ret;
    int val = 1;
    //socklen_t addrlen;
    struct sockaddr_in sAddr;
    struct event sAccept;
    struct addrinfo hints, *res;
    bzero(&hints,sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; //unspecified ai_family
    hints.ai_socktype = SOCK_STREAM;
    (void) event_init();
    srand((unsigned) time(NULL));

    if(argc != 3)
    {   fprintf(stderr,"USAGE: .//%s [host] [port]\n",argv[0]);
        exit(EXIT_FAILURE);

    }

    memset((void *) &sAddr, 0, sizeof(struct sockaddr_in));

    ret = getaddrinfo(argv[1],argv[2],&hints,&res);

    if(ret)
    {   fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(ret));
        freeaddrinfo(res);
        exit(EXIT_FAILURE);

    }

    do //find a connection
    {   listen_fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
        if(listen_fd < 0) continue; //try next one

        ret = setsockopt(listen_fd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));
        if(ret == -1)
        {   perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        ret = bind(listen_fd,res->ai_addr,res->ai_addrlen);
        if(ret == 0) break; //success

        close(listen_fd);

    }
    while((res = res->ai_next) != NULL);

    if(res == NULL)
    {   perror("Error connecting");
        exit(EXIT_FAILURE);
    }
    ret = listen(listen_fd,5);
    if(ret != 0)
    {   perror("listen");
    }

    freeaddrinfo(res);

    event_set(&sAccept,listen_fd,EV_READ | EV_PERSIST,client_accept,NULL);

    ret = event_add(&sAccept,NULL);
    if(ret == -1) perror("event_add");
    ret = event_dispatch();
    if(ret == -1) perror("event_dispatch");

}

