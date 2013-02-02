/************************************************************************
File name - lfy_tcp_client.c
Purpose - A tcp client, which accepts data from keyboard and sends the 
data to a server process.
To compile - gcc lfy_tcp_client.c functions.c -o myclient
To run - ./myclient <server's IP address> <serever's port no>
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
#include	<fcntl.h>
#include	<signal.h>
#include	<unistd.h>
#define BUFSIZE 512

/*Function prototypes*/
int my_work(FILE *, int);
int myread(int, int, void* );
int mywrite(int, int, void* );

int main( int C, char *argv[] )
{
	int	sd, ret;
	struct	sockaddr_in serveraddress;
	if (NULL == argv[1])
	{
		printf("Please enter the IP Address of the server\n");
		exit(1);
	}
	if (NULL== argv[2])
	{
		printf("Please enter the Port Number of the server\n");
		exit(1);
	}
	sd = socket(AF_INET, SOCK_STREAM, 0 );
	if(0 > sd ) 
	{
		perror( "socket" );
		exit (1);
	}

	memset( &serveraddress, 0, sizeof(serveraddress) );
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_port = htons(atoi(argv[2]));//PORT NO
	serveraddress.sin_addr.s_addr = inet_addr(argv[1]);//ADDRESS

	printf("Client - Trying to connect to server\n");
        ret = connect(sd, (struct sockaddr*)&serveraddress, sizeof(serveraddress));
        if (0 > ret)
        {
            perror("Could not connect to server\n");
            exit(1);
        }

	printf("Client - Connected Successfully to server\n" );

        /*Now invoke function to accept user data and proceed*/
	ret = my_work(stdin,sd);
	if (0 > ret)
	{
		printf("Client Exiting - Some error occured\n");
		close(sd);
		exit(1);
	}
        printf("Client - exiting successfully\n");
	close(sd);
	exit(0);
}

/*********************************************************************
Function: - my_work (sends data to server process using tcp socket)
Input - 
	a)input data source(here taken as stdin).
	b)socket descriptor.

Returns - 
	0 or -1 based on whether it exited successfully, when 
	there was end of file(means the user stopped sending data) or 
	if any error condition was encountered.
**********************************************************************/

int my_work(FILE *fp,		/*Here to be used as stdin as argument*/	 
		int sockfd )	/*Connection Socket */
{

	char sendbuf[BUFSIZE]; 
	char recvbuf[BUFSIZE];
	char *cptr;
	int ret;
	int numbytes;
        unsigned short slen;
        unsigned short rlen;

	for(;;)
	{

		printf("Enter Data For the server or press CTRL-D to exit\n");
		/*Reading data from the keyboard*/
		cptr = fgets(sendbuf,BUFSIZE,fp);
		if (NULL == cptr)
		{
			printf("Possible error or end of file\n");
			return 0;
		}
		slen = strlen (sendbuf);
		/*Sending the length of data after converting it to network
                byte order*/
                slen = htons(slen);
		ret = mywrite(sockfd, sizeof(slen), &slen);
		if (0 > ret)
		{
			perror("Error in sending data:\n");
			return -1;
		}
		/*Now send the data*/
		ret = mywrite(sockfd, strlen(sendbuf), sendbuf);
		if (0 > ret)
		{
			perror("Error in sending data:\n");
			return -1;
		}

		printf("Data Sent To Server\n");
                /*wait for receiving the length of data*/
		numbytes = myread(sockfd, sizeof(rlen), &rlen);
		if (0 > numbytes)
		{
			perror("Error in receiving data:\n");
			return -1;
		}
		/*Now read the data*/
		numbytes = myread(sockfd, ntohs(rlen), recvbuf);
		/*writing to stdout*/
                printf("Received from server:\n");
		write(1,recvbuf,numbytes); 
	}
}
