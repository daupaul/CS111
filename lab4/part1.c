#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <mraa/aio.h>
#include <time.h>
#include <fcntl.h>

const int B=4275;                 // B value of the thermistor
//const int R0=100000;            // R0 = 100k
//const int pinTempSensor = A0;     // Grove - Temperature Sensor connect to A5
sig_atomic_t volatile run_flag = 1;

void do_when_interrupted(int sig)
{
	if(sig == SIGINT)
		run_flag = 0;
}
 
int main()
{
	uint16_t value;
	mraa_aio_context rotary;
	rotary = mraa_aio_init(0);
	FILE* ofd;
	ofd = fopen("log_1.txt","w+");
	while(run_flag)
	{
		value = mraa_aio_read(rotary);

	    float R = 1023.0/((float)value)-1.0;
		R = 100000.0*R;
	    float temperature=1.0/(log(R/100000.0)/B+1/298.15)-273.15;//convert to temperature via datasheet;
		temperature = temperature*9.0/5.0+32;

		char* time_str;
		time_str = malloc(9);
		time_t time_1 = time(NULL);
		struct tm *time_2 = localtime(&time_1);
		strftime(time_str, 9, "%H:%M:%S", time_2);

		printf("%s %.1f\n", time_str, temperature);
		fprintf(ofd, "%s %.1f\n", time_str, temperature);
		fflush(ofd);

		sleep(1);
	}
	mraa_aio_close(rotary);
	return 0;
}
