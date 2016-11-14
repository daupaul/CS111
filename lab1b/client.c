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
#include <netdb.h>
#include <pthread.h>
#include <mcrypt.h>

struct termios saved_attributes;
char EOT = 0x04;
char CR = 0x0D;
char LF = 0x0A;
char ETX = 0x03;
int n_port;
int p_flag = 0;
int l_flag = 0;
int e_flag = 0;
int log_fd = -1;
int sockfd;
MCRYPT td;


void reset_input_mode(void)
{
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode(void)
{
  struct termios tattr;
  tcgetattr(STDIN_FILENO, &saved_attributes);
  atexit(reset_input_mode);

  tcgetattr(STDIN_FILENO, &tattr);
  tattr.c_lflag &= ~(ICANON|ECHO);
  
  tattr.c_cc[VMIN] =1 ;
  tattr.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

void *thread_func(void *arg)
{
  char buf;
  int status;
  while(1)
    {
      status = read(sockfd, &buf, 1);
      if(status <= 0)
	exit(1);
      if(l_flag == 1)
      {
        write(log_fd, "RECEIVED 1 bytes: ", 18);
        write(log_fd, &buf, 1);
	if(buf != CR && buf != LF)
	  write(log_fd, "\n", 1);
      }
      if(e_flag == 1)
          mdecrypt_generic(td, &buf,1);
      write(STDOUT_FILENO, &buf, 1);
    }
  return NULL;
}
  
int main(int argc, char **argv)
{
  static struct option long_options[] = {
    { "port", required_argument, NULL, 'a'},
    { "log" , required_argument, NULL, 'b'},
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
	  		case 'b':
	  		l_flag = 1;
	  		log_fd = creat(optarg, 0666);
	  		break;
        case 'c':
        e_flag = 1;
        break;
	  	}


	}
  set_input_mode ();
  struct sockaddr_in serv_addr;
  struct hostent *server;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
  {
  	fprintf(stderr, "Error! Problem opening socket\n");
  	exit(1);
  }
  server = gethostbyname("localhost");
  if (server == NULL) {
     fprintf(stderr,"Error! Problem finding host\n");
     exit(0);
  }
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(n_port);

  if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      fprintf(stderr, "Error! Problem connecting to server\n");
      exit(1);
   }
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
  pthread_t thread;
  pthread_create(&thread, NULL, thread_func, NULL);

  char c;
  while(1)
    {
      int temp = read(STDIN_FILENO, &c, 1);
      if(temp <= 0)
	{
    shutdown(sockfd, 2);
	  exit(1);
	}
      if(c == EOT)
	{
    shutdown(sockfd, 2);
	  exit(0);
	}
      else if(c == CR || c == LF)
	{
	  write(STDOUT_FILENO, &CR , 1);
	  write(STDOUT_FILENO, &LF, 1);
	  c = LF;
    if(e_flag == 1)
      mcrypt_generic(td, &c, 1);
    write(sockfd, &c , 1);
    if(l_flag == 1)
    {
      write(log_fd, "SENT 1 bytes: ", 14);
      write(log_fd, &c, 1);
      if(c != CR && c != LF)
	write(log_fd, "\n", 1);
    }

	}
      else
	{
    write(STDOUT_FILENO, &c, 1);
    if(e_flag == 1)
      mcrypt_generic(td, &c, 1);
	  write(sockfd, &c, 1);
    if(l_flag == 1)
    {
      write(log_fd, "SENT 1 bytes: ", 14);
      write(log_fd, &c, 1);
      write(log_fd, "\n", 1);
    }
	}
    }
  return 0;
}
