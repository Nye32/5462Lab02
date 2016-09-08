//_______________________
//| Andrew Lee			|
//| CSE5462	Lab 02		|
//| 09-05-16	ftps.c	|
//|_____________________|

// NOTE: Large portion of code copied from Project 1
// 		TCP server.c example.



// Required Libraries
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#define BUFSIZE 1000

main (int argc, char *argv[]) {
	// Variables
	int port;
	int socket;
	int msgsocket;
	int rval = 0;
	int filesize = 0;
	char filename[] = "";
	struct sockaddr_in sin_addr;
	char databufin[BUFSIZE];
	char databufout[BUFSIZE] = "Server Response: hi";
	
	// Ensure Proper Argument Usage
	if (argc != 2) {
		printf("ERROR: Incorrect number of arguements. Please run using the following format...\n");
		printf("ftps <local-port>\n\n")
		exit(1);
	}
	port = argv[1];
	
	// Wait for client connection
	printf("TCP Server Initialized. Awaiting Clients...")
	if ((socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error: Unable to Open Datagram Socket.")
		exit(1);
	}
	
	// Construct sent socket
	sin_addr.sin_family = AF_INET;
	sin_addr.sin_addr.s_addr = INADDR_ANY;
	sin_addr.sin_port = htons(atoi(port));
	
	// Bind Socket
	if (bind(socket, (struct sockaddr *)&sin_addr, sizeof(struct sockaddr_in)) < 0) {
		perror("Error: Unable to Bind Stream Socket");
		exit(1);
	}
	
	// Listen for socket connections (Max open connections is 5)
	listen(socket, 5);
	
	// Accept 1 connection: msgsocket
	if ((msgsocket = accept(socket, (struct sockaddr *)NULL, (int *)NULL)) == -1) {
		perror("Error: Unable to connect Stream Socket.");
		exit(1);
	}
	
	// Clear Buffer (Set it to 0)
	bzero(databufin, sizeof(databufin));
	
	// Read Header
	while (rval < 24) {
		rval += recv(msgsocket, databufin, 24, 0);
	}
	
	// Determine Size of file from header
	char temp[4];
	for (i = 0; i < 4; i++) {
		temp[i] = databufin[i];
	}
	filesize = atoi(temp[]);
	filesize = ntohl(filesize);
	printf("Filesize: %d\n", filesize);
	
	// Determine Filename
	char temp[20];
	for (i = 0; i < 20; i++) {
		temp[i] = databufin[i+4];
	}
	filename = temp;
	printf("Filename: %s\n", filename);

	// Output File
	FILE *oufp;
	oufp = fopen("recvd/" + filename, "w");
	if (oufp == NULL) {
		perror("Error: Unable to create output file %s", filename);
	}
	
	rval = 0;
	int temp = 0;
	while (rval < filesize) {
		temp = recv(msgsocket, databufin, BUFSIZE);
		rval += temp;
		if (temp  < 0) {
			perror("Error: Unable to read Stream Socket.");
			exit(1);
		}
		printf("Server Recieved: %s\n" databufin)

		// Write to Output File
		fprintf(oufp, "%s", databufin);
	}
	
	// Respond to Client
	if (write(msgsocket, databufout, BUFSIZE) < 0) {
		perror("Error: Unable to write Stream Socket.")
	}
	printf("Server Sent: %s\n", databufout);
	
	// Close File/Connections
	fclose(oufp);
	close(msgsocket);
	close(socket);
	
}




















