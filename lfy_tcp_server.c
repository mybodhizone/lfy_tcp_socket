/************************************************************************
File name - lfy_tcp_server.c
Purpose - An tcp server, which accepts data sent by a client,
and echoes back the same data to the client.
To compile - gcc lfy_tcp_server.c  functions.c -o myserver
To run - ./server 
Port Number of  server hardcoded as 11710 (can be changed)
************************************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include	<errno.h>
#include	<string.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>
#include	<sys/wait.h>
#include	<fcntl.h>
#include	<unistd.h>
#include	<signal.h>

/*Function prototypes*/
int serverwork(int );
int myread(int, int, void* );
int mywrite(int, int, void* );

#define QSIZE 5
#define BUFSIZE 512
#define MYPORT 11710
#define MAXNAME 100 
int main(int argc,  char *argv[] )
{
	int	sd, ret, connfd;
	struct	sockaddr_in serveraddress,cliaddr;
	socklen_t length;
	char buffer[MAXNAME];

	sd = socket(AF_INET, SOCK_STREAM, 0 );
	if(0 > sd ) 
	{
		perror( "socket" );
		exit( 1 );
	}

	memset( &serveraddress, 0, sizeof(serveraddress) );
	memset( &cliaddr, 0, sizeof(cliaddr) );
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(MYPORT);//PORT NO
	serveraddress.sin_addr.s_addr = htonl(INADDR_ANY);//IP ADDRESS
	ret = bind(sd,(struct sockaddr*)&serveraddress,sizeof(serveraddress));
	if(0 > ret)
	{
		perror("Bind Fails:");
		exit(1);
	}
        ret = listen(sd, QSIZE);
        if (0 > ret)
        {
            perror("Listen fails\n");
            exit(1);
        }
	for(;;)
	{
		printf("TCP  Server:  Waiting for new client connection\n");
		length = sizeof(cliaddr);
 		connfd = accept(sd,(struct sockaddr*)&cliaddr,&length);
	        if ( 0 > connfd)
 		{
         		if (errno == EINTR)
         		printf("Interrupted system call ?");
         		continue;
 		}
 		
		printf("Server: Connection from  client -%s\n",
                     inet_ntop(AF_INET,&cliaddr.sin_addr,buffer,sizeof(buffer)));
                /*Now serve this client*/
                while(1)
                {
                    ret = serverwork(connfd);
                    /*ret = 0 or -ve means break no longer this client is to
                    to be served. Now wait for another client connection*/
                    if (0 >= ret)
                    {
                            close(connfd);
                            break;
                    }
                }
	}
}

/****************************************************************************
*Function Name - serverwork
*Description -reads data from client through connection socket and echoes
back the same data. Befor reading/sending data it receives/sends two 
byte of data, which is the lenghth of data.
*Arguments -connection socket
*Returns -0 when client closes connection, -ve value in case of any error,
+ve value when successfull.
****************************************************************************/
int serverwork(int connfd)
{

    char recvbuf[BUFSIZE];
    unsigned short rlen;
    unsigned short slen;
    int ret;

    /*Wait to receive to get the length of data from client*/
    ret = myread(connfd, sizeof(rlen), &rlen);
    if (0 >= ret)
    {
        printf("Returning to main - Client closed or some error\n");
        return ret;
    }
    /*Now read so many bytes of data, whose length was received in the last
    step*/
    ret = myread(connfd, ntohs(rlen), recvbuf); 
    if (0 >= ret)
    {
        printf("Returning to main - client closed or some error\n");
        return ret;
    }
    /*Printing the received data on screen after building the string*/
    recvbuf[ret] = '\0';
    printf("Server: Received from client:%s", recvbuf);

    /*Now echoing back the length and data to client*/
    slen = strlen(recvbuf);
    /*convert the short to network byte order*/
    slen = htons(slen);
    printf("Sending length - %d\n", ntohs(slen));
    /*send the 2 bytes(short) length of data to client*/
    ret = mywrite(connfd, sizeof(slen) , &slen);
    if (0 >= ret)
    {
        printf("Returning to main 0 bytes written or some error\n");
        return ret;
    }
    /*send the data to client*/
    ret = mywrite(connfd,strlen(recvbuf), recvbuf);
    if (0 >= ret)
    {
        printf("Returning to main 0 bytes written or some error\n");
        return ret;
    }
    /*Everything successful*/
    return ret;
}

