// BUMPY 2.1 FIRMWARE
// Version: 0.1
// Autor:   PS
//
// Firmware für die weiterentwickelte Steuerelektronik
// für den von René Jung entwickelten Bumpy Roboter

#define F_CPU 16000000UL  

#include <avr/io.h>
#include <util/delay.h>

// delay Funktion
void delay(uint16_t ms)
{
    for(; ms>0; ms--) _delay_ms(1);
}

uint8_t PINARR[] = {PD2,PD3,PD5,PD6,PD7,PB0,PB1,PB2,PB3,PB4};
#define OUTPUT 1
#define INPUT_PULLUP 2
void pinMode(uint8_t pin, uint8_t mode)
{
	if(pin > 9)
	{
		if(mode == OUTPUT)
			DDRB |= (1 << PINARR[pin-5]);
		else
			DDRB &= ~(1 << PINARR[pin-5]); 
	}
	else
	{
		if(mode == OUTPUT)
			DDRD |= (1 << PINARR[pin-5]);
		else
			DDRD &= ~(1 << PINARR[pin-5]); 	
	}
}

#define HIGH 0x1
#define LOW 0x0
void digitalWrite(uint8_t pin, uint8_t val)
{
	if(pin > 9)
	{
		if(val == HIGH)
			PORTB  |= (1 << PINARR[pin-5]);
		else
			PORTB  &= ~(1 << PINARR[pin-5]); 
	}
	else
	{
		if(val == HIGH)
			PORTD  |= (1 << PINARR[pin-5]);
		else
			PORTD  &= ~(1 << PINARR[pin-5]); 	
	}
}

int digitalRead(uint8_t pin)
{
	if(pin > 9)
	{
		if(PINB & (1 << PINARR[pin-5]))
			return HIGH;
		else
			return LOW;
	}
	else
	{
		if(PIND & (1 << PINARR[pin-5]))
			return HIGH;
		else
			return LOW;	
	}	
}

// Pinzuweisung
#define MOTOR1_PIN_A 8
#define MOTOR1_PIN_B 7
#define MOTOR2_PIN_A 9
#define MOTOR2_PIN_B 10
#define SWITCH1_PIN  11
#define SWITCH2_PIN 12
#define LED1_PIN 6
#define LED2_PIN 5



// Zähler für die Schalter 1 und 2
// (wird nur im IDLE Modus verwendet)
int iSwitchCounter1;
int iSwitchCounter2;
#define SWITCH_COUNT 60

int iBackCounter;
#define BACK_COUNT_1 60
#define BACK_COUNT_2 60

// Zustandsvariable
int iMode;
int iDirection;
#define MODE_IDLE 0
#define MODE_RUNNING_FORWARD 1
#define MODE_RUNNING_BACKWARD_1 2
#define MODE_RUNNING_BACKWARD_2 3
#define DIRECTION_LEFT 0
#define DIRECTION_RIGHT 1

