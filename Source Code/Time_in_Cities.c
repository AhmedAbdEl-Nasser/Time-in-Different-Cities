////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////// ---------->>>>>>>>>> INCLUDES <<<<<<<<<<---------- //////////


// FreeRTOS.org includes
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

// C libraries includes
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Stellaris includes
#include "TM4C123GH6PM.h"


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////// ---------->>>>>>>>>> AAPLICATION DATA <<<<<<<<<<---------- //////////


// Queue handlers
static QueueHandle_t queue1;
static QueueHandle_t queue2;

// Time message structure
// Time components (HH:MM:SS)
typedef struct Message {
	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;
} TimeMessage;

// World cities
char cities[][10] = {"London", "Paris", "Madrid", "Rome", "Athens", "Ankara", "Istanbul", "Cairo", "Moscow", "Tehran"};

// Time Differences from London city
char time_diff[] = {0, 1, 1, 1, 2, 2, 2, 2, 3, 4};

// User city selection
// Global variable that represents user's selection from options menu
unsigned int selection;


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////// ---------->>>>>>>>>> TIME CONTROLLER TASK FUNCTION <<<<<<<<<<---------- //////////


void TimeTask(void *pvParameters) {
	
	// The time message
	// To be recieved initially from the UART via queue2
	TimeMessage current_time;
	xQueueReceive(queue2, &current_time, portMAX_DELAY);
	
	for(;;) {
		
		// The idea is that time gets updated every 1 second
		vTaskDelay(pdMS_TO_TICKS(1000));
		
		// Then we can increment the current time by one second
		current_time.seconds++;
		
		// Now, we need to mimic the clock
		// Every 60 seconds increments one minute
		// Every 60 minutes increments one Hour
		// Every day (24 hours) reset the clock
		// Note that we are using the 24 hours time format
		// And that's it :D
		if(current_time.seconds == 60) {
			
			current_time.seconds = 0;	// Reset the seconds to count a new minute
			current_time.minutes++;		// Increment the current time by 1 minute
			
			// Keep incrementing minutes untill we reach one hour (60 minutes)
			if(current_time.minutes == 60) {
				current_time.minutes = 0;	// Reset the minutes to count a new hour
				current_time.hours++;			// Increment the current time by 1 hour
				
				// Keep incrementing hours untill we reach one day (24 hours)
				if(current_time.hours == 24) {
					current_time.hours = 0; // Reset the hours to count a new day
				}
			}
		}
		
		// The current time is updated on the LCD display every 1 second
		// Send current time to the LCD controller to display
		xQueueSend(queue1, &current_time, 0);
		
	}
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////// ---------->>>>>>>>>> LCD CONTROLLER TASK FUNCTION <<<<<<<<<<---------- //////////


#include "LCD.h"


void LCDTask(void *pvParameters) {
	
	TimeMessage current_time; // Current time to display on the LCD
	char time_as_string[7];			 // The time string in "char" type
	
	
	// Initialize the LCD
	// Communication via portB
	LCDInit();			// Init
	//LCDClear();			// Clear display
	selection = 0;	// Reset city selection to London
	
	// What this task in intended to do is simply three things
	// Acquire current time from the current controller task via queue1
	// Calculate time differences from london 
	// and convert the time format into string format that is suitable to be displayed
	// Print the selected city current time on the LCD display
	for (;;) {
		
		xQueueReceive(queue1, &current_time, portMAX_DELAY); // Get current time
		
		// Note that initially the LCD displays the time in London
		// Which is given by the user at the beginning
		// After that it displays the selected city by user
		LCDPrintString(cities[selection]);
		
		// Adjust each time component and display it
		
		// Hours adjustment
		current_time.hours += time_diff[selection]; 														// calculate time difference
		if(current_time.hours > 23) { current_time.hours -= 24; }								// Cycle over one day (24 hours)
		memcpy(time_as_string, &current_time.hours, sizeof current_time.hours);	// Convert to string
		RemoveWhiteSpaces(time_as_string);																			// Remove white Spaces
		// If (hours < 10 for example 9) -> (hours = 09);
		if(current_time.hours < 10) {
			time_as_string[1] = time_as_string[0];																// Move to second digit
			time_as_string[0] = '0';																							// Insert leadig zero at first digit
			time_as_string[2] = '\0';																							// Concatenate the null character
		}
		LCDPrintString(time_as_string);																					// Display hours
		LCDPrintChar(':');																											// Time separator
		
		// Minutes adjustment
		// Note that minutes are not affected by time differences
		memcpy(time_as_string, &current_time.minutes, sizeof current_time.minutes);	// Convert to string
		RemoveWhiteSpaces(time_as_string);																					// Remove white Spaces
		// If (minutes < 10 for example 9) -> (minutes = 09)
		if(current_time.minutes < 10) {
			time_as_string[1] = time_as_string[0];																		// Move to second digit
			time_as_string[0] = '0';																									// Insert leadig zero at first digit
			time_as_string[2] = '\0';																									// Concatenate the null character
		}
		LCDPrintString(time_as_string);																							// Display hours
		LCDPrintChar(':');																													// Time separator
		
		// Seconds adjustment
		// Note that seconds are not affected by time differences
		memcpy(time_as_string, &current_time.seconds, sizeof current_time.seconds);	// Convert to string
		RemoveWhiteSpaces(time_as_string);																					// Remove white Spaces
		// If (seconds < 10 for example 9) -> (seconds = 09)
		if(current_time.seconds < 10) {
			time_as_string[1] = time_as_string[0];																		// Move to second digit
			time_as_string[0] = '0';																									// Insert leadig zero at first digit
			time_as_string[2] = '\0';																									// Concatenate the null character
		}
		LCDPrintString(time_as_string);																							// Display hours
		// No need for (:) the time separator at the end
		
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////// ---------->>>>>>>>>> UART CONTROLLER TASK FUNCTION <<<<<<<<<<---------- //////////


#include "UART.h"


void UARTTask(void *pvParameters) {
	
	// UART initialization
	UARTInit();
	
	// Acquire initial time from user (London)
	UARTPrintString("\n\rWELCOME TO TIME IN CITIES APPLICATION");
	UARTPrintString("\n\r========================================");
	UARTPrintString("\rPlease enter current time in london with the format (hh:mm:ss):");
	
	// Read initial time from user
	char message_buffer[10];	// Input message buffer
	TimeMessage init_time;		// Initial time message
	ReadTime(message_buffer);	// Read time
	// Now we have the initial time as meaningless characters
	// We need to extract time data from it
	// Note that each 2 characters represent a time component
	init_time.hours = 10*(message_buffer[0] - '0') + message_buffer[1] - 0;		// Convert hours
	init_time.minutes = 10*(message_buffer[3] - '0') + message_buffer[4] - 0;	// Convert minutes
	init_time.seconds = 10*(message_buffer[6] - '0') + message_buffer[7] - 0;	// Convert seconds
	
	// Send the init time to the Time controller task via queue2
	xQueueSend(queue2, &init_time, 0);
	
	// All this was initial
	// What this task should be doing all the time (looping)
	// is to display options menu and wait for user selection
	for (;;) {
		
		// Send the application control window to PC
		// Consisting of city options menu to select from
		UARTPrintString("\n\r\n\r\n\rPlease select a city to view its time:\n\r");
		for(int i = 0; i < 10; i++) {
			UARTPrintString("\n\r");
			UARTTransmit(i + '0');
			UARTPrintString(". ");
			UARTPrintString(cities[i]);
		}
		UARTPrintString("\n\r\n\rSelection: ");
		
		// Read user's selected city
		selection = ReadSelection();
		
	}
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////// ---------->>>>>>>>>> MAIN FUNCTION <<<<<<<<<<---------- //////////


// Initializes everything
// Including
// Invokes the scheduler
int main( void ) {
	
	// Define the queues
	queue1 = xQueueCreate(1, 8); // Queue1 with 1 max itemset of size 8 bytes
	queue2 = xQueueCreate(1, 8); // Queue2 with 1 max itemset of size 8 bytes
	
	// Tasks creation
	// All tasks are created with the following parameters:
	// Task function, Task name, Minimal stack size, Null parameters, Highest priority, Null handler
	// Note that the 3 tasks have the same priority
	
	// Task 1 -> the Time controller task
	//xTaskCreate(TimeTask, "Time Controller", 20, NULL, configMAX_PRIORITIES, NULL);
	
	// Task 2 -> the LCD controller task
	//xTaskCreate(LCDTask, "LCD Controller", 20, NULL, configMAX_PRIORITIES, NULL);
	
	// Task 3 -> the UART controller task
	xTaskCreate(UARTTask, "UART Controller", 20, NULL, configMAX_PRIORITIES, NULL);
	
	// Invoke the FreeRTOS scheduler
	vTaskStartScheduler();
	
	for( ;; ); // Should never reach here
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////