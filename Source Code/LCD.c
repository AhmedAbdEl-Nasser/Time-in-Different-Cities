///////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////// ---------->>>>>>>>>> LCD HELPING FUNCTIONS IMPLEMENTATION <<<<<<<<<<---------- //////////

// LCD header file inclusion
#include "LCD.h"


// LCD initialization
// Bassically initializes PortB to communicate with the LCD
// Then sends some init commands to it
void LCDInit(void) {
	
	// PortB initialization
	SYSCTL->RCGCGPIO |=(1<<1);	// Clock gating enable to GPIO PortB
	LCD->DEN |=0xFF;						// Digital enable PortB pins
	LCD->DIR |=0xFF;						// Set PortB pins as output
	
	// Lcd init commands
	LCDCommand(FunctionSet4bit);	// Select 4-bit mode (pins 4:7)
	LCDCommand(SetFontSize5x7);		// Font size 5x7 and 2 rows of LCD
	LCDCommand(MoveCursorRight);	// shift cursor right
	LCDCommand(ClearDisplay); 		// Clear the display
	LCDCommand(CursorBlink);  		// Enable the display by blinking the cursor
 
}


// Data is sent to LCD as two nibbles of data
// accompanied with control commands via the EN and RS pins
void LCDWriteNibble(unsigned char data, unsigned char control) {
	
	data &= 0xF0;											// Extract data upper nibble
  control &= 0x0F;    							// Extract control lower nibble
  LCD->DATA = data | control;				//	Set RS and RW to zero for write operation
  LCD->DATA = data | control | EN;	// Provide Pulse to Enable pin to perform write operation
  DelayUS(0);
  LCD->DATA = data; 								// Send data to LCD
  LCD->DATA = 0; 										// Stop writing data to LCD
	
}


// Send a command to the LCD
// The same nibble idea applies
void LCDCommand(unsigned char command) {
	
	LCDWriteNibble(command & 0xF0, 0);	// Write upper nibble to LCD
  LCDWriteNibble(command << 4, 0);  	// Write lower nibble to LCD
	
  if (command < 4)
		DelayMS(2);
  else
		DelayUS(40);
	
}


// Print a character on LCD display
void LCDPrintChar(unsigned char data) {
	
    LCDWriteNibble(data & 0xF0, RS);	// Writes upper nibble with RS = 1 to write data
    LCDWriteNibble(data << 4, RS);    // Writes lower nibble with RS = 1 to write data
    DelayUS(40);
	
}


// Print a full string on LCD display
// by recursively calling LCDPrintChar
void LCDPrintString(char *string) {
	
	// Send each char of the string till the NULL terminator
	while(*string) {
		LCDPrintChar(*(string++));
	}
	
}


// String white spaces removal
// Its idea is simple
// Replace the white space with next character
// Propagate the white space
// Terminate the string with null character to exclude the white spaces
void RemoveWhiteSpaces(char *string)
{
	int count = 0;
	
	for (int i = 0; string[i]; i++) {
		if (string[i] != ' ') {
			string[count++] = string[i];
		}
	}
	string[count] = '\0';
}


// Mili seconds delay
void DelayMS(int n)
{
	int i,j;
	for(i = 0; i < n; i++)
	for(j = 0; j < 3180; j++) {}
}

// Micro seconds delay
void DelayUS(int n)
{
	int i,j;
	for(i = 0; i < n; i++)
	for(j = 0; j < 3; j++)	{}
 
}