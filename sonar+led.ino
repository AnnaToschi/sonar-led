//Timer 1 Input capture: it is used to measure the time between the raising edge of the input and the falling edge of the input, which is the echoPin. In this way we are able to measure the distances.
//A LED turns on if the distance is smaller than 10 cm. 

//Libraries
#include <Arduino.h>
//#include <LiquidCrystal.h>


//Variables
volatile uint16_t Capt1, Capt2;//VARIABLES TO HOLD TIMESTAMPS
volatile uint8_t flag = 0; //CAPTURE FLAG
const int speed_sound = 34300; //Speed of sound in cm/s
long deltatime;
int dist;
int j = 0;

//define the pin numbers
const int trigPin = 9;
const int echoPin = 10;
const int led = 2;


//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/*--------------------------------------------------------------------------------------------------
MAIN FUNCTION
---------------------------------------------------------------------------------------------------*/
void setup(){
  
  //Lcd configuration
  //lcd.begin(16, 2); 
  
 //Set echopin as input and enable the pull-up
  DDRB &= ~(1 << echoPin); //Input
  PORTB |= (1 << echoPin); //Enable pull-up resistors

  //Set trigpin as output
  DDRB |= (1 << trigPin); //Output
  PORTB &= ~(1 << trigPin);  //Set the output to zero

  //Set led as output
  DDRD |= (1 << led);
  PORTD &= ~(1 << led);

  
  //Interrupts settings
  TCCR1A = 0; //Disable compare interrupts
  TCCR1B = 0; 
  TCNT1=0; //SETTING INTIAL TIMER VALUE
  
  TCCR1B|=(1<<ICES1); //SETTING FIRST CAPTURE ON RISING EDGE 
  TIMSK1|=(1<<ICIE1); //ENABLING INPUT CAPTURE
  TIFR1 &= ~(1 << ICF1); //Clear the flag 
  TCCR1B|=(1<<CS10);
  TCCR1B &= ~(1<<CS12);
  TCCR1B &= ~(1<<CS11);//STARTING TIMER WITH NO PRESCALER 
  
  sei(); //ENABLING GLOBAL INTERRUPTS
  
  }
  
void loop(){
  
  // I send a signal through the trigger of the sonar.
    PORTB |= (1 << trigPin);
    for (j = 0; j < 1000 ; j++){} //delay
    PORTB &= ~(1 << trigPin);
    
    
    //lcd.print("Distance [cm] = ");  
    
    //calculate the distance if all timestamps are captured
    
     if (flag == 2)
    {
      deltatime = (Capt2 - Capt1)/16000000; // NO Prescaler
      dist = deltatime*speed_sound/2;
      if (dist < 10){
        PORTD |= (1 << led);
        }
    
      //lcd.setCursor(0, 1);
      //lcd.print(dist);
      flag = 0; //CLEARING FLAGS
      PORTD &= ~(1 << led);
      TIMSK1|=(1<<ICIE1); //ENABLING INPUT CAPTURE INTERRUPTS
      TCNT1 = 0;
    }
  
  }

/*--------------------------------------------------------------------------------------------------
CAPTURE ISR
---------------------------------------------------------------------------------------------------*/
ISR(TIMER1_CAPT_vect)
{
if (flag == 0)
{
  Capt1 = ICR1; //SAVING CAPTURED TIMESTAMP
  TCCR1B &=~(1<<ICES1); //CHANGE CAPTURE ON FALLING EDGE
  

}
if (flag == 1)
{
  Capt2 = ICR1; //SAVING CAPTURED TIMESTAMP
  TCCR1B|=(1<<ICES1); //CHANGING CAPTURE ON RISING EDGE
  
  TIFR1 &= ~(1 << ICF1); //Clear the flag
}
 flag = flag + 1;
}
