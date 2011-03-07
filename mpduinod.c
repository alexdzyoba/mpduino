#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <libmpd-1.0/libmpd/libmpd.h>


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

void signal_handler(int sig)
{
	switch(sig)
	{
		case SIGQUIT:
		case SIGTERM:
			free(host);
			close(fd);
			mpd_free(obj);
			exit(0);
			break;
		default:
			perror("Unhandled signal\n");
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
        printf("Error %i:'%s'\n", errorid, msg);
}
 
void connect2mpd()
{
	struct termios options;
    
	/* Get the current options for the port...*/
    tcgetattr(fd, &options);

    /* Set the baud rates to 9600  */
    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);
	
	/* Create mpd object */
    obj = mpd_new(host, port, NULL); 
	
	/* Set timeout */
    mpd_set_connection_timeout(obj, 10);

	/* Connect to MPD */
    /*mpd_signal_connect2mpd_error(obj,(ErrorCallback)error_callback, NULL);*/
	mpd_connect(obj);

}

int main(int argc, char *argv[])
{
	char buf[1];
	int state;
	
	/* Handle MPD hostname and port number */
	if(argc !=3)
	{
		usage();
		exit(1);
	}

	host = (char *)malloc(strlen(argv[1]));
	strncpy(host, argv[1], strlen(argv[1]));
	port = atoi(argv[2]);
	
	/* Set signal handler */
	signal(SIGQUIT, signal_handler);

	/* Daemonize! */
	if(daemon(0,1))
	{
		perror("Can't daemonize\n");	
	}

	/* Open serial port for reading */
	fd = open("/dev/ttyUSB1",O_RDONLY);
	if(fd == -1)
	{
		perror("Can't open serial port\n");
		exit(1);
	}
	connect2mpd();	
	
	while(1)
	{
		read(fd, buf, 1);
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
