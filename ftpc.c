//lab2
//subash chebolu (chebolu.1)
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <libgen.h>



/*
method for sending file
*/
void sendFile(int * sockfd, FILE * transFile, uint32_t fsize, char * filename)
{
	char * buf = (char *) malloc(1000);
	int read = 0;	
	
	//creating buffer to use for sending and making header
	fprintf(stderr, "%s %d\n","size of file is", fsize);
	fprintf(stdout,"%s %s\n","name of file is",basename(filename));
	bzero(buf,1000);
	uint32_t netfsize = htonl(fsize);	
	memcpy(buf, &netfsize,sizeof(uint32_t));
	memcpy(buf+4,basename(filename),strlen(filename));
	//stopping if header fails
	if(send(*sockfd,buf,24,0) != 24)
	{
		fprintf(stderr,"%s\n","failed to send file header succesfully");
		close(*sockfd);
		exit(0);
	}
	//checking for EOF and sending until EOF is hit
	while(feof(transFile) == 0)
	{
		read = fread(buf,1,1000,transFile);
		int sent = send(*sockfd,buf,read,0);
		//stopping if number of bytes read is not how many were sent
		if(sent != read)
		{
			fprintf(stdout, "%s %d %s %s\n\n","sent only", sent,"bytes because", strerror(errno));
			fprintf(stderr,"%s\n","failed to send correctly");
			close(*sockfd);
			exit(0);
		}
	}
	//freeing allocated buffer
	free(buf);
	return;
}


/*
 for checking filesize and is file is a regular file or directory
*/
unsigned long fileSize(const char *filePath)
{
 
 struct stat fileStat;//declare file statistics struct
 
 stat(filePath,  &fileStat);//filling fileStat with statistics of file
	
//if file is directory stop program and tell user	
 if(S_ISDIR(fileStat.st_mode))
 {
	 fprintf(stderr,"%s\n", "not a file");
	 exit(0);
 }
 	 
 return (uint32_t)fileStat.st_size;//returning only the size of file from stat struct in uint32_t format for 4 byte filesize
}

/*
creating the actual socket connection and setting the socket address details
*/

void createConnection(int * sockfd, struct sockaddr_in * sockaddr, char *port, char * serverIp)
{
	*sockfd = socket(AF_INET, SOCK_STREAM,0);
	//stop if socket can't be made
	if(*sockfd < 0)
	{
		fprintf(stdout, "%s\n", "socket could not be made");
		exit(0);
	}
	
	sockaddr->sin_family = AF_INET;
	sockaddr->sin_port = htons(atoi(port));
	sockaddr->sin_addr.s_addr = inet_addr(serverIp);	
	memset(&(sockaddr->sin_zero),'\0',8);
	//stop if connection fails
	if(connect(*sockfd,(struct sockaddr *)sockaddr,sizeof(struct sockaddr_in)) < 0)
	{
		fprintf(stderr, "%s failed because %s\n", "failed to connect", strerror(errno));
		close(*sockfd);
		exit(0);
	}
	return;
}

/*
main method of ftpc
*/
int main(int args, char * argv[])
{
	//checking number of aurguments first and stopping if there aren't the right number of aurguments
	if (args != 4)
	{
		fprintf(stderr,"%s %d %s\n" , "please give three aurguments,",args - 1,"were entered, the aurguments are remote-ip, remote-port, local-file-to-transfer");

	}
	
	FILE* transFile = fopen(argv[3],"r");//trying to open local-file-to-transfer in read mode

	//end program is file couldn't be opened
	if(transFile == NULL)//Checking if transfer file was opened correctly
 	{
		fprintf(stdout,"%s","please enter a valid path to an existing file for the third argument\n");
		exit(0);//if not opened correctly, exit program
	}
	//uint32_t to gaurentee only 4 bytes for the filesize
 	uint32_t fsize = fileSize(argv[3]);	
	//socket descriptor created 
	int sockfd;
	//socket address created statically
	struct sockaddr_in sockaddr;
	//creating socket connection		
	createConnection(&sockfd, &sockaddr, argv[2], argv[1]);
	//sending file through created connection
	sendFile(&sockfd,transFile,fsize,argv[3]);
	//successful finish
	return 1;
}
