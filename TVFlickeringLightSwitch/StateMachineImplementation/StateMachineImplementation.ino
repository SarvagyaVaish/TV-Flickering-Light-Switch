/*
 * IRremote Library 
 * An IR LED must be connected to Arduino PWM pin 3.
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 *
 * Detecting flickering lights to switch on/off TV
 * -----------------------------------------------
 * Copyright 2013 Sarvagya Vaish
 * You have the right to use and modify, 
 * but must buy me a beer if you make any money off of it :)
 * Email: sarvagyavaish@gatech.edu
 */

#include <IRremote.h>
IRsend irsend;

// Adjustable parameters
int flickerWindow = 500;     // Milli-second window to detect consequent flickers.
int lightThreshold = 400;    // Threshold for differentiating between Light and Dark environments.

// Parameters not to be messed with
int sm_state = 0;            // State Machine State. Starts in in State 0. 
int sensorValue;
int timer;
int elapsedTime;


void setup()
{
  Serial.begin(9600);
  Serial.println("Starting up...");
  delay(500);
}


// State Machine Diagram
//      .->     if dark     ->     (1)     ->     if light     ->     (2)
//     ^                            |                                  |  \
//    /                             v                                  v    v
// (0)  <-----     (if elpased_time > flickerWindow)    <------------------- (5)
//    \                             ^                                  ^    ^
//     v                            |                                  |  /
//      .->     if light    ->     (3)     ->     if dark      ->     (4)
// 
// State (5) sends the power signal.

void loop() {
  
  delay(1);
  sensorValue = analogRead(A0);
  //Serial.println(sensorValue);
  elapsedTime = millis() - timer;
  
  switch (sm_state) {
    case 0:
      if (!isLight()) {
        changeToState(1);      // Goto state 1 if dark
      }
      else if (isLight()) {
        changeToState(3);      // Goto state 3 if light
      } 
    break;
    
    
    case 1:                    // Dark
      if (elapsedTime > 500)
        changeToState(0);
      else if (isLight())      // Goto state 2 if light
        changeToState(2);
    break;
    
    
    case 2:                    // Light
      if (elapsedTime > 500)
        changeToState(0);
      else if (!isLight())     // Goto state 5 if dark
        changeToState(5);
    break;
    
    
    case 3:                    // Light
      if (elapsedTime > 500)
        changeToState(0);
      else if (!isLight())    // Goto state 4 if dark
        changeToState(4);
    break;
    
    
    case 4:                    // Dark
      if (elapsedTime > 500)
        changeToState(0);
      else if (isLight())      // Goto state 5 if light
        changeToState(5);
    break;
    
    
    case 5:                    // Flicker Detected
      hitPowerButton();
      changeToState(0);        // Goto state 0 and restart
      delay(1000);
    break;
  }
  
}

// Helper Methods 

void hitPowerButton(){
    for (int i = 0; i < 3; i++) {
      Serial.println("Sending");
      irsend.sendSony(0xA90, 12); // Sony TV power code
      delay(40);
    }
}

void changeToState(int newState) {
  Serial.println(newState);
  sm_state = newState;
  resetTimer();
}

void resetTimer() {
  elapsedTime = 0;
  timer = millis();
}

int isLight() {
  if (analogRead(A0) > lightThreshold) return 1;
  return 0;
}
