# USB_UART_LCD
Info char display for PC. Receive data from USB via FT232RL, convert it and print on LCD. Supporting 16x02 and 20x04 LCD with HD44780 controller.

- UART baud rate - 9600

# DATA FORMAT:
#### TEXT DATA:
	{START_TEXT} {text string} {END_TEXT}
		START_TEXT  - 0x02
		text string - up to 80 symbols with up to 4 new line symbols(\n, 0x0A) for 20x04 LCD
		END_TEXT    - 0x03

		Clear text	- only CLEAR_TEXT symbol
			CLEAR_TEXT	- 0x7F

#### BACKLIGHT BRIGHTNESS DATA:
	{START_BACKLIGHT}{text value}{END_BACKLIGHT}
		START_BACKLIGHT - 0x11
		text value      - PWM pulse width - 0 to 49
		END_BACKLIGHT   - 0x12