int main(void) {
	
	int i;
	
  ////////////////////////////////////////////////////////////////
  // SETUP
  ////////////////////////////////////////////////////////////////
  pinMode(MOTOR1_PIN_A, OUTPUT);
  pinMode(MOTOR1_PIN_B, OUTPUT);
  pinMode(MOTOR2_PIN_A, OUTPUT);
  pinMode(MOTOR2_PIN_B, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  
  iSwitchCounter1 = 0;
  iSwitchCounter2 = 0;
  iBackCounter = 0;

  //Serial.begin(9600);

  // Bootvorgang beendet
  for(i = 0; i < 5; i++) {
    digitalWrite(LED1_PIN, HIGH);
    digitalWrite(LED2_PIN, HIGH);
    delay(100);
    digitalWrite(LED1_PIN, LOW);
    digitalWrite(LED2_PIN, LOW);
    delay(100);
  }
  
  iMode = 0;
  iDirection = 0;

  while(1)
  {
  // Hauptroutine

  //Serial.write(iSwitchCounter1);

  switch(iMode) {
    
    // Zustand: IDLE
    // Beide Motoren sind augeschaltet
    // Aktivierung durch Drücken und gedrückthalten
    // einer der beiden Schalter
    case MODE_IDLE:
      
      digitalWrite(MOTOR1_PIN_A, LOW);
      digitalWrite(MOTOR1_PIN_B, LOW);
      digitalWrite(MOTOR2_PIN_A, LOW);
      digitalWrite(MOTOR2_PIN_B, LOW);

      if(digitalRead(SWITCH1_PIN) == LOW) {
        iSwitchCounter1++;
      }
      else {
        iSwitchCounter1 = 0;
      }

      if(digitalRead(SWITCH2_PIN) == LOW) {
        iSwitchCounter2++;
      }
      else {
        iSwitchCounter2 = 0;
      }      

      if((iSwitchCounter1 > SWITCH_COUNT) || (iSwitchCounter2 > SWITCH_COUNT)) {
        iMode = MODE_RUNNING_FORWARD;
        iSwitchCounter1 = 0;
        iSwitchCounter2 = 0;
      }
      
    break;
    // ---Ende des IDLE Zustandes---//

    // Zustand: RUNNING_FORWARD
    // Beide Motoren laufen mit 100% Leistung vorwärts
    case MODE_RUNNING_FORWARD:
      
      digitalWrite(MOTOR1_PIN_A, HIGH);
      digitalWrite(MOTOR1_PIN_B, LOW);
      digitalWrite(MOTOR2_PIN_A, HIGH);
      digitalWrite(MOTOR2_PIN_B, LOW);

      if(iSwitchCounter1 > SWITCH_COUNT) {
        if(digitalRead(SWITCH1_PIN) == LOW) {
          iMode = MODE_RUNNING_BACKWARD_1;
          iDirection = DIRECTION_LEFT;
          digitalWrite(LED1_PIN, HIGH);
          iBackCounter = 0;
        }
  
        if(digitalRead(SWITCH2_PIN) == LOW) {
          iMode = MODE_RUNNING_BACKWARD_1;
          iDirection = DIRECTION_RIGHT;
          digitalWrite(LED2_PIN, HIGH);
          iBackCounter = 0;
        }
      }
      else {
        iSwitchCounter1++;      
      }
      
    break;

    // ---Ende des RUNNING FORWARD Zustandes---//

    // Zustand: RUNNING_BACKWARD_1
    // Beide Motoren laufen mit 100% Leistung rückwärts
    case MODE_RUNNING_BACKWARD_1:

      digitalWrite(MOTOR1_PIN_A, LOW);
      digitalWrite(MOTOR1_PIN_B, HIGH);
      digitalWrite(MOTOR2_PIN_A, LOW);
      digitalWrite(MOTOR2_PIN_B, HIGH);

      iBackCounter++;

      if(iBackCounter > BACK_COUNT_1) {
        iMode = MODE_RUNNING_BACKWARD_2;
        iBackCounter = 0;
      }
          
    break;

    // ---Ende des RUNNING BACKWARD 1 Zustandes---//

    // Zustand: RUNNING_BACKWARD_2
    // In die gewünschte Richtung drehen
    case MODE_RUNNING_BACKWARD_2:

    digitalWrite(MOTOR1_PIN_A, LOW);
    digitalWrite(MOTOR2_PIN_A, LOW);

      if(iDirection == DIRECTION_LEFT) {
        digitalWrite(MOTOR1_PIN_B, HIGH);
        digitalWrite(MOTOR2_PIN_B, LOW);
      }
      else {
        digitalWrite(MOTOR1_PIN_B, LOW);
        digitalWrite(MOTOR2_PIN_B, HIGH);
      }

      iBackCounter++;

      if(iBackCounter > BACK_COUNT_2) {
        iMode = MODE_RUNNING_FORWARD;
        digitalWrite(LED1_PIN, LOW);
        digitalWrite(LED2_PIN, LOW);
      }
          
    break;
    
    //case MODE_RUNNING_BACKWARD_RIGHT:
    //break;
    /*case MODE_TURING_LEFT:
    break;
    case MODE_TURING_RIGHT:
    break;    
    */
  }

  delay(50);
  }
  
  return 0;
  
}
