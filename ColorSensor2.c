// Jack Berkowitz
// Final Project, Color Sensor, Mechatronics
// 4/3/24 
/******************************************************************** 
This program sets up the ATmega328P as a Main device on the TWI (I2C) bus.
The program sends an arbitrary data byte to a Secondary device on the bus, then reads a data byte from a Secondary device on the bus.
 */

#include <avr/io.h>

/** Function Declarations ***/
void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_repeated_start(void);
void i2c_write_to_address(unsigned char address);
void i2c_read_from_address(unsigned char address);
void i2c_write_data(unsigned char data);
unsigned char i2c_read_data(unsigned char ack);

void wait(volatile int);



int main() 
{
			
	DDRD = 0b00001111; //Pins 0,1,2 set as output for LED's (checking red, green, blue)
	PORTD = 0b00001111; //setting output of Port D high to turn off LEDs (wired active low)
						//Red = Pin 0, Green = Pin 1, Blue = Pin 2
						//Checker = Pin 3
	
	// I2C (TWI) Setup
	//If needed, turn on TWI power: PRR = PRR & 0b01111111; // Ensure that TWI is powered on (PRTWI = 0)
	DDRC = 0b00000000; // Define all PORTC bits as input (specifically PC4 and PC5)
	PORTC = PORTB | 0b00110000;  // set internal pull-up resistors on SCL and SDA lines (PC5 and PC4) ofr I2C bus
	i2c_init();
	
	// I2C Write data as Main. Writing to Enable Register of RGB Color Sensor
	i2c_start();
	i2c_write_to_address(0x29); //Address the RGB Chip
	i2c_write_data(0x00);  //Address 0x00: Enable Register
	i2c_write_data(0b00000011);  //Enabling RGBC, turning power on
	i2c_stop();
	
	while(1)
	{	
		//I2C Read data as Main. Reading from RGB Color Sensor
		
		//1: Low Clear/High Clear
		i2c_start(); 
		i2c_write_to_address(0x29); //Address the RGB Color Sensor
		i2c_write_data(0x14);  //Address 0x14: Clear Data Low Byte
		i2c_repeated_start();
		i2c_read_from_address(0x29); //Address the RGB Color Sensor Again
		unsigned char LowerClear = i2c_read_data(1);  // read with NO_ACK
		
		i2c_write_to_address(0x29); //Address the RGB Color Sensor
		i2c_write_data(0x15);  // Address 0x15: Clear Data High Byte
		i2c_repeated_start();
		i2c_read_from_address(0x29); //Address the RGB Color Sensor Again
		unsigned char UpperClear = i2c_read_data(0);  // read with NO_ACK
		i2c_stop();
		
		//unsigned int clear = UpperClear*256 + LowerClear;
		//if(clear < 256)
		//{
		//	PORTD = 0b11111110; //turn LED 3 on?
		//}
		
		//2: Low Red/High Red
		i2c_start();
		i2c_write_to_address(0x29); //Address the RGB Color Sensor
		i2c_write_data(0x16);  // Address 0x16: Red Data Low Byte
		i2c_repeated_start();
		i2c_read_from_address(0x29); //Address the RGB Color Sensor Again
		unsigned char LowerRed = i2c_read_data(1);  // read with NO_ACK
		
		i2c_write_to_address(0x29); //Address the RGB Color Sensor
		i2c_write_data(0x17);  // Address 0x17: Red Data High Byte
		i2c_repeated_start();
		i2c_read_from_address(0x29); //Address the RGB Color Sensor Again
		unsigned char UpperRed = i2c_read_data(0);  // read with NO_ACK
		i2c_stop();
		
		unsigned int red = UpperRed*256 + LowerRed;
		if(red < 65535)
		{
			PORTD = 0b11111110; //turn LED 0 on
			unsigned int check_red = red;
		}	

		//3: Low Green/High Green
		i2c_start();
		i2c_write_to_address(0x29); //Address the RGB Color Sensor
		i2c_write_data(0x18);  // Address 0x18: Green Data Low Byte
		i2c_repeated_start();
		i2c_read_from_address(0x29); //Address the RGB Color Sensor Again
		unsigned char LowerGreen = i2c_read_data(1);  // read with NO_ACK
		
		i2c_write_to_address(0x29); //Address the RGB Color Sensor
		i2c_write_data(0x19);  // Address 0x19: Green Data High Byte
		i2c_repeated_start();
		i2c_read_from_address(0x29); //Address the RGB Color Sensor Again
		unsigned char UpperGreen = i2c_read_data(0);  // read with NO_ACK
		i2c_stop();
		
		unsigned int green = UpperGreen*256 + LowerGreen;
		if(green < 65535)
		{
			PORTD = 0b11111101; //turn LED 1 on
			unsigned int check_green = green;
		}
		
		
		//4: Low Blue/High Blue
		i2c_start();
		i2c_write_to_address(0x29); //Address the RGB Color Sensor
		i2c_write_data(0x1A);  // Address 0x1A: Blue Data Low Byte
		i2c_repeated_start();
		i2c_read_from_address(0x29); //Address the RGB Color Sensor Again
		unsigned char LowerBlue = i2c_read_data(1);  // read with NO_ACK
		
		i2c_write_to_address(0x29); //Address the RGB Color Sensor
		i2c_write_data(0x1B);  // Address 0x1B: Blue Data High Byte
		i2c_repeated_start();
		i2c_read_from_address(0x29); //Address the RGB Color Sensor Again
		unsigned char UpperBlue = i2c_read_data(0);  // read with NO_ACK
		i2c_stop();
		
		unsigned int blue = UpperBlue*256 + LowerBlue;	
		if(blue < 65535)
		{
			PORTD = 0b11111011; //turn LED 2 on
			unsigned int check_blue = blue;
		}
		
		wait(1000);
		PORTD = PORTD & 0b11110111; //checker led on
		wait(1000);
		PORTD = PORTD | 0b00001000; //checker led off
		wait(1000);
		
		wait(2000);
		if(check_red < 65535 && check_green < 65535)
		{
			PORTD = 0b11111110; //turn LED 0 on
		}
		wait(2000);
	}
	
}

