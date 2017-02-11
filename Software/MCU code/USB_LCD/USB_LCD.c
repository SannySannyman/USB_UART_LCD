/*
 * USB_LCD.c
 *
 * Created: 19.04.2016 16:36:21
 *  Author: Oleksandr
 */ 

#define UART_BAUD_RATE      9600
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <string.h>

#include "bits.h"
#include "uart.h"
#include "lcd.h"

#define LCD_LINES           4
#define LCD_DISP_LENGTH    20

/*LCD pins defines*/
#define LCD_DATA0_PORT   PORTC
#define LCD_DATA0_PIN    4

#define LCD_DATA1_PORT   PORTC
#define LCD_DATA1_PIN    5

#define LCD_DATA2_PORT   PORTD
#define LCD_DATA2_PIN    2

#define LCD_DATA3_PORT   PORTD
#define LCD_DATA3_PIN    3

#define LCD_RS_PORT      PORTC
#define LCD_RS_PIN       1

#define LCD_RW_PORT      PORTC
#define LCD_RW_PIN       2

#define LCD_E_PORT       PORTC
#define LCD_E_PIN        3

/*LCD back light port*/
#define LCD_LED PORTD4

/*global variable for status flags*/
volatile uint8_t mainFlags = 0;			
#define TEXT_STARTED		0
#define NO_CONNECTION		1
#define BACKLIGHT_STARTED	2

#define LCD_ROW_N		LCD_LINES				
#define LCD_COL_N		LCD_DISP_LENGTH			
#define LCD_BUFFER_LEN  LCD_ROW_N*LCD_COL_N+4

#define BL_BUFFER_LEN	4


/*ASCII codes for control symbols*/
#define START_TEXT		0x02
#define NEW_LINE		0x0A
#define END_TEXT		0x03
#define CLEAR_TEXT		0x7F
#define START_BACKLIGHT	0x11
#define END_BACKLIGHT	0x12

typedef enum
{
	OFF, 
	ON,
	RESET
} state_t;
	
volatile uint16_t timer_cnt = 0;	
volatile uint8_t bl_brightness = 0xFF;


/*************************************************************************
* Function Name: setTimer2sec()
**************************************************************************
* Summary:
* This API is for timer initialization.
*
* Parameters:
* state_t state - new state of timer
*
* Return:
* None.
*************************************************************************/
void setTimer2sec(state_t state)
{
	switch (state)
	{
		case ON:
		{
			set_bit(TCCR2, CS20);		/*on timer, no prescaling (interrupts 31.25KHz at 8MHz clk)*/
			set_bit(TIMSK, TOIE2);		/*timer overflow interrupt enable*/
		}
		break;
		
		case OFF:
			unset_bit3(TCCR2, CS22, CS21, CS20);	/*OFF timer clk*/
		break;
		
		case RESET:
		{
			timer_cnt = 0;
		}
	}
}

/*************************************************************************
* Function Name: LCD_PrintString(char *lcd_buffer, uint8_t len)
**************************************************************************
* Summary:
* This function for printing string on charLCD with new line symbol.
*
* Parameters:
* char *lcd_buffer - pointer to lcd buffer array
* uint8_t len - number of symbols in array
*
* Return:
* None.
*
*************************************************************************/
void LCD_PrintString(char *lcd_buffer, uint8_t len)
{
	uint8_t lcd_buffer_position = 0;
	uint8_t temp = 0;

	lcd_clrscr();

	for (uint8_t row_cnt = 0; row_cnt<LCD_ROW_N; row_cnt++)
	{
		for(uint8_t column_cnt = 0; column_cnt<LCD_COL_N+1; column_cnt++)
		{
			if (lcd_buffer_position < len)
			{
				temp = lcd_buffer[lcd_buffer_position];
				
				if(temp == NEW_LINE)
				{
					lcd_buffer_position++;
					break;
				}
				else if(column_cnt< LCD_COL_N)
				{
					lcd_buffer_position++;
					lcd_gotoxy(column_cnt, row_cnt);
					lcd_putc(temp);
				}
			}

		}/*end column_cnt loop*/

	}/*end row_cnt loop*/
}


