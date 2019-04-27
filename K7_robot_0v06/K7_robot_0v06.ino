//  KARL 7 ROBOT

#include  "pins.h"
#include  "prototypes.h"
#include "pcf8574_esp.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "Adafruit_PCD8544.h"

PCF857x expanderTwo(0x20, &Wire);
PCF857x expanderOne(0x21, &Wire);
Adafruit_PCD8544 display = Adafruit_PCD8544(14, 13, 12, -1); // (SCLK) // (DIN) // (D/C) // (RST) on IO EXPANDER

extern volatile long CommandedPositionLeft;
extern volatile long CommandedPositionRight;

volatile bool startupSound = false;
volatile bool loopSound = false;

volatile bool PCFInterruptFlagTwo = false;
void ICACHE_RAM_ATTR PCFInterruptTwo() {
  PCFInterruptFlagTwo = true;
}
volatile bool PCFInterruptFlagOne = false;
void ICACHE_RAM_ATTR PCFInterruptOne() {
  PCFInterruptFlagOne = true;
}

void checkMotorFault (void ) {

  if (PCFInterruptFlagOne == true) {
    PCFInterruptFlagOne = false;
    bool fault = expanderOne.read(7);
    if (!fault)  Serial.println("mot fault");
  }

}

void setup() {
  Serial.begin(115200);
  Serial.println("K7 ROBOT STARTING");

  Wire.begin(21, 22);
  Wire.setClock(100000L);
  
  expanderTwo.begin();
  pinMode(32, INPUT_PULLUP);
  expanderTwo.resetInterruptPin();
  attachInterrupt(digitalPinToInterrupt(32), PCFInterruptTwo, FALLING);

  expanderOne.begin();
  pinMode(35, INPUT_PULLUP);
  expanderOne.resetInterruptPin();
  attachInterrupt(digitalPinToInterrupt(35), PCFInterruptOne, FALLING);

  display.begin();
  display.setContrast(60);
  display.clearDisplayRAM();
  setupExpression();
  encoderEnable ();
 // setupMotor();
  setupSound ();

  
/*    goSpin ();
    delay (5000);
    goStop();
    delay(500);
    goForward();
    delay(1000);
    goStop();

    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Hello!");
    display.display();

  */

  backLight (true);
  for (int i = 200; i < 1000; i += 100) {
    led (i % 3, true);

    if (startupSound == true)
      tone (i, 50);
      
    led (i % 3, false);
  }
  backLight (false);


}


uint8_t ledCounter = 1;
void loop() {

 
  handleInt();
  checkMotorFault ();

  //CommandedPositionRight++;
  //CommandedPositionLeft++;
 
  led (ledCounter, true);

  if (loopSound == true)
    tone (ledCounter*500, 50);
    
  delay (500);
  led (ledCounter, false);
  
  ledCounter ++;
  if (ledCounter > 3) ledCounter = 1;
  
  eyeblink();
  delay(5000);
  
 /*   if (PCFInterruptFlagOne) {

      display.println("Got an interrupt: ");
      expanderTwo.write(7, expanderOne.read(3)); // 3 is button right
      expanderTwo.write(6, expanderOne.read(2)); // 2 is button left
      PCFInterruptFlagOne = false;
    }

    if (PCFInterruptFlagTwo) {
      display.println("Got an interrupt: ");
      if (expanderTwo.read(3) == HIGH) display.println("Pin 3 is HIGH!");
      else Serial.println("Pin 3 is LOW!");
      // DO NOTE: When you write LOW to a pin on a PCF8574 it becomes an OUTPUT.
      // It wouldn't generate an interrupt if you were to connect a button to it that pulls it HIGH when you press the button.
      // Any pin you wish to use as input must be written HIGH and be pulled LOW to generate an interrupt.
      //  pcf8574.write(7, pcf8574.read(3));
      PCFInterruptFlagTwo = false;
    }
    Serial.println("Blink.");
    // if(digitalRead(2)==HIGH) digitalWrite(2, LOW);
    // else digitalWrite(2, HIGH);

    //expanderTwo.write(7, HIGH); // red
    //expanderTwo.write(6, HIGH); // green
    // expanderTwo.write(5, HIGH); // blue
    expanderTwo.write(4, HIGH); // lcd back
    delay(1000);
    //expanderTwo.write(7, LOW); // red
    // expanderTwo.write(6, LOW); // green
    // expanderTwo.write(5, LOW); // blue
    expanderTwo.write(4, LOW); // lcd back
    delay(1000);
  */
}
/*Default expression: two filled circle eyes and a smiling mouth*/
void setupExpression(void){
  display.clearDisplay();
  display.setRotation(0);
  delay (100);
  display.fillCircle(24, 30, 8, BLACK);
  display.fillCircle(60, 30, 8, BLACK);
  
  display.setCursor(30,32);
  display.setTextSize(2);
  display.setRotation(1);
  display.print(")");
  display.display();
}

/*Eye blinks for 500 ms and then back to default expression*/
void eyeblink(void){
  display.setRotation(0);
  display.clearDisplay();

  display.drawLine(18,31,30,30,BLACK);
  display.drawLine(54,30,64,31,BLACK);

  display.setCursor(30,32);
  display.setTextSize(2);
  display.setRotation(1);
  display.print(")");
  display.display();

  delay(500);

  setupExpression();
}
