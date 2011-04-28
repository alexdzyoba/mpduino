#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <libmpd-1.0/libmpd/libmpd.h>

#include <syslog.h>

#define CONNECTION_TIMEOUT 0

#define VOLDOWN	2
#define PREV 	3
#define STOP 	4
#define PLAY 	5
#define NEXT 	6
#define VOLUP 	7

/* File descriptor for serial port */
int fd;

/* MPD object */
MpdObj *obj = NULL;	

/* MPD hostname */
char *host;

/* MPD port */
int port;

/* Priority for syslog messages */
int priority = LOG_CONS|LOG_USER;


void signal_handler(int sig)
{
	switch(sig)
	{
		case SIGQUIT:
		case SIGTERM:
			syslog(priority, "Caught SIGTERM. Exiting...");
			free(host);
			close(fd);
			mpd_free(obj);
			syslog(priority, "halted");
			exit(0);
			break;
		default:
			syslog(priority, "Caught unhandled signal\n");
			break;
	}
	return;
}

void usage()
{
	printf("Usage: mpduinod <HOST> <PORT>\n");
	return;
}

void error_callback(MpdObj *mi,int errorid, char *msg, void *userdata)
{
        syslog(priority, "Error %i:'%s'\n", errorid, msg);
}

void configure_serial_port()
{
	struct termios options;
    
	/* Get the current options for the port...*/
    tcgetattr(fd, &options);

    /* Set the baud rates to 9600  */
    if(cfsetispeed(&options, B9600) || cfsetospeed(&options, B9600))
		syslog(priority, "Error configuring port");

	return;
}
 
void connect2mpd()
{
	/* Create mpd object */
    obj = mpd_new(host, port, NULL); 
	if(obj == NULL)
		syslog(priority, "Error: can't create mpd object");
	
	/* Set timeout */
    mpd_set_connection_timeout(obj, 10);

	/* Connect to MPD */
	mpd_signal_connect_error(obj,(ErrorCallback)error_callback, NULL);
	if(mpd_connect(obj))
		syslog(priority, "Error: can't connect to mpd server");
	else
		syslog(priority, "Successfully connected to mpd server %s at %d", host, port);
}

int main(int argc, char *argv[])
{
	char buf[1];
	int state;
	
	/* Open syslog	*/
	openlog("mpduinod", LOG_CONS, LOG_USER);
			
	/* Set signal handler */
	signal(SIGQUIT, signal_handler);
	
	/* Handle MPD hostname, port number and serial port*/
	if(argc !=4)
	{
		usage();
		exit(1);
	}

	host = (char *)malloc(strlen(argv[1]));
	strncpy(host, argv[1], strlen(argv[1]));
	port = atoi(argv[2]);

	/* Daemonize! */
	if(daemon(0,1))
		syslog(priority, "Can't daemonize\n");	

	/* Open serial port for reading */
	configure_serial_port();
	fd = open(argv[3], O_RDONLY);
	if(fd == -1)
	{
		syslog(priority, "Can't open serial port\n");
		exit(1);
	}
	

	/* Enter infinite loop */	
	while(1)
	{
		read(fd, buf, 1);
		syslog(priority, "Received byte %d", buf[0]);
		
		/* Establish connection to mpd	*/
		if(!mpd_check_connected(obj))
		{
			syslog(priority, "Connection not established: connecting");
			connect2mpd();
		}

		switch(buf[0])
		{
			case VOLDOWN	: 	mpd_status_set_volume(obj, mpd_status_get_volume(obj)-5); 	break;
			case PREV		:	mpd_player_prev(obj);	break;
			case STOP 		:	mpd_player_stop(obj);	break;	
			case PLAY 		:	{ 
									state = mpd_player_get_state(obj);
									switch(state)
									{
										case MPD_PLAYER_PAUSE: 
										case MPD_PLAYER_STOP:	mpd_player_play(obj); 	break;
										case MPD_PLAYER_PLAY:	mpd_player_pause(obj);	break;
									}
									break;
								}
			case NEXT 		:	mpd_player_next(obj);	break;
			case VOLUP 		:	mpd_status_set_volume(obj, mpd_status_get_volume(obj)+5);	break;
		}
	}
}