/*************************************************************************
* Function Name: ISR(TIMER2_OVF_vect)
**************************************************************************
* Summary:
* This is ISR for TIMER2 overflow event.
*
*************************************************************************/
ISR(TIMER2_OVF_vect)
{
	timer_cnt++;
	if (timer_cnt>=64000)						
	{
		/*approx every 2 sec*/
		/*if no new data*/
		set_bit(mainFlags, NO_CONNECTION);
		timer_cnt = 0;
		unset_bit(mainFlags, BACKLIGHT_STARTED);	/*????*/
	}
	
	static uint8_t cnt = 0;				/**/
										/**/
	if (cnt<bl_brightness)				/*software PWM for LCD back light brightness control*/
	set_bit(PORTD, LCD_LED);			/*FIXED error in PCB layout*/
	else								/**/
	unset_bit(PORTD, LCD_LED);			/**/
										/**/
	cnt++;								/**/
	if (cnt>=50)						/**/
		cnt = 0;						/**/
}



/*************************************************************************
* Function Name: main(void)
**************************************************************************
* Summary:
* This is main function, receive data from UART, convert and put it on character LCD
*
* Parameters:
* None.
*
* Return:
* None.
*
*************************************************************************/
int main(void)
{
	uint16_t	temp = 0;
	char		lcd_buffer[LCD_BUFFER_LEN];
	char		bl_brightness_buff[BL_BUFFER_LEN];
	uint8_t		buffer_cnt = 0;
	
	uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) );	
	lcd_init(LCD_DISP_ON);
	set_bit(DDRD, LCD_LED);
	set_bit(mainFlags, NO_CONNECTION);
	setTimer2sec(ON);

	sei();
	
    while(1)
    {
		if (check_bit(mainFlags, NO_CONNECTION))
		{
			unset_bit(mainFlags, NO_CONNECTION);					
			lcd_clrscr();
			lcd_gotoxy(0,0);							
			lcd_puts_P("  No connection ");
		}
		
		temp = uart_getc();
		if (!(temp & 0xFF00))
        {
			/*if no UART errors get received char*/
			char in_char = (unsigned char)temp;	
	
			switch(in_char)
			{
				case START_TEXT:
					/*prepare for receiving text string*/
					set_bit(mainFlags, TEXT_STARTED);	
					buffer_cnt = 0;	
					break;
			
				case END_TEXT:	
					if (mainFlags&(1<<TEXT_STARTED))
					{
						/*if text was started, reset NO_CONNECTION timer and
						* put buffer content on display*/

						setTimer2sec(RESET);
						LCD_PrintString(lcd_buffer, buffer_cnt);
						unset_bit(mainFlags, TEXT_STARTED);
					}
					buffer_cnt = 0;
					break;
				
				case CLEAR_TEXT:
					lcd_clrscr();
					break;
				
				case START_BACKLIGHT:
					set_bit(mainFlags, BACKLIGHT_STARTED);
					buffer_cnt = 0;
					break;
				
				case END_BACKLIGHT:
					unset_bit(mainFlags, BACKLIGHT_STARTED);
					bl_brightness = atoi(bl_brightness_buff);
					
					memset(bl_brightness_buff, 0, BL_BUFFER_LEN);
					buffer_cnt = 0;
					break;
						
				default: /*other chars*/
				{
					if (check_bit(mainFlags, BACKLIGHT_STARTED))
					{
						if (buffer_cnt < BL_BUFFER_LEN)
						{
							bl_brightness_buff[buffer_cnt] = in_char;
						}
					}	
					else if (check_bit(mainFlags, TEXT_STARTED))
					{
						lcd_buffer[buffer_cnt] = in_char;
					}
					
					if (buffer_cnt >= LCD_BUFFER_LEN)
						buffer_cnt = 0;	
					else
						buffer_cnt++;
							
				}
				
			}/*end switch*/
			
		}/*end if()*/
			
	}/*end while(1)*/
	
}/*end main*/