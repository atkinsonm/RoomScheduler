//*************************************************************
// Michael Meluso
// CSC 345 - Operating Systems
// Project 2 - Threaded Reservation System
// 
// Requester.c
// Generates room requests, passes them to the request
// handler, and receives the response.
//*************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MAX_STRING 20 /*maximum length string to be read in */

char buf[MAX_STRING + 1]; /* input buffer */

int getID(void) 
{
	fgets(buf, MAX_STRING, stdin);
	strcat (buf, "\0"); // add end of string character
	return atoi(buf);
}

void getEmail(char *email) 
{
	do 
	{
		printf("Enter your email in the form username@tcnj.edu: ");
		fgets(buf, MAX_STRING, stdin);
		strcat (buf, "\0"); // add end of string character
		strcpy(email, buf);

	} while (strstr(buf, "@tcnj.edu") == NULL);
}

int logIn(void) 
{
	// Simulate authentication process. In a true implementation,
	// would require the user to log in and get their position from
	// the database. For the simulation, simply ask the user to supply
	// a privilege level.
	bool correct = false;
	while (!correct) {
	  printf("Enter (1,2,3) for student, faculty/staff, admin: ");
	  fgets(buf, MAX_STRING, stdin);
	  strcat (buf, "\0"); // add end of string character
	  
	  if ((atoi(buf) > 0) && (atoi(buf) <= 3)) { correct = true; }
	}
	
	return atoi(buf);
}

char *getRoomSize(void)
{
	fgets(buf, MAX_STRING, stdin);
	strcat (buf, "\0"); // add end of string character
	bool correct = false;

	do
	{
		// interpret 'v', 'V', 'l', 'L' as the user requesting a very large room
		if ((buf[0] == 'v') || (buf[0] == 'V') || (buf[0] == 'l') || (buf[0] == 'L'))
		{
			correct = true;
			return "very large";
		}

		else if ((buf[0] == 's') || (buf[0] == 'S')) 
		{
			if ((buf[1] == 'm') || (buf[1] == 'M')) {
				correct = true;
				return "small";
			}

			// else assume standard size
			else {
				correct = true;
				return "standard";
			}
		}
		
		// Error - user did not enter a valid room size
		else
		{
			printf("\nYou did not enter a valid room size. Select from \"small\", \"standard\", or \"very large\": ");
			buf[0] = '\0'; // reset buffer	
		}	
	} while (!correct);
	
}

int getYear(void)
{  
    int year = 0;
    bool correct = false;

    // Get the current year
    time_t now;
   	if ( time(&now) != (time_t)(-1) )
   	{
      struct tm *mytime = localtime(&now);
      if ( mytime )
      {
         if ( strftime(buf, sizeof buf, "%Y", mytime) )
         {
         	// Year holds the value for the current year
            year = atoi(buf);
         }
      }
   	}

   	buf[0] = '\0'; // reset buffer
    fgets(buf, MAX_STRING, stdin);
    strcat (buf, "\0"); // add end of string character

    if ((buf[0] == 'y') || (buf[0] == 'Y')) {
		return year;
    }

    else if ((buf[0] == 'n') || (buf[0] == 'N')) {
    	do {
    		printf("What year would you like to book for? (%i or after): ", year);

    		buf[0] = '\0'; // reset buffer
    		fgets(buf, MAX_STRING, stdin);
    		strcat (buf, "\0"); // add end of string character

    		if (atoi(buf) < year) {
    			printf("\nIncorrect input. Year must be present or future.");
    		}

    		else {
    			correct = true;
    		}

    	} while (!correct);

    	year = atoi(buf);
    }

    else {
		printf("\nIncorrect input. Please answer with \'Y\' or \'N\': ");
    }

    return year;
}

int getMonth(int year)
{
	int month = 0;
	int cYear;
	bool correct = false;

    // Get the current month
    time_t now;
   	if ( time(&now) != (time_t)(-1) )
   	{
      struct tm *mytime = localtime(&now);
      if ( mytime )
      {
         if (strftime(buf, sizeof buf, "%m", mytime) )
         {
         	// Current month
            month = atoi(buf);
            buf[0] = '\0'; // reset buffer
         }

         if (strftime(buf, sizeof buf, "%Y", mytime) )
         {
         	// Year holds the value for the current year
            cYear = atoi(buf);
         }
      }
   	}

   	buf[0] = '\0'; // reset buffer
    fgets(buf, MAX_STRING, stdin);
    strcat (buf, "\0"); // add end of string character

    if ((buf[0] == 'y') || (buf[0] == 'Y')) {
		return month;
    }

    else if ((buf[0] == 'n') || (buf[0] == 'N')) {
    	do {
    		printf("What month would you like to book for?: (1-12) ");

    		buf[0] = '\0'; // reset buffer
    		fgets(buf, MAX_STRING, stdin);
    		strcat (buf, "\0"); // add end of string character

    		if (year == cYear) {
    			if (atoi(buf) >= month) {
    				month = atoi(buf);
    				correct = true;
    			}

    			else {
    				printf("\nIncorrect input. Please enter a current or future month for this year.");
    			}
    		}

    		else {
    			month = atoi(buf);
    			correct = true;
    		}

    	} while (!correct);

    	year = atoi(buf);
    }

    else {
		printf("\nIncorrect input. Please answer with \'Y\' or \'N\': ");
    }

    return month;
}

