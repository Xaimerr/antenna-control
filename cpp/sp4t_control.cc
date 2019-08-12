// 				XAIME RIVAS REY - SP4T CONTROLLER - xr39@drexel.edu
/*                                    NOTE
 * This program is a server socket, listening on Port 8081. It controls the 
 * SP4T RF Switch on the grid. By default, the pins used are 17 (A) and 27 (B)
 * (default GPIO numbering scheme for RPi 3). As of now, the input for this are
 * 1 - 4, each number corresponding to the RF port that we want to select.
 *
 * TODO
 * Start up this as a daemon when booting up
 */

// ==========================================================================
// ============================== HOUSE KEEPING =============================
// ==========================================================================
// Libraries
#include <stdlib.h>		// atoi, atof
#include <stdio.h>		// printf
#include <wiringPi.h>	// GPIO
#include <string.h>		//strcpy 
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define NSTATES 4 
#define NPINS 2

// Functions
void setState(int state);
void error (const char *msg);
bool dostuff(int newsockfd);
void onExit(int dummy);
void setup();

// Variables
char state[1] = {'0'};     // Initilize with RF1 port enabled
int sPin[4] = {17, 27}; // Pins A and B for RFSwitch
short code[4][2] = {
	{0, 0}, // RF 1
	{1, 0},	// RF 2
	{0, 1}, // RF 3
	{1, 1}, // RF 4
};

// ==========================================================================
// ================================ MAIN ====================================
// ==========================================================================

int main(int argc, char *argv[]) 
{	
	signal(SIGINT, onExit); // If Ctrl + C pressed, turn off the antenna
	wiringPiSetupGpio () ; // Specify the pin definition to use (BCM)
	signal(SIGCHLD, SIG_IGN);
	int sockfd, newsockfd, portno, pid;
	socklen_t clilen;
	struct sockaddr_in serv_addr, cli_addr;

	if (argc < 2)
	{
		portno = 8081;
		fprintf(stderr, "WARNING, no port provided, using %d\n", portno);
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

	setup();
  	
	while (true){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0) error("ERROR on accept");

		pid = fork();

		if (pid < 0) error("ERROR on fork");
		else if (pid == 0){
			bool flag = 1;
			close(sockfd);
			
			// Child process keeps receiving until flag becomes 0
			while(flag) {
				flag = dostuff(newsockfd);
			}

			exit(0);
		} else close(newsockfd);
	}
  
	return 0;
}


// ==========================================================================
// ============================= FUNCTIONS ==================================
// ==========================================================================
// ============================== SET STATE ================================
// Set the RFSwitch to the desired output
void setState(int state)
{
  // Chech the state is within the range (0 - (NSTATES-1))
  if (state >= 0 && state < NSTATES) // 
  {
  	for (int i = 0; i < NPINS; i++)
	{
		if (code[state][i] == 1){
			digitalWrite(sPin[i], HIGH);
		}else{
			digitalWrite(sPin[i], LOW);
		}
	}
  }
  else 
  {
    printf("\nERROR: Incorrect RF port (1 - 4)");
  }
}

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
	setState(atoi(&buffer[0])-1); // -1 because RF1 has code 0 and so on
	n = write(newsockfd, "State updated!", 14);
	if (n < 0) error("ERROR writing to socket");

	// If we want the communication to stop. this is the place, whatever
	// condition triggers that should be used here and make flag = 0
	return flag;
}

// ================================ ERROR ===================================
// Error handling, probably should terminate the program at this point, since 
// we want the controller to work without any human interaction needed
void error (const char *msg){
	perror(msg);
	exit(1);
}

// ================================ ONEXIT ==================================
// Default to RF1 
void onExit (int dummy){  // Need the int dummy since the call provides that input
	(void) dummy;

	setState(0);
	exit(0);
}

// ================================= SETUP ==================================
// Set the GPIO up and enable RF1
void setup() {
  // Enable the pins as outputs
	for (int i = 0; i < NPINS; i++)
	{
		pinMode(sPin[i], OUTPUT);
	}

	setState(0);
}
