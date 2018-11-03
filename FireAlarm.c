/* ******************************************************************************************************************************* */
/*					Assignment - Staffordshire University 2018                                 	           */
/*																   */
/*	    Microprocessor fire alarm system embedded within a 68hc11 Micro-controller - Release version 1.0    	           */
/*																   */
/*		        		       by Terence Broadbent (B028035c)					       	           */
/*  																   */
/* ******************************************************************************************************************************* */

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.											           */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Details  : Ensure that no error messages appear plus system define name labels to make reading this source code a lot easier.   */
/* Modified : N/A		 												   */
/* ******************************************************************************************************************************* */

#define _CRT_SECURE_NO_WARNINGS
#define MemoryWipe 0
#define False 0
#define True 1
#define Off 0
#define On 1

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.					 							   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Details  : Preload any required extended C command header files.								   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

#include <stdio.h>
#include <stdlib.h>

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.										    		   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Details  : Declare any global variables to be used by the system - The variables below are only used by the ClockTimer interrupt*/
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int G_Hours;	/* See ClockTimer() for explanation of global variables */
unsigned int G_Mins;	/*                       ""                             */
unsigned int G_Secs;	/*                       ""                             */
unsigned int G_Ticks;	/*                       ""                             */

unsigned char *G_PADR;	/*                       ""                             */
unsigned char *G_PADDR;	/*                       ""                             */
unsigned char *G_TFLG2;	/*                       ""                             */
unsigned char *G_PACTL;	/*                       ""                             */
unsigned char *G_TMSK2;	/*                       ""                             */

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.						 						   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Details  : Define any structures to be called by the main program.								   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

struct Alarm	{ 
		unsigned int Line [1];		/* Incrementing log entry counter for the individual zon      */
		unsigned int Node [5];		/* Nodes: 1 zone switch plus 3 sensors & 1 fire flag per zone */
		unsigned int Book [100];	/* Stipulates the limit of any log book entries per zone      */
		unsigned int Hour [100];	/* Stipulates the limit of any time log entries per Zone      */
		unsigned int Mins [100];	/*                             ""			      */
		unsigned int Secs [100];	/*                             ""			      */
		};

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Details  : Define any functions to be called by the main program.								   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */


void  ClockTimer (void);
unsigned int  Start_Logs (struct Alarm Zone []);
unsigned int  Reset_Syst (struct Alarm Zone []);
unsigned int  Reset_Zone (struct Alarm Zone []);
unsigned int  Reset_Sens (struct Alarm Zone []);
unsigned int  Reset_Stat (struct Alarm Zone []);
unsigned int  Togg_Zone1 (struct Alarm Zone []);
unsigned int  Togg_Zone2 (struct Alarm Zone []);
unsigned int  Togg_Zone3 (struct Alarm Zone []);
unsigned int  Build_Disp (struct Alarm Zone []);
unsigned int  Build_Logg (struct Alarm Zone []);
unsigned char SerialPort (struct Alarm Zone []);
unsigned char GrabK_Char (struct Alarm Zone []);
unsigned char ParralPort (struct Alarm Zone []);
unsigned int  Time_Stamp (struct Alarm Zone []);
unsigned int  Comp_Strng (const unsigned char String1[], const unsigned char String2[]);
unsigned int  Write_Book (struct Alarm Zone [], const unsigned int ZCode_0, const unsigned int ZCode_1, const unsigned int ZCode_2);


/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Details  : Main program - Assignment 1 (Fire Alarm Embedded System) Version 1.0						   */
/* Libraries: <stdlib.h>													   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

void main()
{
	
/* ******************************************************************************************************************************* */
/* Define and populate any local variables to be used by the main program                                                          */
/* ******************************************************************************************************************************* */
	
struct Alarm Zone[3]; 	/* Create 3 fire alarm zone circuits including switch, sensors and fire flag display plus log entries */
	unsigned char Key_Stroke;	/* Used to catch key strokes entered from the 8bit keyboard via the user	      */	
	Key_Stroke = '0';		/* Null the initial keystroke char value					      */

Start_Logs(Zone);			/* Set up the time and book log entries with default values of '0'		      */
	Reset_Zone(Zone);		/* Set the zones to a default value of '0'					      */
	Reset_Sens(Zone);		/* Set the zones sensors to a defalt value of '0'				      */
	Reset_Stat(Zone);		/* Set the zones fire status to a default value of '0'				      */
	Build_Disp(Zone);		/* Show the fire alarm system to the user via the screen			      */

/* ******************************************************************************************************************************* */
/* Human/Computer interface - Main menu system                                                                                     */
/* ******************************************************************************************************************************* */
	
for (;;)
	{
		Key_Stroke = SerialPort(Zone);
		switch (Key_Stroke)
				{
					case '1': { Togg_Zone1(Zone); break; } 	 /* Toggle zone one			           */
					case '2': { Togg_Zone2(Zone); break; }	 /* Toggle zone two			           */
					case '3': { Togg_Zone3(Zone); break; }	 /* Toggle zone three			           */
					case '4': { Reset_Syst(Zone); break; }	 /* Reset the alarm zones to default value '0'	   */
					case '5': { Build_Logg(Zone); break; }	 /* Show all log entries - upto maximum of 100	   */
					case '6': { exit(0); }			 /* Exit the program				   */
					default : { break; }			 /* Ignore any incorrectly entered characters	   */
				}
	}
}
/*End of main() */


