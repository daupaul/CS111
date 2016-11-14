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
#include "SortedList.h"

int opt_yield = 0;
int opt_sync = 0;
char sync_op;
pthread_mutex_t mutex;
int spin = 0;
int n_thread = 1;
int n_iter = 1;
SortedListElement_t* elements;
SortedListElement_t head;

void *thread_func(void *arg)
{
	int *offsets = (int *)arg;
	if(opt_sync == 0)
	{
		int i;
		for(i = 0; i < n_iter; i++)
			SortedList_insert(&head, &elements[*offsets + i]);
	}
	else
	{	
		int i;
		for(i = 0; i < n_iter; i++)
		{
			switch (sync_op) 
			{
		      	case 'm':
		   		pthread_mutex_lock(&mutex);
		   		SortedList_insert(&head, &elements[*offsets + i]);
		   		pthread_mutex_unlock(&mutex);
		        break;
		      	case 's':
		        while (__sync_lock_test_and_set(&spin, 1));
			    SortedList_insert(&head, &elements[*offsets + i]);
			    __sync_lock_release(&spin);
		        break;
	    	}
	    }
    }
    int check;
    if(opt_sync == 0)
		{
			check = SortedList_length(&head);
			if(check < 0)
			{
				fprintf(stderr, "Error! Corrupted linked list\n");
				exit(1);
			}
		}
	else
	{	
		switch (sync_op) 
		{
	      	case 'm':
	   		pthread_mutex_lock(&mutex);
			check = SortedList_length(&head);
			if(check < 0)
			{
				fprintf(stderr, "Error! Corrupted linked list\n");
				exit(1);
			}
	   		pthread_mutex_unlock(&mutex);
	        break;
	      	case 's':
	        while (__sync_lock_test_and_set(&spin, 1));
			check = SortedList_length(&head);
			if(check < 0)
			{
				fprintf(stderr, "Error! Corrupted linked list\n");
				exit(1);
			}
		    __sync_lock_release(&spin);
	        break;
    	}
    }

    SortedListElement_t *temp;
	if(opt_sync == 0)
	{
		int i;
		for(i = 0; i < n_iter; i++)
		{
			temp = SortedList_lookup(&head, elements[*offsets + i].key);
			if(temp == NULL)
			{
				fprintf(stderr, "Error! Corrupted linked list\n");
				exit(1);
			}
			check = SortedList_delete(temp);
			if(check == 1)
			{
				fprintf(stderr, "Error! Corrupted linked list\n");
				exit(1);
			}
		}
	}
	else
	{	
		int i;
		for(i = 0; i < n_iter; i++)
		{
			switch (sync_op) 
			{
		      	case 'm':
		   		pthread_mutex_lock(&mutex);
		   		temp = SortedList_lookup(&head, elements[*offsets + i].key);
		   		if(temp == NULL)
				{
					fprintf(stderr, "Error! Corrupted linked list\n");
					exit(1);
				}
				check = SortedList_delete(temp);
				if(check == 1)
				{
					fprintf(stderr, "Error! Corrupted linked list\n");
					exit(1);
				}
		   		pthread_mutex_unlock(&mutex);
		        break;
		      	case 's':
		        while (__sync_lock_test_and_set(&spin, 1));
			    temp = SortedList_lookup(&head, elements[*offsets + i].key);
			    if(temp == NULL)
				{
					fprintf(stderr, "Error! Corrupted linked list\n");
					exit(1);
				}
				check = SortedList_delete(temp);
				if(check == 1)
				{
					fprintf(stderr, "Error! Corrupted linked list\n");
					exit(1);
				}
			    __sync_lock_release(&spin);
		        break;
	    	}
	    }
    }


	return NULL;
}

int main(int argc, char **argv)
{
	static struct option long_options[] = {
	    {"threads", required_argument, NULL, 't'},
	    {"iterations", required_argument, NULL, 'i'},
	    {"yield", required_argument, NULL, 'y'},
	    {"sync", required_argument, NULL, 's'}
	  };
	int ret = 0;
	while(1)
    {
      ret = getopt_long(argc, argv, "", long_options, NULL);
      if(ret == -1)
		break;
	  int i;
      switch(ret)
		{
			case 't':
				n_thread = atoi(optarg);
				break;
			case 'i':
				n_iter = atoi(optarg);
				break;
			case 'y':
				for(i = 0; i < strlen(optarg); i++)
				{
					if(optarg[i] == 'i')
						opt_yield |= INSERT_YIELD;
			        if(optarg[i] == 'd')
			            opt_yield |= DELETE_YIELD;
			        if(optarg[i] == 'l')
			            opt_yield |= LOOKUP_YIELD;
				}
				break;
			case 's':
				opt_sync = 1;
				sync_op = *optarg;
				break;
	  	}
	}


	pthread_t *threads;
  	threads = (pthread_t*)malloc(n_thread * sizeof(pthread_t));
  	elements = (SortedListElement_t *)malloc(sizeof(SortedListElement_t) * n_thread * n_iter);
  	
  	head.prev = &head;
  	head.next = &head;
  	head.key = NULL;

  	int j, k;
  	int key_length = 3;
  	for(j = 0; j < n_thread * n_iter; j++)
  	{
  		char *temp = (char*)malloc(sizeof(char) * (key_length + 1));
  		for(k = 0; k < key_length; k++)
  			temp[k] = rand() % 26 + 65;
  		temp[key_length] = '\0';
  		elements[j].key = temp;
  	}

  	struct timespec t_start;
  	clock_gettime(CLOCK_MONOTONIC, &t_start);

  	if (sync_op == 'm') {
    pthread_mutex_init(&mutex, NULL);
	}

  	int *offsets = (int *)malloc(sizeof(int) * n_thread);
  	for (j = 0; j < n_thread; j++) 
  	{
    	offsets[j] = j * n_iter;
    	pthread_create(&threads[j], NULL, thread_func, &offsets[j]);
  	}

  	for(j = 0;  j < n_thread; j++)
  		pthread_join(threads[j], NULL);

  	struct timespec t_end;
  	clock_gettime(CLOCK_MONOTONIC, &t_end);
	if(SortedList_length(&head) != 0)
	{
		fprintf(stderr, "Error! Corrupted linked list\n");
		exit(1);
	}
	printf("list-");
	int count = 0;
  	if(opt_yield & INSERT_YIELD)
  	{
 		printf("i");
 		count++;
  	}
  	if(opt_yield & DELETE_YIELD)
  	{
  		printf("d");
  		count++;
  	}
  	if(opt_yield & LOOKUP_YIELD)
  	{
  		printf("l");
  		count++;
  	}

  	if(count == 0)
	{
		printf("none");
	}
  	
  	if(opt_sync)
  	{
  		if(sync_op == 's')
  			printf("-s");
  		if(sync_op == 'm')
  			printf("-m");
  	}
  	else
	{
		printf("-none");
	}

  	long long t1 = (long long)(t_start.tv_sec * 1000000000 + t_start.tv_nsec);
  	long long t2= (long long)(t_end.tv_sec * 1000000000 + t_end.tv_nsec);
  	long long t_total = t2 - t1;
  	long long n_ops = 3 * n_thread * n_iter;
  	int t_ave = t_total / n_ops;
  	printf(",%d,%d,1,%lld,%lld,%d\n", n_thread, n_iter, n_ops, t_total, t_ave);

  	exit(0);
  	return 0;
}