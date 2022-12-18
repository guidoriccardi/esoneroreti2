/*
        demo-udp-03: udp-send: a simple udp client
	send udp messages
	This sends a sequence of messages (the # of messages is defined in MSGS)
	The messages are sent to a port defined in SERVICE_PORT 

        usage:  udp-send

        Paul Krzyzanowski
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>

#define SERVICE_PORT	21234
#define BUFLEN 2048
#define MSGS 5	/* number of messages to send */

int main(void)
{
	struct sockaddr_in myaddr, remaddr;
	struct hostent *hp;
	int fd, i, slen=sizeof(remaddr);
	char buf[BUFLEN];	/* message buffer */
	int recvlen;		/* # bytes in acknowledgement message */
	char *server [BUFLEN];	/* nome del server */

	/* create a socket */

	if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
		printf("socket created\n");

	/* bind it to all local addresses and pick any port number */

	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(0);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		perror("bind failed");
		return 0;
	}       
	int num_port;
	printf("Inserisci numero di porta\n");
	scanf("%d",&num_port);
	memset((char *) &remaddr, 0, sizeof(remaddr));
	remaddr.sin_family = AF_INET;
	remaddr.sin_port = htons(num_port);

	/* now define remaddr, the address to whom we want to send messages */
	/* For convenience, the host address is expressed as a numeric IP address */
	/* that we will convert to a binary format via inet_aton */
	//inserimento dell'ip

	printf("Inserisci IP:\n");
	scanf("%s",server);

	 hp = gethostbyname(server);
        if (!hp) {
                fprintf(stderr, "could not obtain address of %s\n", server);
                return 0;
        }
	
	struct sockaddr_in servaddr;    /* server address */
	/* put the host's address into the server address structure */
	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);



	if (inet_aton(server, &remaddr.sin_addr)==-1) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	/* now let's send the messages */

	for (i=0; i < MSGS; i++) {
		printf("Sending packet %d to %s port %d\n", i, server, num_port);
		sprintf(buf, "This is packet %d", i);
		if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) {
			perror("sendto");
			exit(1);
		}
		/* now receive an acknowledgement from the server */
		recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
                if (recvlen >= 0) {
                        buf[recvlen] = 0;	/* expect a printable string - terminate it */
                        printf("received message: \"%s\"\n", buf);
                }
	}
	close(fd);
	return 0;
}