// Note that for coding efficiencies, some of the following functions can be combined, for example to write to or read from an secondary using a single function call.  
// This would eliminate the need for some of the while(test TWINT bit) loops that occur back-to-back when subsequent functions are called.

void i2c_init(void) { // initialize i2c
	TWSR = 0b00000000; // prescaler is set to 1
	TWBR = 72; // Put 72 into TWBR to define SCL frequency as 100kHz for 16MHz oscillator
	TWCR = 0b00000100; // TWEN = 1 (enable TWI)
}

void i2c_start(void) { // send start command
	//while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for idle condition -- TWINT bit must be high to proceed -- not needed if single main is used
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  //TWCR | 0b10100100;       // initiate START condition -- write 1 to TWINT to clear it and initiate action, set TWSTA bit, set TWEN bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != START) // error checking -- need to predefine START = 0x08 and ERROR() function.
		// ERROR();
}

void i2c_stop(void) { // send stop command
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO); //TWCR = TWCR | 0b10010100;       // initiate STOP condition -- write 1 to TWINT to clear it and initiate action, set TWSTO bit and set TWEN bit
}

void i2c_repeated_start(void) {
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);  //TWCR | 0b10100100;       // initiate START condition -- write 1 to TWINT to clear it and initiate action, set TWSTA bit, set TWEN bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000));   // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != START) // error checking -- need to predefine START = 0x10 and ERROR() function.
	// ERROR();
}

void i2c_write_to_address( unsigned char address) { //Write an address byte to the I2C2 bus in form of SLA_W (address to write to)
	unsigned char SLA_W = address<<1; // create SLA_W byte by shifting address and leaving R/W bit clear
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = SLA_W;       // Load TWDR with address plus R/W bit
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MT_SLA_ACK) // error checking -- need to predefine MT_SLA_ACK and ERROR() function depending on possible outcomes: 0x18, 0x20, or 0x38.
		//ERROR();	
}

void i2c_read_from_address(unsigned char address) { //Write an address byte to the I2C bus in form of SLA_R (address to read from)
	unsigned char SLA_R = address<<1 | 1; // create SLA_R byte by shifting address and setting R/W bit
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = SLA_R;       // Load TWDR with address plus R/W bit
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MR_SLA_ACK) // error checking -- need to predefine MR_SLA_ACK and ERROR() function depending on possible outcomes: 0x38, 0x40, or 0x48.
		//ERROR();
}

void i2c_write_data(unsigned char data) { //Write data byte to the I2C2 bus
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	TWDR = data;       // Load TWDR with data to be sent
	TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Write -- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MT_DATA_ACK) // error checking -- need to predefine MT_DATA_ACK and ERROR() function depending on possible outcomes: 0x28 or 0x30.
		//ERROR();
}

unsigned char i2c_read_data(unsigned char ACK) { //Read a byte of data from a secondary on the I2C2 bus
	unsigned char data;
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for idle condition -- TWINT bit must be high to proceed
	if (ACK) // check for whether ACK or NO_ACK should be sent upon receipt of byte from secondary
		TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN); //TWCR = TWCR | 0b11000100;       // initiate Read with ACK -- write 1 to TWINT to clear it and initiate action, and set TWEA and TWEN
	else
		TWCR = (1<<TWINT) | (1<<TWEN); //TWCR = TWCR | 0b10000100;       // initiate Read with NO_ACK-- write 1 to TWINT to clear it and initiate action, and set TWEN
	while (!(TWCR & (1<<TWINT))); //while (!(TWCR & 0b10000000)) ;  // wait for action to finish (poll TWINT bit)
	// if ((TWSR & 0xF8) != MR_SLA_ACK) // error checking -- need to predefine MR_SLA_ACK and ERROR() function depending on possible outcomes: 0x50 or 0x58.
	//ERROR();
	//If multiple bytes are to be read, this function can be repeated with proper ACK or NO_ACK until done.
	data = TWDR;  // read the received data from secondary
	return(data);
}

void wait(volatile int multiple)
{
	while(multiple > 0)
	{
		TCCR0A = 0x00; //clear WMG00 and WGM01 registers to ensure timer/counter in normal mode
		TCNT0 = 0; //preload value testing on count = 250
		TCCR0B = 0b00000011; //Start TIMER0, normal mode, crystal clock, prescaler = 64
		while(TCNT0 < 0xFA); //exits loop when count = 250 (requires preload of 0 to make count = 250)
		TCCR0B = 0x00; //Stop TIMER0
		multiple--;
	}
}
	
