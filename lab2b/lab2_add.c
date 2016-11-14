#define _GNU_SOURCE

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
#include <time.h>
#include <mcrypt.h>

long long counter = 0;
int opt_yield = 0;
int opt_sync = 0;
char sync_op;
pthread_mutex_t mutex;
int spin = 0;
int n_thread = 1;
int n_iter = 1;

void add(long long *pointer, long long value) {
        long long sum = *pointer + value;
        if (opt_yield)
                sched_yield();
        *pointer = sum;
}

void mutex_func(void)
{
	int j;
	for(j = 0; j < n_iter; j++)
	{
		pthread_mutex_lock(&mutex);
        add(&counter, 1);
        pthread_mutex_unlock(&mutex);
    }
	for(j = 0; j < n_iter; j++)
	{
		pthread_mutex_lock(&mutex);
        add(&counter, -1);
        pthread_mutex_unlock(&mutex);
    }
}

void spin_func(void)
{
	int j;
	for(j = 0; j < n_iter; j++)
	{
		while (__sync_lock_test_and_set(&spin, 1));
        add(&counter, 1);
        __sync_lock_release(&spin);
    }
	for(j = 0; j < n_iter; j++)
	{
		while (__sync_lock_test_and_set(&spin, 1));
        add(&counter, -1);
        __sync_lock_release(&spin);
    }
}

void cas_func(void)
{
	int j;
	long long pre = 0;
	long long sum = 0;
	for(j = 0; j < n_iter; j++)
	{
		do {
          pre = counter;
          sum = pre + 1;
          if (opt_yield) 
          	pthread_yield();
        } while (__sync_val_compare_and_swap(&counter, pre, sum) != pre);
    }
	for(j = 0; j < n_iter; j++)
	{
		do {
          pre = counter;
          sum = pre - 1;
          if (opt_yield)
          	pthread_yield();
        } while (__sync_val_compare_and_swap(&counter, pre, sum) != pre);
    }
}


void *thread_func(void *arg)
{
	if(opt_sync == 0)
	{
		int j;
		for(j = 0; j < n_iter; j++)
			add(&counter, 1);
		for(j = 0; j < n_iter; j++)
			add(&counter, -1);
	}
	else
	{
		switch (sync_op) 
		{
	      	case 'm':
	        mutex_func();
	        break;
	      	case 's':
	      	spin_func();
	        break;
	        case 'c':
	        cas_func();
	        break;
    	}
    }
	return NULL;
}

int main(int argc, char **argv)
{
	static struct option long_options[] = {
	    {"threads", required_argument, NULL, 't'},
	    {"iterations", required_argument, NULL, 'i'},
	    {"yield", no_argument, NULL, 'y'},
	    {"sync", required_argument, NULL, 's'}
	  };
	int ret = 0;
	while(1)
    {
      ret = getopt_long(argc, argv, "", long_options, NULL);
      if(ret == -1)
		break;
      switch(ret)
		{
			case 't':
				n_thread = atoi(optarg);
				break;
			case 'i':
				n_iter = atoi(optarg);
				break;
			case 'y':
				opt_yield = 1;
				break;
			case 's':
				opt_sync = 1;
				sync_op = *optarg;
				break;
	  	}
	}

	struct timespec t_start;
  	clock_gettime(CLOCK_MONOTONIC, &t_start);
  	if (sync_op == 'm') {
    pthread_mutex_init(&mutex, NULL);
	}

  	pthread_t *threads;
  	threads = (pthread_t*)malloc(n_thread * sizeof(pthread_t));
	int i;
  	for(i = 0;  i < n_thread; i++)
  		pthread_create(&threads[i], NULL, thread_func, NULL);

  	for(i = 0;  i < n_thread; i++)
  		pthread_join(threads[i], NULL);
	struct timespec t_end;
  	clock_gettime(CLOCK_MONOTONIC, &t_end);
  	
  	char *test;
  	if(opt_sync == 0)
  	{
		if(opt_yield)
			test = "add-yield-none";
		else
			test = "add-none";
	}
	else
	{
		switch (sync_op) 
		{
	      	case 'm':
	        if(opt_yield)
				test = "add-yield-m";
			else
				test = "add-m";
	        break;
	      	case 's':
	        if(opt_yield)
				test = "add-yield-s";
			else
				test = "add-s";
	        break;
	        case 'c':
	        if(opt_yield)
				test = "add-yield-c";
			else
				test = "add-c";
	        break;
		}
	}

  	long long t1 = (long long)(t_start.tv_sec * 1000000000 + t_start.tv_nsec);
  	long long t2= (long long)(t_end.tv_sec * 1000000000 + t_end.tv_nsec);
  	long long t_total = t2 - t1;
  	long long n_ops = 2 * n_thread * n_iter;
  	int t_ave = t_total / n_ops;
  	printf("%s,%d,%d,%lld,%lld,%d,%lld\n", test, n_thread, n_iter, n_ops, t_total, t_ave, counter);
  	if(counter != 0)
  	{
  		//fprintf(stderr, "Error! Counter not equal to 0\n");
  		exit(1);
  	}
  	exit(0);
  	return 0;
}