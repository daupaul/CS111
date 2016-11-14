#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <mcrypt.h>

struct termios saved_attributes;
char EOT = 0x04;
char CR = 0x0D;
char LF = 0x0A;
char ETX = 0x03;
int pID;
int n_port;
int p_flag = 0;
int e_flag = 0;
int s_eof = 0;
int sockfd, newsockfd;
int to_child[2];
int from_child[2];
MCRYPT td;

void my_sigpipe(int signum)
{
	shutdown(sockfd, 2);
	shutdown(newsockfd, 2);
	close(to_child[1]);
	close(from_child[0]);
    exit(2);
}

void *thread_func(void *arg)
{
  char buf;
  int status;
  while(1)
    {
      status = read(from_child[0], &buf, 1);
      if(status < 0)
		break;
	  else if(status == 0)
	  {
		s_eof = 1;
		break;
	  }

      else
      {
      	if(e_flag == 1)
      		mcrypt_generic(td, &buf, 1);
		write(STDOUT_FILENO, &buf, 1);
   	  } 
    }
  return NULL;
}
  
int main(int argc, char **argv)
{
  static struct option long_options[] = {
    { "port", required_argument, NULL, 'a'},
    { "encrypt" , no_argument, NULL, 'c'}

  };
  int ret = 0;
  while(1)
    {
      ret = getopt_long(argc, argv, "", long_options, NULL);
      if(ret == -1)
		break;
      switch(ret)
		{
			case 'a':
			p_flag = 1;
	  		n_port = atoi(optarg);
	  		break;
	  		case 'c':
        	e_flag = 1;
        	break;
	  	}
    }
  
  if(pipe(to_child) < 0 || pipe(from_child) < 0)
	{
	  fprintf(stderr, "Error! pipe failed!");
	  exit(1);
	}
	struct sockaddr_in serv_addr, cli_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd < 0) {
      fprintf(stderr, "Error! Problem opening socket\n");
      exit(1);
   }
   	bzero((char *) &serv_addr, sizeof(serv_addr));
  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_addr.s_addr = INADDR_ANY;
  	serv_addr.sin_port = htons(n_port);
  	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("Error! Problem linking socket\n");
      exit(1);
   }
   listen(sockfd,5);
   socklen_t clilen = sizeof(cli_addr);
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
   if(e_flag == 1)
   {
	int i;
    char *key;
    char password[20];
    char *IV;
    int keysize=16; /* 128 bits */
    key = calloc(1, keysize);
    int my_key = open("my.key", O_RDONLY);
    if(my_key < 0)
    {
      fprintf(stderr, "Error! Problem loading my.key\n");
      exit(1);
    }
    read(my_key, &password, strlen(password));
    memmove( key, password, strlen(password));
    td = mcrypt_module_open("twofish", NULL, "cfb", NULL);
    if (td==MCRYPT_FAILED) {
       exit(1);
    }
    IV = malloc(mcrypt_enc_get_iv_size(td));
  /* Put random data in IV. Note these are not real random data,
   * consider using /dev/random or /dev/urandom.
   */
    /*  srand(time(0)); */
    for (i=0; i< mcrypt_enc_get_iv_size( td); i++) {
      IV[i]=rand();
    }
    i=mcrypt_generic_init( td, key, keysize, IV);
    if (i<0) {
       mcrypt_perror(i);
       exit(1);
    }
   }
	pID = fork();
	if(pID < 0)
	{
	  fprintf(stderr, "Error! fork() failed!\n");
	  exit(1);
	}
	if(pID == 0)
	{
	  close(to_child[1]);
	  close(from_child[0]);
	  dup2(to_child[0], STDIN_FILENO);
	  dup2(from_child[1], STDOUT_FILENO);
	  dup2(from_child[1], STDERR_FILENO);
	  close(to_child[0]);
	  close(from_child[1]);

	  char *exec_shell[2];
	  char exec_name[] = "/bin/bash";
	  exec_shell[0] = exec_name;
	  exec_shell[1] = NULL;
	  if(execvp(exec_name, exec_shell) < 0)
	    {
	      fprintf(stderr, "Error! Executing shell failed!");
	      exit(1);
	    }
	}
	  else if(pID > 0)
	{
	  signal(SIGPIPE, my_sigpipe);
	  close(to_child[0]);
	  close(from_child[1]);
	  dup2(newsockfd, STDIN_FILENO);
	  dup2(newsockfd, STDOUT_FILENO);
	  dup2(newsockfd, STDERR_FILENO);
	  close(newsockfd);
	  pthread_t thread;
	  pthread_create(&thread, NULL, thread_func, NULL);

	  char c;
	  while(1)
	    {
	      int temp = read(STDIN_FILENO, &c, 1);
	      if(temp <= 0)
			{
			  shutdown(sockfd, 2);
			  shutdown(newsockfd, 2);
			  close(to_child[1]);
			  close(from_child[0]);
			  exit(1);
			  break;
			}
		  if(s_eof == 1)
			{
				shutdown(sockfd, 2);
			    shutdown(newsockfd, 2);
				close(to_child[1]);
				close(from_child[0]);
				exit(2);
				break;
			}
		  else
			{
			  if(e_flag == 1)
			  	mdecrypt_generic(td, &c, 1);
		      write(to_child[1], &c, 1);
		    }
	}
	}
       return 0;
}
