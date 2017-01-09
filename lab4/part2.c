#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mraa/aio.h>
#include <time.h>
#include <getopt.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>


const int B=4275;                 // B value of the thermistor
//const int R0=100000;            // R0 = 100k
//const int pinTempSensor = A5;     // Grove - Temperature Sensor connect to A5
int run_flag = 1;
int stop = 0;
int scale = 0;
int freq = 3;
FILE* ofd;
int sockfd;
int n_port;
char* command[5]={"OFF", "STOP", "START", "SCALE", "FREQ"};

void get_port(void)
{
	struct sockaddr_in serv_addr;
	struct hostent *server;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		fprintf(stderr, "Error! Problem opening socket\n");
		exit(1);
	}
	server = gethostbyname("lever.cs.ucla.edu");
	if (server == NULL) {
	 fprintf(stderr,"Error! Problem finding host\n");
	 exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(16000);
	connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	char c[22] = "Port request 504451468";
	write(sockfd, &c , sizeof(c));
	read(sockfd, &n_port, sizeof(n_port));;
}

void *thread_func(void *arg)
{
	fd_set rfds;
	int retval;
	//Watch stdin (fd 0) to see when it has input.

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);
	FD_SET(sockfd, &rfds);
	while(1)
    {
		retval = select(sockfd + 1, &rfds, NULL, NULL, NULL);
		char buf[10] = {'\0'};
		if (retval == -1)
		   perror("select()");
		else if(FD_ISSET(sockfd, &rfds))
		{
			read(sockfd, &buf, sizeof(buf));
			fprintf(ofd, "%s", buf);
			printf("%s\n", buf);
			if(strcmp(buf, command[0]) == 0)
			{
				run_flag = 0;
				exit(0);
			}
			else if(strcmp(buf, command[1]) == 0)
				stop = 1;
			else if(strcmp(buf, command[2]) == 0)
				stop = 0;
			else if(strncmp(buf, command[3], 5) == 0)
			{
				if(buf[6] == 'C')
					scale = 1;
				else
					scale = 0;
			}
			else if(strncmp(buf, command[4], 4) == 0)
			{	
				if(buf[6] == '\0')
					freq = (buf[5] - '0');
				else
				{
					int i = 5;
					int temp = 0;
					while(buf[i] != '\0')
					{
						temp += temp*10 + (buf[i] - '0');
						i++;
					}
					freq = temp;
				}
			}
			else
				fprintf(ofd, " I");
			fprintf(ofd, "\n");
			fflush(ofd);
		}
	}
	return NULL;
}

int main()
{
	get_port();
	printf("%d\n", n_port);
	struct sockaddr_in serv_addr;
	struct hostent *server;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		fprintf(stderr, "Error! Problem opening socket\n");
		exit(1);
	}
	server = gethostbyname("lever.cs.ucla.edu");
	if (server == NULL) {
	 fprintf(stderr,"Error! Problem finding host\n");
	 exit(0);
	}
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(n_port);
	if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) 
	{
		fprintf(stderr, "Error! Problem connecting to server\n");
		exit(1);
	}

	uint16_t value;
	mraa_aio_context rotary;
	rotary = mraa_aio_init(0);
    ofd = fopen("log_2.txt","w+");

    pthread_t thread;
  	pthread_create(&thread, NULL, thread_func, NULL);
	
	while(run_flag && ofd)
	{
		while(stop)
			if(stop == 0)
				break;

		value = mraa_aio_read(rotary);

	    float R = 1023.0/((float)value)-1.0;
		R = 100000.0*R;
	    float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet;
		if(scale == 0)
			temperature = temperature*9.0/5.0+32;

		char* time_str;
		time_str = malloc(9);
		time_t time_1 = time(NULL);
		struct tm *time_2 = localtime(&time_1);
		strftime(time_str, 9, "%H:%M:%S", time_2);

		if(scale == 0)
		{
			printf("%s %.1f F\n", time_str, temperature);
			fprintf(ofd, "%s %.1f F\n", time_str, temperature);
		}
		else
		{
			printf("%s %.1f C\n", time_str, temperature);
			fprintf(ofd, "%s %.1f C\n", time_str, temperature);	
		}
		fflush(ofd);

		char output[20] = {'\0'};
		sprintf(output,"504451468 TEMP=%.1f\n", temperature);
		write(sockfd, &output, sizeof(output));

		sleep(freq);
	}
	
	if(ofd)
		fclose(ofd);
	mraa_aio_close(rotary);
	return 0;
}
