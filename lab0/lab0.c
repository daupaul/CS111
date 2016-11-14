#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void my_handler(int signum)
{
  fprintf(stderr, "Error! Segmentation fault! Exit number: %d \n", signum);
  _exit(3);
}

int main(int argc, char *argv[])
{
  if(argc == 1)
    exit(0);
  struct option long_options[]=
    {
      {"input", required_argument, NULL, 'a'},
      {"output", required_argument, NULL, 'b'},
      {"catch", no_argument, NULL, 'c'},
      {"segfault", no_argument, NULL, 'd'},
      {0,0,0,0}
    };
  int ret = 0;
  int seg_flag, i_flag, o_flag= 0;
  while (1)
  {
    ret = getopt_long(argc, argv, "", long_options, NULL);
    if(ret == -1)
      break;
    switch(ret)
      {
      case 'a':
	i_flag = open(optarg, O_RDONLY);
	if (i_flag >= 0)
	    dup2(i_flag, 0);
	else
	  {
	    fprintf(stderr, "Error! Unable to open input file \n");
	    perror("Input file error");
	    exit(1);
	  }
	break;
      case 'b':
	o_flag = creat(optarg, 0666);
	if(o_flag >= 0)
	  dup2(o_flag, 1);
	else
	  {
	    fprintf(stderr, "Error! Unable to create output file \n");
	    perror("Output file error");
	    exit(2);
	  }
	break;
      case 'c':
	if(argc == 2)
	  exit(0);
	signal(SIGSEGV, my_handler);
	break;
      case 'd':
	seg_flag = 1;
	break;
      default:
	exit(0);
      }
    
  }
  
  if(seg_flag == 1)
    {
      char* err = NULL;
      *err = 's';
    }

  char temp[1];
  while (read(0, temp, 1) > 0)
    {
      write(1, temp, 1);
    }
  exit(0);
}
