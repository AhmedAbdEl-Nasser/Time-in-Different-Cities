////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

////////// ---------->>>>>>>>>> LCD HELPING FUNCTIONS HEADER FILE <<<<<<<<<<---------- //////////


// Stellaris includes
#include "TM4C123GH6PM.h"

// PortB pins mapping to LCD pins
#define LCD GPIOB	// LCD as symbolic name for GPIO PortB
#define RS 0x01		// RS pin connected to PortB Pin 0
#define RW 0x02		// RW pin connected to PortB Pin 1
#define EN 0x04		// EN pin connected to PortB Pin 2
// LCD Data pins (4:7) connected to GPIO PortB Data register pins (4:7)

// Some useful symbolic names for LCD command
// Instead of using meaningless command numbers
#define ClearDisplay			0x01
#define MoveCursorRight		0x06
#define CursorBlink       0x0F
#define FunctionSet4bit 	0x28
#define SetFontSize5x7    0x20


// APIs to LCD functions
void LCDInit(void);																							// LCD initialization
void LCDWriteNibble(unsigned char data, unsigned char control); // Writes 4-bits
void LCDCommand(unsigned char command);													// Send commands to LCD						
void LCDPrintChar(unsigned char data); 													// Prints ASCII character
void LCDPrintString (char *string);															// Prints full string
void RemoveWhiteSpaces(char *string);														// String white spaces removal
void DelayMS(int n);																						// Delay for specified mili seconds
void DelayUS(int n);																						// Delay for specified micro seconds