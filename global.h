#ifndef GLOBAL_H
	//header envroiment variabile, is used to detect multiple inclusion
	//of the same header, and can be used in the c file to detect the
	//included library
	#define GLOBAL_H

	/****************************************************************************
	**	ENVROIMENT VARIABILE
	****************************************************************************/

	#define F_CPU 20000000

	//Enable to test reading on the ADC module
	//#define TEST_ADC

	/****************************************************************************
	**	GLOBAL INCLUDE
	**	TIPS: you can put here the library common to all source file
	****************************************************************************/

	//type definition using the bit width and signedness
	#include <stdint.h>
	//define the ISR routune, ISR vector, and the sei() cli() function
	#include <avr/interrupt.h>
	//name all the register and bit
	#include <avr/io.h>
	//hard delay
	#include <util/delay.h>
	//General porpouse macros
	#include "at_utils.h"
	//Library to control a 16x2 LCD display
	#include "at_lcd.h"
	//Basic string manipulation
	#include "at_string.h"
	//AT Mega specific MACROS
	#include "at_mega_port.h"

	/****************************************************************************
	**	DEFINE
	****************************************************************************/

		///----------------------------------------------------------------------
		///	BUFFERS
		///----------------------------------------------------------------------

	#define UART_RX_BUF_SIZE	16
	#define UART_TX_BUF_SIZE	8

		///----------------------------------------------------------------------
		///	ADC
		///----------------------------------------------------------------------


	/****************************************************************************
	**	MACRO
	****************************************************************************/

		///----------------------------------------------------------------------
		///
		///----------------------------------------------------------------------


	/****************************************************************************
	**	TYPEDEF
	****************************************************************************/

	//Global flags raised by ISR functions
	typedef struct _Isr_flags Isr_flags;

	/****************************************************************************
	**	STRUCTURE
	****************************************************************************/

	//Global flags raised by ISR functions
	struct _Isr_flags
	{
		//First byte
		U8 lcd_update	: 1;	//Execute LCD update routine 9765[Hz]
		U8 tick_100m	: 1;	//raised every 100mS
		U8 				: 6;	//unused bits
	};

	/****************************************************************************
	**	PROTOTYPE: INITIALISATION
	****************************************************************************/

	//port configuration and call the pheriperals initialisation
	extern void global_init( void );

	//9765 Hz Time Base
	extern void timer0_init( void );
	//UART communication
	extern void usart0_init( void );
	//Init the ADC module
	extern void adc_init( void );

	/****************************************************************************
	**	PROTOTYPE: FUNCTION
	****************************************************************************/

		///----------------------------------------------------------------------
		///
		///----------------------------------------------------------------------

	/****************************************************************************
	**	PROTOTYPE: GLOBAL VARIABILE
	****************************************************************************/

		///----------------------------------------------------------------------
		///
		///----------------------------------------------------------------------

	//Volatile flags used by ISRs
	extern volatile Isr_flags flags;

		///BUFFERS
	//Safe circular buffer for RS485 input data
	extern volatile At_buf8_safe uart_rx_buf;
	//Safe circular buffer for RS485 output data
	extern At_buf8 uart_tx_buf;

#else
	#warning "multiple inclusion of the header file global.h"
#endif
