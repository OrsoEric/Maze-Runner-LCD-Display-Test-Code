/****************************************************************************
**	ORANGEBOT PROJECT
*****************************************************************************
**	MODULE TESTING
*****************************************************************************
**	Author: 			Orso Eric
**	Creation Date:
**	Last Edit Date:
**	Revision:			1
**	Version:			0.1 ALPHA
****************************************************************************/

/****************************************************************************
**	HYSTORY VERSION
*****************************************************************************
**	R1 V0.1ALPHA
**		>
****************************************************************************/

/****************************************************************************
**	DESCRIPTION
*****************************************************************************
**	This project is meant to test the AT324 Module
**	>Test LCD display
**	>Test USART0 (57Kbs) with loopback
****************************************************************************/

/****************************************************************************
**	USED PIN
**	TIPS: compile this field before writing any code: it really helps!
*****************************************************************************
**
****************************************************************************/

/****************************************************************************
**	USED PHERIPERALS
**	TIPS: compile this field before writing any code: it really helps!
*****************************************************************************
**
****************************************************************************/

/****************************************************************************
**	KNOWN BUG
*****************************************************************************
**	>
****************************************************************************/

/****************************************************************************
**	TODO
*****************************************************************************
**
****************************************************************************/

/****************************************************************************
**	ENVROIMENT VARIABILE
****************************************************************************/

/****************************************************************************
**	INCLUDE
**	TIPS: ".h" should not include other ".h", it lower the leggibility of the code
**	TIPS: ".h" must not contain anything that generate code, write only declaration or prototype
**	this help the leggibility and the debug phase
**	TIPS: type from the stdint.h have a well defined width and signedness, use them
**	( uint8_t = unsigned 8 bit, int8_t = signed 8 bit, uint32_t = unsiged 32 bit, ecc... )
****************************************************************************/

#include "global.h"

/****************************************************************************
**	DEFINE
****************************************************************************/

//define for the mail loop
#define EVER (;;)

/****************************************************************************
**	MACRO
****************************************************************************/

/****************************************************************************
**	STRUCTURE
****************************************************************************/

/****************************************************************************
**	PROTOTYPE: FUNCTION
**	TIPS: use "extern" in function prototype, it's not necessary, but any other
**	prototype need it, it help the leggibility of the code
****************************************************************************/

/****************************************************************************
**	PROTOTYPE: GLOBAL VARIABILE
****************************************************************************/

/****************************************************************************
**	GLOBAL VARIABILE:
**	TIPS: "const" variabile will be loaded in the flash memory, saving the ram,
**	use it for string that will not be modified
**	TIPS: if you want a ISR to manipulate a global variabile, than that variabile
**	**must** be declared "volatile" so that the c compiler will not wipe out the
**	variabile by optimising the code, use that variabile as less as possibile
**	because it will not be optimised
**	TIPS: "volatile int" variabile may give problem, don't use it (uP is 8 bits, while
**	int is 16 bits, it's implemented by concatenating 2 byte, the volatile statement
**	disable the optimisation on that variabile, and mess up that code)
****************************************************************************/

//Volatile flags used by ISRs
volatile Isr_flags flags;

	///----------------------------------------------------------------------
	///	BUFFERS
	///----------------------------------------------------------------------
	//	Buffers structure and data vectors

//Safe circular buffer for UART input data
volatile At_buf8_safe uart_rx_buf;
//Safe circular buffer for uart tx data
At_buf8 uart_tx_buf;
//allocate the working vector for the buffer
U8 v0[ UART_RX_BUF_SIZE ];
//allocate the working vector for the buffer
U8 v1[ UART_TX_BUF_SIZE ];

/****************************************************************************
**	MAIN
****************************************************************************/

