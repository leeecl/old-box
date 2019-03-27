/* 
 * receive two modes: pinch button  --> red light
 * receive touch sensor 3 seconds  --> green light 
 * output: LCD display change word
 * output: Buzzer in three modes of frequencies, mimic heartbeat sound
 */

#include <Wire.h>
#include "rgb_lcd.h"

// input PIN
int pinTouch = 5;   // touch; for 5 seconds
const int pinLight = A3;  // light sensor

// output PIN
int pinRED = 11; // select the pin for the red LED
int pinBLUE =10; // select the pin for the  blue LED
int pinGREEN =9; // select the pin for the green LED
int LEDval;

// LCD display
rgb_lcd lcd;
int green[] = {150,247,224};
int red[] = {255,0,208}; 

// touch
int touchStart = 0;
int touchCurrent = 0;
const unsigned long period = 2000;     // changed from 5000 to 3000 in case not working well.(
int touchCount = 0;

// light sensor
int lightThresholdVal = 130;   // everyday: 760; covered: 12; the new paper sometimes doesn't attach: 150; because that way everyday will be 100

// buzzer sound
int tempo = 300;
int thresholdValue = 650;    // needs refine better; 700 or 650
int soundCount = 0;

// timer for system sleep / wake up
int timer = millis();
int prevInteractTimer;
boolean activated = false;

// check check two actions
int care = 0;
int tear = 0;

void setup() {
    pinMode(pinTouch, INPUT);
    pinMode(pinRED, OUTPUT);
    pinMode(pinBLUE, OUTPUT);
    pinMode(pinGREEN, OUTPUT);

    Serial.begin(9600);

    // setup LCD
    lcd.begin(16, 2);
    lcd.setRGB(40,40,40);
}

