////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////// ---------->>>>>>>>>> UART HELPING FUNCTIONS IMPLEMENTATION <<<<<<<<<<---------- //////////


// UART header file inclusion
#include "UART.h"


// UART initialization
void UARTInit() {
	
	SYSCTL->RCGCUART |= 0x01;  // Clock gating enable to UART0
  SYSCTL->RCGCGPIO |= 0x01;  // Clock gating enable to GPIO PortA
	
  // UART0 configuration
  UART0->CTL = 0;			// Disable the UART0 module to configure
  UART0->IBRD = 104;  // For 9600 baud rate, integer = 104
  UART0->FBRD = 11;   // For 9600 baud rate, fractional = 11
  UART0->CC = 0;      // Use system clock
  UART0->LCRH = 0x60; // Configure data lenght as 8-bit, with no parity bit, and disable FIFO
  UART0->CTL = 0x301; // Enable the UART0 module, alongside with Rx and Tx

  // PortA configuration to use as UART
	// Using Pin0 and Pin1 from the USB communication
  GPIOA->DEN = 0x03;				// Digital enable Pin0 and Pin1
	GPIOA->AMSEL = 0x03; 			// Turn off analg function
  GPIOA->AFSEL = 0x03;  		// Alter pins function, WHICH BY DEFAULT ARE UART
  GPIOA->PCTL = 0x00000011; // Configure pins for UART use
	
}


// Character transmission using UART
void UARTTransmit( uint8_t message_data ) {
	
	while((UART0->FR & 0x20) != 0);	// Wait for the buffer to be not full (TXFF = 0)
	UART0->DR = message_data;				// Transmit a character
	
}


// Print a full string
// Uses the character transmission function
void UARTPrintString(char *out_message) {
	
	// Print untill null character (end of the string)
	while(*out_message) {
		UARTTransmit(*(out_message++));
	}
	
}


// Recieve a character through UART
char UARTRecieve() {
	
	char recieved_data;
	
	while((UART0->FR & 0x10) != 0);	// Wait for a new input (RXFE = 0)
	recieved_data = UART0->DR;			// Recieve a character
	
	return (unsigned char)recieved_data;
	
}


// Time reading function
// To read initial time from user
// Terminates reading when user hits "Enter"
void ReadTime(char message_buffer[]) {
	
	unsigned char in_char;	// Character to read
	unsigned char i = 0;		// Message buffer iterator
	
	// Keep reading untill user hits "Enter"
	for(;;) {
		in_char = UARTRecieve();			// Recive characters from user
		UARTTransmit(in_char);				// Echo user's input on terminal
		if( in_char == '\r') break;		// Check if "Enter" is hit to terminate
		message_buffer[i] = in_char;	// Add the input char to message
		i++;
	}
	
	message_buffer[i] = '\0';	// Concatenate the null character
	
}


// City selection reading function
// Simply reads intiger number that represents the selected city
unsigned int ReadSelection() {
	
	unsigned int selection = 0;	// The selection integer
	unsigned char in_char;			// The UART input
	
	for(;;) {
		in_char = UARTRecieve();						// Recive characters from user
		UARTTransmit(in_char);							// Echo user's input on terminal
		if( in_char == '\r') break;					// Check if "Enter" is hit to terminate
		in_char = in_char - '0';						// Pure number
		selection = 10*selection + in_char;	// Int numberic value from char
	}
	
	return selection;
	
}