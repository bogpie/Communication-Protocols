/*
*  	Protocoale de comunicatii: 
*  	Laborator 6: UDP
*	client mini-server de backup fisiere
*/

#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "helpers.h"

void usage(char*file)
{
	fprintf(stderr,"Usage: %s ip_server port_server file\n",file);
	exit(0);
}

/*
*	Utilizare: ./client ip_server port_server nume_fisier_trimis
*/
int main(int argc,char**argv)
{
	if (argc!=4)
		usage(argv[0]);
	
	int fd;
	struct sockaddr_in to_station;
	char buf[BUFLEN];


	/*Deschidere socket*/
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	
	if(sock == -1) {
		printf("[CLIENT] Error opening socket\n");
		exit(1);
	}
	
	/* Deschidere fisier pentru citire */
	
	DIE((fd=open(argv[3],O_RDONLY))==-1,"open file");

	/*Setare struct sockaddr_in pentru a specifica unde trimit datele*/
	
	to_station.sin_family = AF_INET;
	to_station.sin_port = htons(atoi(argv[2]));
	int result_value = inet_aton(argv[1], &to_station.sin_addr);
	
	if(result_value == INADDR_NONE) {
		printf("[CLIENT] Invalid input\n");
		exit(1);
	}

	/*
	*  cat_timp  mai_pot_citi
	*		citeste din fisier
	*		trimite pe socket
	*/	

	int read_result;

	while((read_result = read(fd, buf, sizeof(buf)))) {
		if(read_result == -1) {
			perror("[CLIENT] Read error\n");
			exit(1);
		}
		

		int send_result = sendto(sock, buf, read_result, 0, (struct sockaddr*)(&to_station), sizeof(to_station));
	
		if(send_result == -1) {
			perror("[CLIENT] Sendto error\n");
			exit(1);
		}
	}

	/*Inchidere socket*/

	close(sock);

	/*Inchidere fisier*/

	close(fd);

	return 0;
}