void loop() {   
  int touchVal = digitalRead(pinTouch);
  int lightVal = analogRead(pinLight);

  // Serial.println("timer ");
  // Serial.print(prevInteractTimer);
  // Serial.println(" ");
  
  // Serial.println("timer current ");
  // Serial.print(millis());
  // Serial.println(" ");
  
  if (activated == false) {
    // default LED
      for (LEDval = 0; LEDval < 255; LEDval++) {
        analogWrite(pinRED, 255-LEDval);
        analogWrite(pinBLUE, LEDval);
        analogWrite(pinGREEN, 128-LEDval);
      }
      for (LEDval = 0; LEDval < 255; LEDval++) {
        analogWrite(pinRED, LEDval);
        analogWrite(pinBLUE, 255-LEDval);
        analogWrite(pinGREEN, 128-LEDval);
      }
    Serial.println("not activated");
    if ((touchVal == 1) && (touchCount == 0)) {
      lcd.setRGB(40,40,40);
      lcd.setCursor(0, 0);
      lcd.print("Yes?            ");
      lcd.setCursor(0, 1);
      lcd.print("               ");
      touchCount += 1;
      delay(1000);
    } else if ((touchVal == 1) && (touchCount == 1)) {
      lcd.setCursor(0, 0);
      lcd.print("z....z....z... ");
      lcd.setCursor(0, 1);
      lcd.print("               ");
      touchCount += 1;
      delay(1000);
    } else if ((touchVal == 1) && (touchCount == 2)) {
      lcd.setCursor(0, 0);
      lcd.print("Oh hello. I am ");
      lcd.setCursor(0, 1);
      lcd.print("an old box     ");
      delay(1000);
      touchCount = 0;  
      activated = true;
    }
  } else {
    // default LED
      for(LEDval = 0; LEDval < 255; LEDval++) {
        analogWrite(pinRED, 255-LEDval);
        analogWrite(pinBLUE, LEDval);
        analogWrite(pinGREEN, 128-LEDval);
      }
      for(LEDval = 0; LEDval < 255; LEDval++) {
        analogWrite(pinRED, LEDval);
        analogWrite(pinBLUE, 255-LEDval);
        analogWrite(pinGREEN, 128-LEDval);
      }
    // after care LED
    if ((care == 1)&&(tear == 0)) {
      for(LEDval = 0; LEDval < 255; LEDval++) {
        analogWrite(pinRED, 255 - LEDval);
        analogWrite(pinBLUE, 128 - LEDval);
        analogWrite(pinGREEN, LEDval);
      };
    };
    // after tear LED
    if ((care == 0)&&(tear == 1)) {
      for(LEDval = 0; LEDval < 255; LEDval++) {
        analogWrite(pinGREEN, 255 - LEDval);
        analogWrite(pinBLUE, 128 - LEDval);
        analogWrite(pinRED, LEDval);
      }
    };
    lcdPrintGuide();

    // check for time of non-interactions; memory of this experience wiped out.      
    if ((prevInteractTimer != 0) && (millis() - prevInteractTimer >=  300000)) {
        activated = false;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Who r you again?");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        delay(2000);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Who am I?       ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        delay(2000);
        prevInteractTimer = 0;
        care = 0;
        tear = 0;
    }

    // Touched -> green light (no green now)
    touchVal = digitalRead(pinTouch);
    touchStart = millis();
    while (touchVal == 1) {
        touchCurrent = millis();
        if ((touchCurrent - touchStart) >= period) {
            for(LEDval = 0; LEDval < 255; LEDval++) {
              analogWrite(pinRED, 255 - LEDval);
              analogWrite(pinBLUE, 128 - LEDval);
              analogWrite(pinGREEN, LEDval);
            }

            lcd.setRGB(green[0],green[1],green[2]); 
            lcd.setCursor(0, 0);
            lcd.print("Thank you       ");
            lcd.setCursor(0, 1);
            lcd.print("I feel better   ");
            delay(2000);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("I recall a good ");
            lcd.setCursor(0, 1);
            lcd.print("memory          ");
            delay(6000);

            care = 1;
        }
        touchVal = digitalRead(pinTouch);
        prevInteractTimer = millis();
    }

    // Torn -> red light
    lightVal = analogRead(pinLight);
    while (lightVal >= lightThresholdVal) {
      for(LEDval = 0; LEDval < 255; LEDval++) {
        analogWrite(pinGREEN, 255 - LEDval);
        analogWrite(pinBLUE, 128 - LEDval);
        analogWrite(pinRED, LEDval);
      }
      lcd.setRGB(red[0],red[1],red[2]);
      lcd.setCursor(0, 0);
      lcd.print("Ouch           ");
      lcd.setCursor(0, 1);
      lcd.print("               ");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("I've felt      ");
      lcd.setCursor(0, 1);
      lcd.print("hurt before....");
      delay(6000);
      lightVal = analogRead(pinLight);
      prevInteractTimer = millis();
      tear = 1;
    }

    if ((care == 1) && (tear == 1)) {
      for(LEDval = 0; LEDval < 255; LEDval++) {
        analogWrite(pinRED, 255-LEDval);
        analogWrite(pinBLUE, LEDval);
        analogWrite(pinGREEN, 128-LEDval);
      }
      for(LEDval = 0; LEDval < 255; LEDval++) {
        analogWrite(pinRED, 128-LEDval);
        analogWrite(pinBLUE, 255-LEDval);
        analogWrite(pinGREEN, LEDval);
      }
      
      lcd.clear();
      delay(6000);
      lcd.setRGB(green[0],green[1],green[2]);
      lcd.setCursor(0, 0);
      lcd.print("I remember now  ");
      lcd.setCursor(0, 1);
      lcd.print("once I ...  ");
      delay(6000);
      lcd.clear();
      delay(5000);
      lcd.setCursor(0, 0);
      lcd.print("Thank you   ");
      lcd.setCursor(0, 1);
      lcd.print("Now I know  ");
      delay(2000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("who I am    ");
      lcd.setCursor(0, 1);
      lcd.print("            ");
      delay(6000);
    }
  }
}

void lcdPrintGuide() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("I am an old box");
  lcd.setCursor(0, 1);
  lcd.print("               ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("U can do wutever");
  lcd.setCursor(0, 1);
  lcd.print("u want with me  ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("I am an old box");
  lcd.setCursor(0, 1);
  lcd.print("               ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("I'm fragile....");
  lcd.setCursor(0, 1);
  lcd.print("               ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Plz take care  ");
  lcd.setCursor(0, 1);
  lcd.print("of me          ");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Who am I??     ");
  lcd.setCursor(0, 1);
  lcd.print("               ");
  delay(2000);
}
