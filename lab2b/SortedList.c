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


void SortedList_insert(SortedList_t *list, SortedListElement_t *element)
{
	SortedListElement_t *p = list;
	if(p == NULL || element == NULL)
		return;
	while(p->next != list)
	{
		if(strcmp(p->next->key, element->key) < 0)
			break;
		p = p->next;
	}
	if(opt_yield & INSERT_YIELD)
		pthread_yield();
	p->next->prev = element;
	element->next = p->next;
	p->next = element;
	element->prev = p;
}

int SortedList_delete(SortedListElement_t* element)
{
	if(element == NULL)
		return 1;
	if(element->next->prev != element || element->prev->next != element)
		return 1;
	element->next->prev = element->prev;
	if (opt_yield & DELETE_YIELD) {
    	pthread_yield();
  	}
	element->prev->next = element->next;
	element = NULL;
	return 0;
}

SortedListElement_t* SortedList_lookup(SortedList_t* list, const char* key)
{
	SortedListElement_t *p = list;
	if(p == NULL || key == NULL)
		return NULL;
	while(p->next != list)
	{
		if(opt_yield & LOOKUP_YIELD) 
      		pthread_yield();
    	if(strcmp(p->next->key, key) == 0)
      		return p->next;
      	p = p->next;

	}
	return NULL;
}

int SortedList_length(SortedList_t *list)
{
	int length = 0;
	SortedListElement_t *p = list;
	while(p->next != list)
	{
		length++;
		if (opt_yield & LOOKUP_YIELD) 
			pthread_yield();
	    if (p->next->prev != p || p->prev->next != p)
	        return -1;
	    p = p->next;
  	}
	return length;
}
