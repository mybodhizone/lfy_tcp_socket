#include<unistd.h>
#include<stdio.h>
/****************************************************************
Function - myread
Arguments - socket descriptor, number of bytes to read, array
in which the data read is to be stored
Returns - Number of bytes successfully read or -1 on error
****************************************************************/
int myread(int sd, int n, void* buffer)
{
	int ret = 0;
	int pointer = 0;
	int totnoofbytes = 0;
	while (1)
	{
		ret = read(sd, buffer + pointer, n - totnoofbytes);
		if (0 > ret)
		{
			perror("I am getting error in read!!\n");
			return -1;
		}
			
		totnoofbytes = totnoofbytes + ret;
		pointer = pointer + ret;
		//totnoofbytes = 0 means, nothing more to read.
		if ((totnoofbytes == n) || (ret == 0))
		{
			return totnoofbytes;
		}
	}
}
/****************************************************************
Function - mywrite
Arguments - socket descriptor, number of bytes to write, array
having the data
Returns - Number of bytes successfully written or -1 on error
****************************************************************/
int mywrite(int sd, int n, void* buffer)
{
	int ret = 0;
	int pointer = 0;
	int totnoofbytes = 0;
	while (1)
	{
		ret = write(sd, buffer + pointer, n - totnoofbytes);
		if (0 > ret)
		{
			perror("I am getting error in write!!\n");
                        return -1;
		}
			
		totnoofbytes = totnoofbytes + ret;
		pointer = pointer + ret;

		/*All bytes written*/
		if (totnoofbytes == n) 
		{
			return totnoofbytes;
		}
	}
}
