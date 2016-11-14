#include<stdio.h>
#include<errno.h>
#include<unistd.h>
#include<getopt.h>
#include<signal.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<termios.h>
#include<assert.h>
#include<stdbool.h>
#include<pthread.h>
#include<sys/wait.h>

struct termios saved_attributes;
char EOT = 0x04;
char CR = 0x0D;
char LF = 0x0A;
char ETX = 0x03;
int pID;
int s_flag = 0;
int s_eof = 0;

void my_sigpipe(int signum)
{
  exit(1);
}

void reset_input_mode(void)
{
  if(s_flag == 1)
  {
  	int RC;
  	waitpid(pID, &RC, 0);
  	if(WIFEXITED(RC))
  	  fprintf(stderr, "Shell exits with %d\n", WEXITSTATUS(RC));
  	else
	  fprintf(stderr, "Shell exits with signal %d\n", WTERMSIG(RC));
  }
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode(void)
{
  struct termios tattr;
  char *name;
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
  int *pipe = (int *)arg;
  char buf;
  int status;
  while(1)
    {
      status = read(*pipe, &buf, 1);
      if(status < 0)
	break;
      else if(buf == EOT || status == 0)
	{
	  s_eof = 1;
	  break;
	}
      else
	write(STDOUT_FILENO, &buf, 1);
     }
  return NULL;
}
  
int main(int argc, char **argv)
{
  static struct option long_options[] = {
    { "shell", no_argument, NULL, 's'}
  };
  int ret = 0;
  while(1)
    {
      ret = getopt_long(argc, argv, "", long_options, NULL);
      if(ret == -1)
	break;
      switch(ret)
	{
	case 's':
	  s_flag = 1;
	  break;
	}
    }
  set_input_mode ();
  if(s_flag == 0)
    {
      char c;
      while(1)
        {
          if(read(STDIN_FILENO, &c, 1) == -1)
	    {
	      fprintf(stderr, "Error! reading from input failed");
	      exit(1);
	    }
          if(c == EOT)
	    break;
          else if(c == CR || c == LF)
	    {
	      write(STDOUT_FILENO, &CR, 1);
	      write(STDOUT_FILENO, &LF, 1);
	      continue;
	    }
          else
	    write(STDOUT_FILENO, &c, 1);
        }
    }
  else
    {
      int to_child[2];
      int from_child[2];
      if(pipe(to_child) < 0 || pipe(from_child) < 0)
	{
	  fprintf(stderr, "Error! pipe failed!");
	  exit(1);
	}
      pID = fork();
      if(pID < 0)
	{
	  fprintf(stderr, "fork() failed!\n");
	  exit(1);
	}
      if(pID == 0)
	{
      close(to_child[1]);
	  close(from_child[0]);
	  dup2(to_child[0], STDIN_FILENO);
	  dup2(from_child[1], STDOUT_FILENO);
	  close(to_child[0]);
	  close(from_child[1]);

	  char *exec_shell[2];
	  char exec_name[] = "/bin/bash";
	  exec_shell[0] = exec_name;
	  exec_shell[1] = NULL;
	  if(execvp(exec_name, exec_shell) < 0)
	    {
	      fprintf(stderr, "Error! shell failed!");
	      exit(1);
	    }
	}
      else if(pID > 0)
	{
	  signal(SIGPIPE, my_sigpipe);
	  close(to_child[0]);
	  close(from_child[1]);
	  pthread_t thread;
	  pthread_create(&thread, NULL, thread_func, &from_child[0]);

	  char c;
	  while(1)
	    {
	      int temp = read(STDIN_FILENO, &c, 1);
	      if(temp == -1)
		{
		  fprintf(stderr, "Error! reading from child failed!");
		  exit(1);
		}
	      if(s_eof == 1)
		{
		  exit(1);
		}
	      if(c == EOT || temp == 0)
		{
		  close(to_child[1]);
		  close(from_child[0]);
		  kill(pID, SIGHUP);
		  exit(0);
		}
	      else if(c == ETX)
		{
		  kill(pID, SIGINT);
		}
	      else if(c == CR || c == LF)
		{
		  write(STDOUT_FILENO, &CR , 1);
		  write(STDOUT_FILENO, &LF, 1);
		  write(to_child[1], &LF, 1);
		}
	      else
		{
		  write(STDOUT_FILENO, &c, 1);
		  write(to_child[1], &c, 1);
		}
	    }
	}
    }
  return 0;
}