int main( void )
{
	///----------------------------------------------------------------------
	///	LOCAL VARS
	///----------------------------------------------------------------------
		///TEMP vars
	U8 u8t;
	//Index to the LCD display (serial terminal)
	U8 lcd_index;
	//prescaler
	U8 pre;
	//debug counter
	S8 scnt;

    ///----------------------------------------------------------------------
	///	VARS INIT
	///----------------------------------------------------------------------

		///UART RX BUFFER INIT
		//I init the rx and tx buffers
	//attach vector to buffer
	AT_BUF_ATTACH( uart_rx_buf, v0, UART_RX_BUF_SIZE);
	//attach vector to buffer
	AT_BUF_ATTACH( uart_tx_buf, v1, UART_TX_BUF_SIZE);

	lcd_index 	= 0;		//Index to the LCD display (serial terminal)
	pre 		= 0;		//counter prescaler
	scnt 		= -128;		//debug display counter

	///----------------------------------------------------------------------
	///	DEVS INIT
	///----------------------------------------------------------------------

	//Init all pins, init all devices
	global_init();

	lcd_print_str( LCD_POS( 0, 0), (U8 *)"Maze Runner");

	///----------------------------------------------------------------------
	///	MAIN LOOP
	///----------------------------------------------------------------------

	//Main Loop
	for EVER
	{
		///----------------------------------------------------------------------
		/// LCD Display Driver
		///----------------------------------------------------------------------
		//	Sync the display content with the user structure

		//If: LCD update flag
		if (flags.lcd_update == 1)
		{
			//Clear flag
			flags.lcd_update = 0;
			//Driver that sync the user structure with the LCD.
			//This paradigm solve lots of timing problems of the direct call version.
			//You can print a million time a second, and the driver still won't bug out
			lcd_update();
		}	//End If: LCD update flag

		///----------------------------------------------------------------------
		///	100mS Tick
		///----------------------------------------------------------------------

		//If: 100mS tick
		if (flags.tick_100m == 1)
		{
			//Clear flag
			flags.tick_100m = 0;
			//display a debug number
			lcd_print_s8( LCD_POS( 1, 12 ), scnt);
			//advance debug counter
			scnt = (scnt < 127)?(scnt +1):(-128);
			//advance prescaler (10 counts then reset)
			pre = (pre <= (10-1))?(pre +1):(0);
			//when prescaler is reset (1s)
			if (pre == 0)
			{
				//DEBUG: send a data trough the UART
				AT_BUF_PUSH( uart_tx_buf, 'Z' );
				//Toggle leds
				TOGGLE_BIT( PORTB, PB6 );
				TOGGLE_BIT( PORTB, PB7 );
			}
		}	//Endif: 100mS tick

		///----------------------------------------------------------------------
		/// UART RX
		///----------------------------------------------------------------------
		//	Handle RX from RS232
		//	Loopback

		//if: uart rx buffer is not empty
		if ( AT_BUF_NUMELEM( uart_rx_buf ) > 0)
		{
				///Get data
			//Get the byte from the RX buffer (ISR put it there)
			u8t = AT_BUF_PEEK( uart_rx_buf );
			AT_BUF_KICK_SAFER( uart_rx_buf );
				///Led
			//DEBUG: toggle the led every time a data is popped from RX buffer
			TOGGLE_BIT( PORTC, 0 );
				///Loopback
			//Push into tx buffer
			AT_BUF_PUSH( uart_tx_buf, u8t );
				///Terminal on LCD
			//Write on LCD
			lcd_print_char( LCD_POS( 0, lcd_index), u8t );
			//Advance LCD col
			lcd_index = (lcd_index <= (15 -1))?(lcd_index++):(0);
		}	//end if: uart rx buffer is not empty

		///----------------------------------------------------------------------
		/// UART TX
		///----------------------------------------------------------------------

		//if: the Uart0 HW buffer is empty and the UART tx buffer is not empty
		if ( (UART0_TX_READY()) && (AT_BUF_NUMELEM( uart_tx_buf ) > 0) )
		{
			//Get the byte to be filtered out
			u8t = AT_BUF_PEEK( uart_tx_buf );
			AT_BUF_KICK( uart_tx_buf );
			//Write on UART tx buffer.
			UDR0 = u8t;
		}	//End If: uart tx
	}	//end for: for EVER

	return 0;
}	//end main

/****************************************************************************
** FUNCTIONS:
****************************************************************************/
