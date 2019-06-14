// 				XAIME RIVAS REY - RALA CONTROLLER  - xr39@drexel.edu
/*									  NOTE
 * This program is a server socket, listening on Port 8080. It controls the 
 * RALA (Reconfigurable Alford Loop Antenna), by default, the pins used are
 * 26, 20, 19, 16 (default GPIO numbering scheme for RPi 3). As of now, the 
 * states we can control are 1 -4 (directional), 0 (Omni) and 5 (Off).
 *
 * TODO
 * Start up this as a daemon when booting up
 * Add more options for directional mode
 */

// ==========================================================================
// ============================== HOUSE KEEPING =============================
// ==========================================================================
// Libraries
#include <stdlib.h>		// atoi, atof
#include <stdio.h>		// printf
#include <string.h>		//strcpy 
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define NSTATES 5
#define NLEDS 4

// Functions
void error (const char *msg);
bool dostuff(int newsockfd);
void onExit(int dummy);

// Variables
char state[1] = {'0'};     // Initilize with omni
// Rpi model 3B
int sPin[4] = {26,20,19,16}; // state 1, 2, 3 and 4, GPIO pins on the Pi3 Model B 
int code[6][4] = {  
   {1, 1, 1, 1} ,    // State 0 - Omni
   {1, 0, 0, 0} ,    // State 1
   {0, 1, 0, 0} ,    // State 2
   {0, 0, 1, 0} ,    // State 3
   {0, 0, 0, 1} ,    // State 4
   {0, 0, 0, 0}	     // Turned off
};

// ==========================================================================
// ================================ MAIN ====================================
// ==========================================================================

int main(int argc, char *argv[]) 
{	
	signal(SIGINT, onExit); // If Ctrl + C pressed, turn off the antenna
	signal(SIGCHLD, SIG_IGN);
	int sockfd, newsockfd, portno, pid;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	if (argc < 2)
	{
		fprintf(stderr, "WARNING, no port provided, using 8080\n");
		portno = 8080;
	}else
	{
		portno = atoi(argv[1]);
	}

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {error("ERROR opening socket");}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	if(bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr)) < 0){ error("ERROR on binding");}
	
	// Listen for clients (blocking call)
	listen( sockfd, 5);

	clilen = sizeof(cli_addr);

	while (true){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        
        if (newsockfd >= 0){

            pid = fork();

            if (pid < 0) error("ERROR on fork");
            else if (pid == 0){
                close(sockfd);
                
                // Child process keeps receiving until flag becomes 0
                dostuff(newsockfd);
                close(newsockfd);
                }
	    }
    }

    return 0;
}


// ==========================================================================
// ============================= FUNCTIONS ==================================
// ==========================================================================
// ============================== DOSTUFF ===============================
// Do stuff when a socket connection is openned and a message is received
bool dostuff(int newsockfd){
	bool flag = 1;
	char buffer[256];
	int n;

	bzero(buffer,256);
	n = read(newsockfd, buffer, 1);
	if (n < 0) error("ERROR reading from socket");

	printf("New state = %d\n", atoi(&buffer[0]));
	n = write(newsockfd, "State updated!", 14);
    if (n < 0) error("ERROR writing to socket");

	// If we want the communication to stop. this is the place, whatever
	// condition triggers that should be used here and make flag = 0
	return 0;
}

// ================================ ERROR ===================================
// Error handling, probably should terminate the program at this point, since 
// we want the controller to work without any human interaction needed
void error (const char *msg){
	perror(msg);
	exit(1);
}

// ================================ ONEXIT ==================================
// Turn antenna off on exit 
void onExit (int dummy){  // Need the int dummy since the call provides that input
	(void) dummy;

	exit(0);
}