int getDay (int month, int year)
{
	int dayCheck[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }; // Valid number of days per month
	int cMonth;
	int cYear;
	int cDay;
	int day = 0;
	bool correct = false;

  	// Leap year check
  	if (year % 4 == 0) {
  		dayCheck[2] = 29; // Make Feb have 29 days
  	}

    // Get the current month
    time_t now;
   	if ( time(&now) != (time_t)(-1) )
   	{
      struct tm *mytime = localtime(&now);
      if ( mytime )
      {
         if (strftime(buf, sizeof buf, "%m", mytime) )
         {
         	// Current month
            cMonth = atoi(buf);
         }

         if (strftime(buf, sizeof buf, "%Y", mytime) )
         {
         	// Current year
            cYear = atoi(buf);
         }

         if (strftime(buf, sizeof buf, "%d", mytime) )
         {
         	// Current day
            cDay = atoi(buf);
         }
      }
   	}

   	if ((year == cYear) && (month == cMonth)) {
   		// Print the valid days for the month.
   		printf("(%i-%i): ", cDay, dayCheck[month]);
   	}

   	else {
   		// Print the valid days for the month.
   		printf("(1-%i): ", dayCheck[month]);
   	}

   	do {
   		buf[0] = '\0'; // reset buffer
    	fgets(buf, MAX_STRING, stdin);
    	strcat (buf, "\0"); // add end of string character

    	if (year == cYear) {
			if (month == cMonth) {
				if ((atoi(buf) >= cDay) && (atoi(buf) <= dayCheck[month])) {
					day = atoi(buf);
					correct = true;
				}

				else {
					printf("\nIncorrect input. Please enter a valid day (%i-%i): ", cDay, dayCheck[month]);
				}
			}

			else {
				if ((atoi(buf) >= 0) && atoi(buf) <= dayCheck[month]) {
					day = atoi(buf);
					correct = true;
				}

				else {
					printf("\nIncorrect input. Please enter a valid day (1-%i): ", dayCheck[month]);
				}
			}
		}

		else {
			if ((atoi(buf) >= 0) && atoi(buf) < dayCheck[month]) {
				day = atoi(buf);
				correct = true;
			}

			else {
				printf("\nIncorrect input. Please enter a valid day (1-%i): ", dayCheck[month]);
			}
		}
   	} while (!correct);

    return day;
}

void getTime(int *useTime, int day, int month, int year) {
	int cMonth;
	int cYear;
	int cDay;
	int cTime;
	bool correct = false;

	// Get the current month
    time_t now;
   	if ( time(&now) != (time_t)(-1) )
   	{
		struct tm *mytime = localtime(&now);
		if ( mytime )
		{
		 	if (strftime(buf, sizeof buf, "%m", mytime) )
		 	{
			 	// Year holds the value for the current year
			    cMonth = atoi(buf);
			    buf[0] = '\0'; // reset buffer
			}

			if (strftime(buf, sizeof buf, "%Y", mytime) )
			{
				// Current month
			    cYear = atoi(buf);
			}

			if (strftime(buf, sizeof buf, "%d", mytime) )
			{
			 	// Current day
			    cDay = atoi(buf);
			}

			if(strftime(buf, sizeof buf, "%H", mytime) )
			{
				// Current time (hours)
			 	cTime = atoi(buf) * 100;
			}

			if(strftime(buf, sizeof buf, "%M", mytime) )
			{
				// Current time (hours and minutes) 
			 	cTime += atoi(buf);
			}
				
   		}
    }

   	if ((day == cDay) && (month == cMonth) && (year == cYear)) 
   	{
   		do {
   			buf[0] = '\0'; // reset buffer
    		fgets(buf, MAX_STRING, stdin);
    		strcat (buf, "\0"); // add end of string character

    		if ((atoi(buf) >= cTime) && (atoi(buf) <= 2359))
    		{
    			useTime[0] = atoi(buf);
    			correct = true;
    		}

    		else
    		{
    			printf("\nPlease enter a future time between %i and 2359: ", cTime);
    		}

    	} while (!correct);
   	}

   	else 
   	{
   		do {
   			buf[0] = '\0'; // reset buffer
    		fgets(buf, MAX_STRING, stdin);
    		strcat (buf, "\0"); // add end of string character

    		if ((atoi(buf) >= 0.0) && (atoi(buf) <= 2359.0))
    		{
    			useTime[0] = (atoi(buf));
    			correct = true;
    		}

    		else
    		{
    			printf("\nPlease enter a valid time. (0000-2359): ");
    		}

    	} while (!correct);
   	}

   	correct = false;

   	printf("How long would you like to book for? (1, 2, or 3 hours): ");

   	do
   	{
   		buf[0] = '\0'; // reset buffer
    	fgets(buf, MAX_STRING, stdin);
    	strcat (buf, "\0"); // add end of string character

    	if ((atof(buf) <= 3.0) && (atof(buf) >= 1.0)) 
    	{
    		useTime[1] = useTime[0] + (int)(((atof(buf)))*100);
    		correct = true;
    	}

    	else 
    	{
    		printf("\nPlease enter a whole number duration for your reservation (1-3): ");
    	}

   	} while (!correct);
}

