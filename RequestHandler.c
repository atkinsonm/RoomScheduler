//*************************************************************
// Michael Meluso
// CSC 345 - Operating Systems
// Project 2 - Threaded Reservation System
// 
// RequestHandler.c
// Receives requests for room reservations, keeps track of
// the rooms which are available, and returns data to 
// the requester.
//*************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <stdbool.h>

#define childLim 50 /*maximum number of child processes */

pthread_t thr[childLim]; // Max number of threads

struct arg_set {
	int ID; // ID associated with the person. starts at invalid value
	char *email; // email address of user
	char *roomSize; // Size of the desired room
	int month; // month from 1-12. starts at invalid value
	int day; // day from 1-31. starts at invalid value.
	int year; // month from 2014 up. starts at invalid value.
	int useTime[2]; // start and end time in 24 hour format. starts at invalid value.
	int loggedIn; // privilege level
	bool sub; // wilingness to substitute
};

// contains data from previous scheduling
struct arg_set booked[26][100];
 
struct arg_set args1;
int i = 0; // number of threads

struct arg_set *mailbox = NULL;
pthread_mutex_t mailLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bookLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t flag = PTHREAD_COND_INITIALIZER;

struct node {
    struct arg_set data;
    struct node *next;
    struct node *prev;
} *lowFirst, *midFirst, *highFirst;

void push (struct node *first, struct arg_set *x)
{
    struct node *newNode;// It should be a pointer
    newNode = (struct node *)malloc(sizeof(struct node));
    newNode->data = *x;
    newNode->next = first;
    first = newNode;
}

struct arg_set pop(struct node *first)
{
    struct node *prevPtr;
    if (first == NULL) // check if stack is empty
    {
        printf("Error: Stack Empty.\n");
	struct arg_set empty;
	empty.loggedIn = 0;
        return empty; 
    }

    prevPtr = first;
    first = first->next;

    return(prevPtr->data);
}

void *bookRoom(void *a)
{
	struct arg_set *args;
	system("gnome-terminal -e ./Requester");
	pthread_mutex_lock(&mailLock); // lock the mailbox while we take in input
	FILE *fp = fopen("file.txt", "r");
	if (fp == NULL)
	{
	  printf("Error opening file!\n");
	  exit(1);
	}
	
	sleep (300 * 1000);
	fscanf(fp, "%i,%i,%i,%i,%i,%s,%d,%i,%i,%s", args->month, args->day, args->year, args->useTime[0], args->useTime[1], args->roomSize, args->sub, args->ID, args->loggedIn, args->email);
	
	if (mailbox != NULL) {
	    // wait for mailbox to be empty
	    pthread_cond_wait(&flag,&mailLock);
	}
	
	mailbox = args; // put args into mailbox
	if (mailbox->loggedIn == 3) { // admin
	    push(highFirst, mailbox);
	}
	
	else if (mailbox->loggedIn == 1) { // student
	    push(midFirst, mailbox);
	}
	
	else { // faculty/staff
	    push(lowFirst, mailbox);
	}
	
	// get out of the critical section
	pthread_cond_signal(&flag);
	pthread_mutex_unlock(&mailLock);
	return NULL;
}

int schedule(struct arg_set current, struct arg_set booked[][100], int rooms[][2])
{
    int result;
    bool found = false;
    int i = 0, j = 0;
    
    if (current.roomSize == "small") {
	while (rooms[i][j] != 1) {
	    i++;
	}
	
	while ((!found) || booked[i][j].loggedIn != 0) {
	    if (current.day == booked[i][j].day) {
		if (current.month == booked[i][j].month) {
		  if (current.year == booked[i][j].year) {
		    if (current.useTime[0] == booked[i][j].useTime[0]) {
		      if (current.loggedIn != 3) {
			return -1;
		      }
		      
		      else {
			return 999;
		      }
		    }
		    else {
		      while (booked[i][j].loggedIn != 0) { j++; };
		      booked[i][j] = current;
		      return 1;
		    }
		  }
		}
	    }
	}
    }
    
    else if (current.roomSize == "standard") {
	while (rooms[i][j] != 2) {
	    i++;
	}
	
	while ((!found) || booked[i][j].loggedIn != 0) {
	    if (current.day == booked[i][j].day) {
		if (current.month == booked[i][j].month) {
		  if (current.year == booked[i][j].year) {
		    if (current.useTime[0] == booked[i][j].useTime[0]) {
		      if (current.loggedIn != 3) {
			return -1;
		      }
		      
		      else {
			return 999;
		      }
		    }
		    else {
		      while (booked[i][j].loggedIn != 0) { j++; };
		      booked[i][j] = current;
		      return 1;
		    }
		  }
		}
	    }
	}
    }
    
    else {
      while (rooms[i][j] != 3) {
	    i++;
      }
      
      while ((!found) || booked[i][j].loggedIn != 0) {
	    if (current.day == booked[i][j].day) {
		if (current.month == booked[i][j].month) {
		  if (current.year == booked[i][j].year) {
		    if (current.useTime[0] == booked[i][j].useTime[0]) {
		      if (current.loggedIn != 3) {
			return -1;
		      }
		      
		      else {
			return 999;
		      }
		    }
		    else {
		      while (booked[i][j].loggedIn != 0) { j++; };
		      booked[i][j] = current;
		      return 1;
		    }
		  }
		}
	    }
	}
    }
    
    return result;
}

void email (int success)
{
    if (success == 999) 
    {
	// email affected party through email stored with reservation
    }
    
    else if (success == 1)
    {
	// email confirmation to user using their email address
    }
    
    else if (success == 2)
    {
	// email user of successful cancellation
    }
}

// handle stored requests
void *handle(void *a)
{
  struct arg_set current;
  pthread_create(&thr[i], NULL, bookRoom, (void *) &args1);
  int success = 0;
  // room numbers and sizes represented from 1-3
  int rooms[26][2] = { {109, 1}, {110, 2}, {111, 1}, {202, 2}, {205, 2}, {220, 3}, {224, 3}, {225, 2}, {226, 2}, {228, 1}, {301, 2}, {308, 2}, {309, 2}, {310, 2}, {311, 2}, {315, 2}, {316, 2}, {317, 2}, {319, 2}, {404, 1}, {406, 2}, {411, 2}, {412, 2}, {413, 3}, {414, 2}, {415, 2} };

  // prevent seg fault due to no data stored
  pthread_join(thr[i], NULL);

  while (i > 0) {
    i--;
    success = 0;
    current = pop(highFirst);
    
    if (current.loggedIn = 0)
    {
	current = pop(midFirst);
	
	if (current.loggedIn = 0)
	{
	  current = pop(lowFirst);
	} 
    }
    
    success = schedule(current, booked, rooms);
    if (success == -1) {
	printf("Sorry! That day/time and room is already booked. Try for another time.");
    }
    
    else if (success == 1) {
	printf("Room booked!.");
	email(success);
    }
    
    else if (success == 999) {
	printf("Room override accepted. Affected parties notified.");
	email(success);
    }
  }	 
}

int main(int ac, char *av[])
{
	lowFirst = NULL;
	midFirst = NULL;
	highFirst = NULL;

	// run indefinitely
	while (true) {
		printf("More requests? Press any key to continue...");
		getchar();
		i++; // add to num of threads		
		pthread_create(&thr[i], NULL, handle, (void *) &args1);
		i++; // 2 threads per run
	}

	return 0;
}
