// Jack Berkowitz
// Color Sensor, Initial Operation without Logic
// 4/17/24


#include <avr/io.h>

//Initializing Functions
void wait(volatile int);

int main(void)
{
      DDRC = 0b00000000; //data register, pins input from arduino
      DDRB = 0b00000111; //data register, pins PB1 set as output for LEDs
      //PORTC = 0b00111111; //setting output of PORTC high to turn off LEDs (assuming wired as active low)
      PORTB = 0b00000111; //setting output of PORTB high to turn off LED (assuming wired as active low)
      
      while(1)
      {
            
                  if(PINC & 0b00000100)
                  {
                        PORTB = PORTB & 0b11111110; //Turn on LED 0, PB0
                  }
                  if(PINC & 0b00001000)
                  {
                        PORTB = PORTB & 0b11111101; //Turn on LED 1, PB1
                  }
                  
                  if(PINC & 0b00010000)
                  {
                        PORTB = PORTB & 0b11111011; //Turn on LED 1, PB2
                  }
            
            
      }
}

void wait(volatile int multiple)
{
      while(multiple > 0)
      {
            TCCR0A = 0x00;
            TCNT0 = 0;
            TCCR0B = 0b00000011;
            while(TCNT0 < 0xFA);
            TCCR0B = 0x00;
            multiple--;
      }
}