int main(void)
{
	int ID = 0; // ID associated with the person. starts at invalid value
	char email[100]; // email address of user
	char *roomSize; // Size of the desired room
	int month = 0; // month from 1-12. starts at invalid value
	int day = 0; // day from 1-31. starts at invalid value.
	int year = 0; // month from 2014 up. starts at invalid value.
	int useTime[2] = {-1, -1}; // start and end time in 24 hour format. starts at invalid value.
	int loggedIn = 0; // Whether the user has entered their ID, username
	bool sub = false; // wilingness to substitute
	char output[MAX_STRING*10]; // final output of the Request

	// Enter details to proceed
	printf("Please log in to proceed. Enter your 6 digit ID: ");
	while (loggedIn == 0) {
		ID = getID();
		buf[0] = '\0'; // reset buffer
		getEmail(email);
		buf[0] = '\0'; // reset buffer
		loggedIn = logIn();
		buf[0] = '\0'; // reset buffer
	} 
	
	// Get the year the user would like to book for.
	printf("Would you like to book for this calendar year? (Y/N): ");
	while (year == 0) {
	    year = getYear();
	    buf[0] = '\0'; // reset buffer
	}

	// Get the month the user would like to book for.
	printf("Would you like to book for this month in %i? (Y/N): ", year);
	while ((month < 1) || (month > 12)) {
	    month = getMonth(year);
	    buf[0] = '\0'; // reset buffer
	}

	// Get the day the user would like to book for. Dynamicly change valid days in getDay
	printf("What day would you like to book for? ");
	while (day == 0) {
	    day = getDay(month, year);
	    buf[0] = '\0'; // reset buffer
	}

	// Get the time the user will use the room.
	printf("What time would you like to start using the room? (24 hour): ");
	while (useTime[0] == -1) {
		getTime(useTime, day, month, year);
		buf[0] = '\0'; // reset buffer
	}

	// Get the room size the user would like to book
	printf("What is the size of the room you are looking to reserve? (\"small\", \"standard\", or \"very large\"): ");
    roomSize = getRoomSize();
    strcat (roomSize, "\0"); // add end of string character
    buf[0] = '\0'; // reset buffer

    printf("Would you be willing to take a larger room if there is no %s room available? (Y/N): ", roomSize);
    buf[0] = '\0'; // reset buffer
	fgets(buf, MAX_STRING, stdin);
	strcat (buf, "\0"); // add end of string character

	// willingness to substitute
	if ((buf[0] == 'y') || (buf[0] == 'Y')) {
		sub = true;
	}

	else {
		while ((buf[0] != 'n') || (buf[0] == 'N')) {
			printf("\nIncorrect input. Please answer with \'Y\' or \'N\': ");
			buf[0] = '\0'; // reset buffer
			fgets(buf, MAX_STRING, stdin);
			strcat (buf, "\0"); // add end of string character
		}
	}

	printf("\n\nBooking %i (%s) for a %s room on %i/%i/%i from %i to %i...", ID, email, roomSize, month, day, year, useTime[0], useTime[1]);
	sprintf(output, "%i,%i,%i,%i,%i,%s,%d,%i,%i,%s", month, day, year, useTime[0], useTime[1], roomSize, sub, ID, loggedIn, email);
	
	FILE *f = fopen("file.txt", "w");
	if (f == NULL)
	{
	  printf("Error opening file!\n");
	  exit(1);
	}
	
	// write to file where handler can process it
	fprintf(f, "%s\n", output);

	printf("Press enter to continue...\n");
	getchar();

	return 0;
}

