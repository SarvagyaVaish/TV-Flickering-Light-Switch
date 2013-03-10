/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

IRsend irsend;

void setup()
{
  Serial.begin(9600);
}

int time=500;
int i=0;
char commandbuffer[] = "A90";
int state = 1; // 0 -> DARK
int sensorValue = analogRead(A0);

void loop() {

  int x;
  int mySensVals[29];
  int DiffVec[28];
  int stateVec[28];
  int triggerA = 0;
  int triggerB = 0;

  for (x = 0; x < 29; x = x + 1) {
    mySensVals[x] = analogRead(A0);
    Serial.println(mySensVals[x]);
    delay(100);
  }

  for (x = 1; x < 29; x = x + 1) {
    DiffVec[x-1] = abs(mySensVals[x] - mySensVals[x-1]);
    Serial.println(DiffVec[x-1]); 
  }

  Serial.println("STATEVEC IS COMING");

  for (x = 0; x < 28; x = x + 1) {
    stateVec[x] = DiffVec[x] > 150;
    Serial.println(stateVec[x]);

    if (stateVec[x] == 1 && triggerA==1 && triggerB==1) { 
      Serial.println("PowerON"); 
      sendMessage(getHexVal(commandbuffer));
      break;

    }
    if (stateVec[x] == 1 && triggerA==0) { 
      triggerA = 1;
      Serial.println("triggerA");
    }
    if (stateVec[x] == 1 && triggerA==1) { 
      triggerB = 1;
      Serial.println("triggerB");
    }

  }

  delay(50);


}

int getHexVal(char *s) 
{
  int x = 0;
  for(;;) {
    char c = *s;
    if (c >= '0' && c <= '9') {
      x *= 16;
      x += c - '0'; 
    }
    else if (c >= 'A' && c <= 'F') {
      x *= 16;
      x += (c - 'A') + 10; 
    }
    else break;
    s++;
  }
  return x;
}

void sendMessage(unsigned int message){
  for (int i = 0; i < 3; i++) {
    irsend.sendSony(message, 12); // Sony TV power code
    delay(40);
  }
}