/* ******************************************************************************************************************************* */
/* All the sub functions called by the main program are shown and detailed below:-                                                 */
/* ******************************************************************************************************************************* */

 
/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : ClockTimer()													   */
/* Details  : Interrupts the program every 32.768 ms to update the global clock variables then resets the real time interrupt flag.*/
/* Libraries: N/A														   */
/* Variables: Global variables - G_Ticks, G_Secs, G_Mins, G_Mins and G_Hours.							   */
/* Modified : Note Add @interrupt 												   */
/* ******************************************************************************************************************************* */

@interrupt void ClockTimer(void)
{
	G_Ticks++;
	if (G_Ticks == 30) { G_Ticks = 0; G_Secs++; }	/* Executed every 30 seconds */
	if (G_Secs  == 60) { G_Secs = 0; G_Mins++;  }	/* Executed every 60 seconds */
	if (G_Mins  == 60) { G_Mins = 0; G_Hours++; }	/* Executed every 60 minutes */
	if (G_Hours == 24) { G_Hours = 0; }		/* Executed every 24 hours   */
	*G_TFLG2 = 0x40;				/* Reset RTI flag	     */
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Start_Logs()													   */
/* Details  : Sets the default line entry to '0' and clears the required memory locations for time and log book entries to '0'.	   */
/*          : Allocates the memory locations for the global variables used by the clock timer and sets required controller settings*/
/*          : Only used once at the initiation of the program.									   */
/* Libraries: N/A														   */
/* Variables: Zone[0-2].Line[0-99], G_PADR, *G_PADDR, *G_TMSK2, G_TFLG2, *G_PACTL, G_Ticks, G_Secs, G_Mins, G_Mins and G_Hours.	   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Start_Logs(struct Alarm Zone[])
{
	auto unsigned int Loop;

	Zone[0].Line[0] = 0;				/* Start the log book (and time) line entries off at '0'  */
	Zone[1].Line[0] = 0;				/*					""		  */
	Zone[2].Line[0] = 0;				/*					""		  */

	for (Loop = 0; Loop < 100; Loop++)
	{
		Zone[0].Book[Loop] = MemoryWipe;	/* Clear the log book memory area for new data		  */
		Zone[1].Book[Loop] = MemoryWipe;	/*					""		  */
		Zone[2].Book[Loop] = MemoryWipe;	/*					""		  */
		Zone[0].Hour[Loop] = MemoryWipe;	/* Clear the time log memory area for new data		  */
		Zone[1].Mins[Loop] = MemoryWipe;	/*					""		  */
		Zone[2].Secs[Loop] = MemoryWipe;	/*					""		  */
	}

	G_PADR =  (unsigned char *)0x0000;		/* Port A data register					  */
	G_PADDR = (unsigned char *)0x0001;		/* Port A data direction register			  */
	G_TMSK2 = (unsigned char *)0x24;		/* Timer interrupt mask 2 - RTII flag			  */
	G_TFLG2 = (unsigned char *)0x25;		/* Timer interrupt flag 2 - RTIF flag			  */
	G_PACTL = (unsigned char *)0x26;		/* Pulse accumulator control				  */			
	*G_PADDR = 0xfe;				/* Sets Port A to output				  */
	*G_PACTL = 0x03;				/* sets the real time interrupt period to 32.768 ms	  */
	*G_TMSK2 = 0x40;				/* Enable RTI interrupt					  */

	G_Hours = 0;					/* Start clock timer at 00:00:00			  */
	G_Mins  = 0;					/*					""		  */
	G_Secs  = 0;					/*					""		  */
	G_Ticks = 0;					/*					""		  */
	
	Write_Book(Zone, 1, 1, 1);			/* Update the log book with above function actions	  */
	return (0);	
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Reset_Syst()													   */
/* Details  : Resets the zone and sensor data held within the alarm zones to a default value of '0' and clears fire status display.*/
/*          : Requires the correct user password to be entered to authenticate the action otherwise it is treated as a tamper event*/
/* Libraries: <stdio.h>														   */
/* Variables: Zone[0-2], *SCDR, *SCSR, Pass_Master[], PaswordCopy[], Test_String, Test_Tamper.					   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Reset_Syst(struct Alarm Zone[])
{
	auto unsigned char *SCDR;		/* Serial communications data register		      */
	auto unsigned char *SCSR;		/* SCI status register   			      */
	auto unsigned char Pass_Master[5];	/* Master hard coded password			      */
	auto unsigned char PaswordCopy[5];	/* User entered password			      */
	auto unsigned int  Loop;		/*						""    */
	auto unsigned int  Test_String;		/* Return value for testing matching Password strings */
	auto unsigned int  Test_Tamper;		/* Authentication/Tamper flag                         */

	SCSR = (unsigned char *)0x2e;		/* Clears the RDRF flag				      */
	SCDR = (unsigned char *)0x2f;		/* Retrieves last byte data buffer via the keyboard.  */
	Test_String = False;			/* Default that password strings do not match	      */
	Test_Tamper = Off;			/* Default to tamper off			      */
	Loop = 0;

	printf(" *********************************************************************\n");/* Display sub-menu system on the screen*/
	printf(" *                                                                   *\n");
	printf(" *                 FIRE ALARM SYSTEM - SYSTEM RESET                  *\n");
	printf(" *                                                                   *\n");
	printf(" *********************************************************************\n");
	printf("\n Please enter the five figure security Password > ");

/* ******************************************************************************************************************************* */
/* Grab first 5 key characters entered by the user and compare it with the master password then process and display the outcome    */
/* ******************************************************************************************************************************* */

	for (Loop = 0; Loop < 5; Loop++)
	{
		while (((*SCSR) & 0x20) == 0x0) { ;; }		/* Wait until a 8 bit key stroke is pressed by the user	 */
		PaswordCopy[Loop] = (char) *SCDR;		/* Build the user entered password for authentication	 */
		if (PaswordCopy[Loop] == '\r') { ;; }		/* Check if the user has entered a return value		 */	
			else
				{
				printf("%c", PaswordCopy[Loop]);/* Display typed character to the user for confirmation	 */
				}
		if (PaswordCopy[Loop] == '\r') { Loop = 5; }	/* Break from loop once the enter key has been pressed	 */
	}

	Pass_Master[0] = 'a';					/* Build master password [Complexity can be changed !!!] */
	Pass_Master[1] = 'b';
	Pass_Master[2] = 'o';
	Pass_Master[3] = 'r';
	Pass_Master[4] = 't';

	Test_String = Comp_Strng(PaswordCopy, Pass_Master);	/* Test the user password match's the master password    */

	if (Test_String == True)				/* All good - Authenticated */
	{
		printf("\n\n Security password authenticated.\n The fire alarm system has now been re-set to default values.");
		Reset_Zone(Zone);
		Reset_Sens(Zone);
		Reset_Stat(Zone);
	}
	else		/* No good - Tampered */
	{
	printf("\n\n I am sorry, I cannot reset the alarm system.\n An incorrect security password was entered.");
	Test_Tamper = On;
	}

	printf("\n\n *********************************************************************\n");
	printf("\n Please enter any key to continue....");
	while (((*SCSR) & 0x20) == 0x0) { ;; }			/* Wait until a 8 bit key stroke is pressed by the user	*/
	Build_Disp(Zone);					/* Display a new screen build			        */
	if (Test_Tamper == Off) { Write_Book(Zone, 3, 3, 3); }	/* Zone one, two and three have been reset		*/
	if (Test_Tamper == On)  { Write_Book(Zone, 4, 4, 4); }	/* Zone one, two and three have not been reset   	*/
	return (0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Reset_Zone()													   */
/* Details  : System reset all the zone on/off settings to a default value '0' - ie Off.					   */
/* Libraries: N/A														   */
/* Variables: Zone[0-2].Node[0].                                                                                                   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Reset_Zone(struct Alarm Zone[])
{
	Zone[0].Node[0] = Off;
	Zone[1].Node[0] = Off;
	Zone[2].Node[0] = Off;
	Write_Book(Zone, 5, 5, 5);
	return (0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Reset_Sens()													   */
/* Details  : System reset all the zone sensors on/off settings to a default value '0' - ie Off.				   */
/* Libraries: N/A														   */
/* Variables: Zone[0-2].Node[1].                                                                                                   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */


unsigned int Reset_Sens(struct Alarm Zone[])
{
	Zone[0].Node[1] = Off;
	Zone[0].Node[2] = Off;
	Zone[0].Node[3] = Off;
	Zone[1].Node[1] = Off;
	Zone[1].Node[2] = Off;
	Zone[1].Node[3] = Off;
	Zone[2].Node[1] = Off;
	Zone[2].Node[2] = Off;
	Zone[2].Node[3] = Off;
	Write_Book(Zone, 6, 6, 6);
	return (0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Reset_Stat()													   */
/* Details  : System reset all the zone fire status on/off settings to a default value '0' - ie Off.				   */
/* Libraries: N/A														   */
/* Variables: Zone[0-2].Node[1].                                                                                                   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Reset_Stat(struct Alarm Zone[])
{
	Zone[0].Node[4] = Off;
	Zone[1].Node[4] = Off;
	Zone[2].Node[4] = Off;
	Write_Book(Zone, 7, 7, 7);
	return (0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Togg_Zone1()													   */
/* Details  : Toggle the zone on or off depending on the zones current setting using simple boolean logic.			   */
/* Libraries: N/A														   */
/* Variables: Zone[0].Node[0].                                                                                                     */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Togg_Zone1(struct Alarm Zone[])
{
	Zone[0].Node[0] = !Zone[0].Node[0];
	Build_Disp(Zone);
	Write_Book(Zone, 2, 0, 0);
	return (0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Togg_Zone2()													   */
/* Details  : Toggle the zone on or off depending on the zones current setting using simple boolean logic.			   */
/* Libraries: N/A														   */
/* Variables: Zone[1].Node[0].                                                                                                     */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Togg_Zone2(struct Alarm Zone[])
{
	Zone[1].Node[0] = !Zone[1].Node[0];
	Build_Disp(Zone);
	Write_Book(Zone, 0, 2, 0);
	return (0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Togg_Zone3()													   */
/* Details  : Toggle the zone on or off depending on the zones current setting using simple boolean logic.			   */
/* Libraries: N/A														   */
/* Variables: Zone[2].Node[0].                                                                                                     */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Togg_Zone3(struct Alarm Zone[])
{
	Zone[2].Node[0] = !Zone[2].Node[0];
	Build_Disp(Zone);
	Write_Book(Zone, 0, 0, 2);
	return(0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Build_Disp()													   */
/* Details  : Build a display screen to the user of the current status of all the alarm zones, sensors and status.		   */
/*          : Build a display of the Human/Computer interface - menu system.							   */
/* Libraries: <stdio.h>														   */
/* Variables: Zone[0-2].Node[0-4].                                                                                                 */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Build_Disp(struct Alarm Zone[])
{
	printf("\n");
	printf(" *********************************************************************\n");
	printf(" *                                                                   *\n");
	printf(" *            FIRE ALARM SYSTEM (Ver 1.0) - MONITOR VIEW             *\n");
	printf(" *                                                                   *\n");
	printf(" *********************************************************************\n");
	printf(" *                                                                   *\n");
	printf(" *        ");
	if (Zone[0].Node[4] == Off) { printf("         "); }	/* Display no status     */
	if (Zone[0].Node[4] == On) { printf("- FIRE - "); }	/* Display fire status	 */
	printf("            ");	
	if (Zone[1].Node[4] == Off) { printf("         "); }	/* Display no status	*/
	if (Zone[1].Node[4] == On) { printf("- FIRE - "); }	/* Display fire status	*/
	printf("             ");
	if (Zone[2].Node[4] == Off) { printf("         "); }	/* Display no status	*/
	if (Zone[2].Node[4] == On) { printf("- FIRE - "); }	/* Display fire status	*/
	printf("       *\n");
	printf(" *                                                                   *\n");
	printf(" * ..................... ..................... ..................... *\n");
	printf(" * .   ALARM ZONE: 1   . .   ALARM ZONE: 2   . .   ALARM ZONE: 3   . *\n");
	printf(" * ..................... ..................... ..................... *\n");
	printf(" * .                   . .                   . .                   . *\n");
	printf(" * .                   . .                   . .                   . *\n");
	printf(" * .                   . .                   . .                   . *\n");
	printf(" * .                   . .                   . .                   . *\n");
	printf(" * .               . . . .               . . . .               . . . *\n");
	printf(" * .               .   . .               .   . .               .   . *\n");
	printf(" * .               . ");
	printf("%i", Zone[0].Node[0]);	/* Display zone 1 status	  	        */
	printf(" . .               . ");
	printf("%i", Zone[1].Node[0]);	/* Display zone 2 status			*/
	printf(" . .               . ");
	printf("%i", Zone[2].Node[0]);	/* Display zone 3 status			*/
	printf(" . *\n");
	printf(" * ..................... ..................... ..................... *\n");
	printf(" *   .       .       .     .       .       .     .       .       .   *\n");
	printf(" *   .       .       .     .       .       .     .       .       .   *\n");
	printf(" *   .       .       .     .       .       .     .       .       .   *\n");
	printf(" *   .       .       .     .       .       .     .       .       .   *\n");
	printf(" *   .       .       .     .       .       .     .       .       .   *\n");
	printf(" *   .       .       .     .       .       .     .       .       .   *\n");
	printf(" *   .       .       .     .       .       .     .       .       .   *\n");
	printf(" *   .       .       .     .       .       .     .       .       .   *\n");
	printf(" *   .       .       .     .       .       .     .       .       .   *\n");
	printf(" *   .       .       .     .       .       .     .       .       .   *\n");
	printf(" * . . .   . . .   . . . . . .   . . .   . . . . . .   . . .   . . . *\n");
	printf(" * .   .   .   .   .   . .   .   .   .   .   . .   .   .   .   .   . *\n");
	printf(" * .   .   .   .   .   . .   .   .   .   .   . .   .   .   .   .   . *\n");
	printf(" * .   .   .   .   .   . .   .   .   .   .   . .   .   .   .   .   . *\n");
	printf(" * .   .   .   .   .   . .   .   .   .   .   . .   .   .   .   .   . *\n");
	printf(" * .   .   .   .   .   . .   .   .   .   .   . .   .   .   .   .   . *\n");
	printf(" * .   .   .   .   .   . .   .   .   .   .   . .   .   .   .   .   . *\n");
	printf(" * . . .   . . .   . . . . . .   . . .   . . . . . .   . . .   . . . *\n");
	printf(" * .   .   .   .   .   . .   .   .   .   .   . .   .   .   .   .   . *\n");
	printf(" * . ");
	printf("%i", Zone[0].Node[1]);	/* Display zone 1 sensor 1 status		*/
	printf(" .   . ");
	printf("%i", Zone[0].Node[2]);	/* Display zone 1 sensor 2 status		*/
	printf(" .   . ");
	printf("%i", Zone[0].Node[3]);	/* Display zone 1 sensor 3 status		*/
	printf(".  . ");
	printf("%i", Zone[1].Node[1]);	/* Display zone 2 sensor 1 status		*/
	printf(" .   . ");
	printf("%i", Zone[1].Node[2]);	/* Display zone 2 sensor 2 status		*/
	printf(" .   . ");
	printf("%i", Zone[1].Node[3]);	/* Display zone 2 sensor 3 status		*/
	printf(" . . ");
	printf("%i", Zone[2].Node[1]);	/* Display zone 3 sensor 1 status		*/
	printf(" .   . ");
	printf("%i", Zone[2].Node[2]);	/* Display zone 3 sensor 2 status		*/
	printf(" .   . ");
	printf("%i", Zone[2].Node[3]);	/* Display zone 3 sensor 3 status		*/
	printf(" . *\n");
	printf(" * . . .   . . .   . . . . . .   . . .   . . . . . .   . . .   . . . *\n");			
	printf(" *                                                                   *\n");
	printf(" *                                                                   *\n");
	printf(" *********************************************************************\n");
	printf(" *                                                                   *\n");
	printf(" * [1]. Toggle Zone One [2]. Toggle Zone Two [3]. Toggle Zone Three  *\n");/* Human/Computer interface menu system */
	printf(" *                                                                   *\n");
	printf(" * [4]. System Reset    [5]. View System Log [6]. Exit Program       *\n");
	printf(" *                                                                   *\n");
	printf(" *********************************************************************\n");
	return (0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Build_Logg()													   */
/* Details  : Display the last 100 user log entries per zone (if populated with data) to the screen for the user to read.          */
/*          : Default value '0' has no display output - i.e. no activity within a zone.						   */
/* Libraries: <stdio.h>														   */
/* Variables: Zone[0-2].Book[0-99], Zone[0-2].Hour[0-99], Zone[0-2].Mins[0-99], Zone[0-2].Secs[0-99], *SCDR, *SCDR.                */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Build_Logg(struct Alarm Zone[])
{
	auto unsigned char *SCDR;	/* Serial communications data register		    */
	auto unsigned char *SCSR;	/* SCI status register   			    */
	auto unsigned int  Loop;

	SCSR = (unsigned char *)0x2e;	/* Clears the RDRF flag				    */
	SCDR = (unsigned char *)0x2f;	/* Retrieves last byte data buffer via the keyboard.*/

	printf(" *********************************************************************\n");/*Display sub-menu system on the screen */
	printf(" *                                                                   *\n");
	printf(" *                 FIRE ALARM SYSTEM - ENTRY LOGS                    *\n");
	printf(" *                                                                   *\n");
	printf(" *********************************************************************\n");

	for (Loop = 0; Loop < 100; Loop++)
	{                                          /* ZCode */
	if (Zone[0].Book[Loop] == 1)  { printf(" %02d:%02d:%02d Zone 1 time and log book created.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 1)  { printf(" %02d:%02d:%02d Zone 2 time and log book created.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 1)  { printf(" %02d:%02d:%02d Zone 3 time and log book created.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[0].Book[Loop] == 2)  { printf(" %02d:%02d:%02d Zone 1 toggled.\n",Zone[0].Hour[Loop],	Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 2)  { printf(" %02d:%02d:%02d Zone 2 toggled.\n",Zone[0].Hour[Loop],	Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 2)  { printf(" %02d:%02d:%02d Zone 3 toggled.\n",Zone[0].Hour[Loop],	Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[0].Book[Loop] == 3)  { printf(" %02d:%02d:%02d Zone 1 successfully reset by the user.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 3)  { printf(" %02d:%02d:%02d Zone 2 successfully reset by the user.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 3)  { printf(" %02d:%02d:%02d Zone 3 successfully reset by the user.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[0].Book[Loop] == 4)  { printf(" %02d:%02d:%02d An attempt was made to reset zone 1.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 4)  { printf(" %02d:%02d:%02d An attempt was made to reset zone 2.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 4)  { printf(" %02d:%02d:%02d An attempt was made to reset zone 3.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[0].Book[Loop] == 5)  { printf(" %02d:%02d:%02d Zone 1 reset.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 5)  { printf(" %02d:%02d:%02d Zone 2 reset.\n",Zone[1].Hour[Loop], Zone[1].Mins[Loop], Zone[1].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 5)  { printf(" %02d:%02d:%02d Zone 3 reset.\n",Zone[2].Hour[Loop], Zone[2].Mins[Loop], Zone[2].Secs[Loop]); }
	if (Zone[0].Book[Loop] == 6)  { printf(" %02d:%02d:%02d Zone 1 sensors reset.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 6)  { printf(" %02d:%02d:%02d Zone 2 sensors reset.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 6)  { printf(" %02d:%02d:%02d Zone 3 sensors reset.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[0].Book[Loop] == 7)  { printf(" %02d:%02d:%02d Zone 1 fire flag reset.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 7)  { printf(" %02d:%02d:%02d Zone 2 fire flag reset.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 7)  { printf(" %02d:%02d:%02d Zone 3 fire flag reset.\n",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[0].Book[Loop] == 8)  { printf(" %02d:%02d:%02d Zone 1 system log displayed to the user.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 8)  { printf(" %02d:%02d:%02d Zone 2 system log displayed to the user.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 8)  { printf(" %02d:%02d:%02d Zone 3 system log displayed to the user.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[0].Book[Loop] == 9)  { printf(" %02d:%02d:%02d Sensor 1 has detected a fire in zone 1.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[0].Book[Loop] == 10) { printf(" %02d:%02d:%02d Sensor 2 has detected a fire in zone 1.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[0].Book[Loop] == 11) { printf(" %02d:%02d:%02d Sensor 3 has detected a fire in zone 1.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 12) { printf(" %02d:%02d:%02d Sensor 4 has detected a fire in zone 2.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 13) { printf(" %02d:%02d:%02d Sensor 5 has detected a fire in zone 2.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[1].Book[Loop] == 14) { printf(" %02d:%02d:%02d Sensor 6 has detected a fire in zone 2.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 15) { printf(" %02d:%02d:%02d Sensor 7 has detected a fire in zone 3.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 16) { printf(" %02d:%02d:%02d Sensor 8 has detected a fire in zone 3.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	if (Zone[2].Book[Loop] == 17) { printf(" %02d:%02d:%02d Sensor 9 has detected a fire in zone 3.\n ",Zone[0].Hour[Loop], Zone[0].Mins[Loop], Zone[0].Secs[Loop]); }
	}

	printf("\n *********************************************************************\n");
	printf("\n Press any key to continue...");
	while (((*SCSR) & 0x20) == 0x0) { ;; }
	Build_Disp(Zone);
	Write_Book(Zone, 8, 8, 8);
	return (0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : SerialPort()													   */
/* Details  : Check the serial port and obtain the key character that the user has entered via the keyboard			   */
/* Libraries: N/A														   */
/* Variables: Zone[], Eight_Bit_Port_Val.											   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned char SerialPort(struct Alarm Zone[])
{
	auto unsigned char Eight_Bit_Port_Val;		/* Local variable used within this function                */

	for (;;)
	{
		Eight_Bit_Port_Val = GrabK_Char(Zone);	/* Keep scanning until 8 bit keystroke entered by the user */
		return (Eight_Bit_Port_Val);		/* Return the actual keystroke entered by the user         */
	}

}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : GrabK_Char()													   */
/* Details  : Read SCSR register value if equal to '0' then no ascii character has been entered via the keyboard keep checking.    */
/*          : If SCSR register value is equal to '1' then ascii character has been entered via the keyboard so break from the loop.*/
/* Libraries: N/A														   */
/* Variables: Zone[], *SCSR, *SCDR.  												   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned char GrabK_Char(struct Alarm Zone[])
{
		auto unsigned char *SCDR;	/* Serial communications data register		                               */
		auto unsigned char *SCSR;	/* SCI status register   				                       */

		SCSR = (unsigned char *)0x2e;	/* Clears the RDRF flag				                               */
		SCDR = (unsigned char *)0x2f;	/* Retrieves last byte data buffer via the keyboard.	                       */

		while (((*SCSR) & 0x20) == 0x0)
		{
			ParralPort(Zone);	/* Scan the parallel port for any activated (switched) triggers	               */
		}
		return (*SCDR);			/* Grab the Ascii character and return its char value back to the main program */
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : ParralPort()													   */
/* Details  : Check the parallel port and obtain the trigger that have been switched by the user.				   */
/* Libraries: N/A														   */
/* Variables: Zone[0-2].Node[1-4], *PEDR, Data, Nbit, Bits[0-7], Sensor_Test.							   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned char ParralPort(struct Alarm Zone[])
{
	auto unsigned char *PEDR;			/* Port E data register			            */
	auto unsigned char  Data;			/* Byte containing switch bits 0 - 7	            */
	auto unsigned char  Nbit;			/* Bit 9				            */
	auto unsigned int   Bits[8];			/* Will become single bits of byte 0 - 7            */
	auto unsigned int   Loop;			/* Local variables used by this function            */
	auto unsigned int   Sensor_Test;		/*                    ""		            */

	PEDR = (unsigned char *)0x000a;			/* Assign to port E			            */
	Data = *PEDR;					/* Read triggers 0 - 7 as a char byte	            */
	Nbit = (*G_PADR) & 0x1;				/* Read the ninth trigger		            */
	Sensor_Test = Off;				/* Used to test if the screen display is up to date */

/* ******************************************************************************************************************************* */
/* Return back to the serial port scan as quickly as possible if there is no visual bit trigger changes to be displayed to the user*/
/* ******************************************************************************************************************************* */

	for (Loop = 0; Loop < 3; Loop++)
		{
		if (Data != 0x00 || Nbit != 0x00 || Zone[Loop].Node[Loop + 1] == 1)
			{ Sensor_Test = On; }	/* The screen still needs updating */}
		if (Sensor_Test == Off) { return (0); }    
		/* Display is good - all sensors = 0 and no triggers switched since last display build      */

/* ******************************************************************************************************************************* */
/* New trigger switched or the last remaining trigger has been turned off but still needs to be removed from the display carry on  */
/* ******************************************************************************************************************************* */

	Bits[0] = (Data & (1 << 0)) != 0;/* Split byte variable ‘Data’ into a 8 bits where ‘1’ = triggered on & ‘0’ = not triggered*/
	Bits[1] = (Data & (1 << 1)) != 0;
	Bits[2] = (Data & (1 << 2)) != 0;
	Bits[3] = (Data & (1 << 3)) != 0;
	Bits[4] = (Data & (1 << 4)) != 0;
	Bits[5] = (Data & (1 << 5)) != 0;
	Bits[6] = (Data & (1 << 6)) != 0;
	Bits[7] = (Data & (1 << 7)) != 0;

/* ******************************************************************************************************************************* */
/* Repopulate the zone sensors with 'bit' trigger values, so that the sensors display the current status and update FIRE flag      */
/* ******************************************************************************************************************************* */

if (Zone[0].Node[0] == On  && Bits[0] == On)  { Zone[0].Node[1] = On; }		/* Zone on trip on turn sensor on      		   */
if (Zone[0].Node[0] == On  && Bits[0] == On)  { Zone[0].Node[4] = On; }		/* Activate fire flag		           	   */
if (Zone[0].Node[0] == On  && Bits[0] == Off) { Zone[0].Node[1] = Off;}		/* Zone on Trip off turn sensor off    		   */
if (Zone[0].Node[0] == Off && Bits[0] == On)  { Zone[0].Node[1] = Off;}/* Zone off trip on turn sensor off if prev. set  reset	   */
if (Zone[0].Node[0] == On  && Bits[1] == On)  { Zone[0].Node[2] = On; }		/* Zone on trip on turn sensor on      		   */
if (Zone[0].Node[0] == On  && Bits[1] == On)  { Zone[0].Node[4] = On; }		/* Activate fire flag		           	   */
if (Zone[0].Node[0] == On  && Bits[1] == Off) { Zone[0].Node[2] = Off;}		/* Zone on Trip off turn sensor off    		   */
if (Zone[0].Node[0] == Off && Bits[1] == On)  { Zone[0].Node[2] = Off;} /* Zone off trip on turn sensor off - if prev. set  re-set */		
if (Zone[0].Node[0] == On  && Bits[2] == On)  { Zone[0].Node[3] = On; }		/* Zone on trip on turn sensor on      		   */
if (Zone[0].Node[0] == On  && Bits[2] == On)  { Zone[0].Node[4] = On; }		/* Activate fire flag		           	   */
if (Zone[0].Node[0] == On  && Bits[2] == Off) { Zone[0].Node[3] = Off;}		/* Zone on Trip off turn sensor off    		   */
if (Zone[0].Node[0] == Off && Bits[2] == On)  { Zone[0].Node[3] = Off;}/* Zone off trip on turn sensor off - if prev. set  re-set  */		
if (Zone[1].Node[0] == On  && Bits[3] == On)  { Zone[1].Node[1] = On; }		/* Zone on trip on turn sensor on      		   */
if (Zone[1].Node[0] == On  && Bits[3] == On)  { Zone[1].Node[4] = On; }		/* Activate fire flag		           	   */
if (Zone[1].Node[0] == On  && Bits[3] == Off) { Zone[1].Node[1] = Off;}		/* Zone on Trip off turn sensor off    		   */
if (Zone[1].Node[0] == Off && Bits[3] == On)  { Zone[1].Node[1] = Off;} /* Zone off trip on turn sensor off - if prev. set  re-set */		
if (Zone[1].Node[0] == On  && Bits[4] == On)  { Zone[1].Node[2] = On; }		/* Zone on trip on turn sensor on      		   */
if (Zone[1].Node[0] == On  && Bits[4] == On)  { Zone[1].Node[4] = On; }		/* Activate fire flag		           	   */
if (Zone[1].Node[0] == On  && Bits[4] == Off) { Zone[1].Node[2] = Off;}		/* Zone on Trip off turn sensor off    		   */
if (Zone[1].Node[0] == Off && Bits[4] == On)  { Zone[1].Node[2] = Off;} /* Zone off trip on turn sensor off - if prev. set re-set  */		
if (Zone[1].Node[0] == On  && Bits[5] == On)  { Zone[1].Node[3] = On; }		/* Zone on trip on turn sensor on      		   */
if (Zone[1].Node[0] == On  && Bits[5] == On)  { Zone[1].Node[4] = On; }		/* Activate fire flag		           	   */
if (Zone[1].Node[0] == On  && Bits[5] == Off) { Zone[1].Node[3] = Off;}		/* Zone on Trip off turn sensor off	           */
if (Zone[1].Node[0] == Off && Bits[5] == On)  { Zone[1].Node[3] = Off;} /* Zone off trip on turn sensor off - if prev. set  re-set */		
if (Zone[2].Node[0] == On  && Bits[6] == On)  { Zone[2].Node[1] = On; }		/* Zone on trip on turn sensor on      		   */
if (Zone[2].Node[0] == On  && Bits[6] == On)  { Zone[2].Node[4] = On; }		/* Activate fire flag			           */
if (Zone[2].Node[0] == On  && Bits[6] == Off) { Zone[2].Node[1] = Off;}		/* Zone on Trip off turn sensor off	           */
if (Zone[2].Node[0] == Off && Bits[6] == On)  { Zone[2].Node[1] = Off;} /* Zone off trip on turn sensor off - if prev. set  re-set */		
if (Zone[2].Node[0] == On  && Bits[7] == On)  { Zone[2].Node[2] = On; }		/* Zone on trip on turn sensor on      		   */
if (Zone[2].Node[0] == On  && Bits[7] == On)  { Zone[2].Node[4] = On; }		/* Activate fire flag		           	   */
if (Zone[2].Node[0] == On  && Bits[7] == Off) { Zone[2].Node[2] = Off;}		/* Zone on Trip off turn sensor off	           */
if (Zone[2].Node[0] == Off && Bits[7] == On)  { Zone[2].Node[2] = Off;}/* Zone off trip on turn sensor off - if prev. set  re-set  */		
if (Zone[2].Node[0] == On  && Nbit == 0x01)   { Zone[2].Node[3] = On; }		/* Zone on trip on turn sensor on      		   */
if (Zone[2].Node[0] == On  && Nbit == 0x01)   { Zone[2].Node[4] = On; }		/* Activate fire flag			           */
if (Zone[2].Node[0] == On  && Nbit == 0x00)   { Zone[2].Node[3] = Off;}		/* Zone on Trip off turn sensor off	           */
if (Zone[2].Node[0] == Off && Nbit == 0x01)   { Zone[2].Node[3] = Off;}/* Zone off trip on turn sensor off - if prev. set  re-set  */

	Build_Disp(Zone);

/* Write the above Zone data changes to the log book	for zones 1,2 and 3							   */

	if (Zone[0].Node[0] == On && Bits[0] == On) { Write_Book( Zone, 9, 0, 0);  } 		/* Trigger 1 activated by the user */
	if (Zone[0].Node[0] == On && Bits[1] == On) { Write_Book( Zone, 10, 0, 0); }		/* Trigger 2 activated by the user */
	if (Zone[0].Node[0] == On && Bits[2] == On) { Write_Book( Zone, 11, 0, 0); }		/* Trigger 3 activated by the user */
	if (Zone[1].Node[0] == On && Bits[3] == On) { Write_Book( Zone, 0, 12, 0); }		/* Trigger 4 activated by the user */
	if (Zone[1].Node[0] == On && Bits[4] == On) { Write_Book( Zone, 0, 13, 0); }		/* Trigger 5 activated by the user */
	if (Zone[1].Node[0] == On && Bits[5] == On) { Write_Book( Zone, 0, 14, 0); }		/* Trigger 6 activated by the user */
	if (Zone[2].Node[0] == On && Bits[6] == On) { Write_Book( Zone, 0, 0, 15); }		/* Trigger 7 activated by the user */
	if (Zone[2].Node[0] == On && Bits[7] == On) { Write_Book( Zone, 0, 0, 16); }		/* Trigger 8 activated by the user */
	if (Zone[2].Node[0] == On && Nbit == 0x01 ) { Write_Book( Zone, 0, 0, 17); }		/* Trigger 9 activated by the user */
	return (0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Time_Stamp()													   */
/* Details  : Set Hours:Minutes:Seconds for time and log book entries in each zone.						   */
/* Libraries: N/A														   */
/* Variables: Zone[0-2].Hour[0-24], Zone[0-2].Mins[0-60], Zone[0-2].Secs[0-60], Zone[0-2].Line[0].				   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */
	
unsigned int Time_Stamp(struct Alarm Zone[])
{
	Zone[0].Hour[Zone[0].Line[0]] = G_Hours;	/* Write the hours into the time log for Zone 1	  */
	Zone[1].Hour[Zone[1].Line[0]] = G_Hours;	/*                        ""		      2	  */
	Zone[2].Hour[Zone[2].Line[0]] = G_Hours;	/*                        ""		      3	  */
	Zone[0].Mins[Zone[0].Line[0]] = G_Mins;		/* Write the minutes into the time log for Zone 1 */
	Zone[1].Mins[Zone[1].Line[0]] = G_Mins;		/*                        ""			2 */
	Zone[2].Mins[Zone[2].Line[0]] = G_Mins;		/*                        ""			3 */
	Zone[0].Secs[Zone[0].Line[0]] = G_Secs;		/* Write the seconds into the time log for Zone 1 */
	Zone[1].Secs[Zone[1].Line[0]] = G_Secs;		/*                        ""			2 */
	Zone[2].Secs[Zone[2].Line[0]] = G_Secs;		/*                        ""		 	3 */
	return (0);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Comp_Strng()													   */
/* Details  : Check 2 strings the system password and the user entered password if they match return true else return false.	   */
/* Libraries: N/A														   */
/* Variables: String1[], String2[], Charater, MatchValue. 									   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Comp_Strng(const unsigned char String1[], const unsigned char String2[])
{
	auto unsigned int Character;
	auto unsigned int MatchValue;

	Character = 0;
	MatchValue = False;

	while (String1[Character] == String2[Character] && String1[Character] != '\0' && String2[Character] != '\0')
		{
			Character++;
			if (String1[Character] == String2[Character]) {	MatchValue = True;}
				else
					{ MatchValue = False;}
		}
		return (MatchValue);
}

/* ******************************************************************************************************************************* */
/* AUTHOR   : Terence Broadbent.												   */
/* CONTRACT : Stafford University.												   */
/* Version  : 1.0														   */
/* Function : Write_Book()													   */
/* Details  : Write to the log book the short logcodes (values) passed to this function ie what is happening in each function call.*/
/* Libraries: N/A														   */
/* Variables: Zone[0-2].Book[0-2]; Zone[0-2].Line[0], Zcode1, ZCode2, ZCode3. 							   */
/* Modified : N/A														   */
/* ******************************************************************************************************************************* */

unsigned int Write_Book(struct Alarm Zone[], const unsigned int ZCode_0, const unsigned int ZCode_1, const unsigned int ZCode_2)
{
	Time_Stamp(Zone);					/* Time stamp the log entry			*/
	Zone[0].Book[Zone[0].Line[0]] = ZCode_0; 		/* Write the data to the log book about Zone 1	*/
	Zone[1].Book[Zone[1].Line[0]] = ZCode_1;		/* Write the data to the log book about Zone 2	*/
	Zone[2].Book[Zone[2].Line[0]] = ZCode_2;		/* Write the data to the log book about Zone 3	*/
	Zone[0].Line[0]++;					/* Increment Zone 1 log book entry by 1		*/
	if (Zone[0].Line[0] == 100) { Zone[0].Line[0] = 0; }	/* Check 100 entries limit & reset if hit	*/
	Zone[1].Line[0]++;					/* Increment Zone 2 log book entry by 1		*/
	if (Zone[1].Line[0] == 100) { Zone[1].Line[0] = 0; }	* Check 100 entries limit & reset if hit	*/
	Zone[2].Line[0]++;					/* Increment Zone 3 log book entry by 1		*/
	if (Zone[2].Line[0] == 100) { Zone[2].Line[0] = 0; }	/* Check 100 entries limit & reset if hit	*/
	return (0);
}

/* ******************************************************************************************************************************* */
/* #Eof */