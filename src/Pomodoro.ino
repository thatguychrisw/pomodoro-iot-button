#include "InternetButton/InternetButton.h"
#include <math.h>

InternetButton b = InternetButton();

// States
bool stSessionStarted = false;
bool stSessionEnded = false;

// Status
int currentLedNum = 1;
float currentLedPercentage = 0;

float sessionPctCompleted = 0;
int sessionPercent = 0;

// Time
float sessionStartTime = 0;
float sessionEndTime = 0;
float now = 0;
float sessionLength = (1 * 60 * 1000); // minutes - to - seconds

STARTUP(pinMode(D3, INPUT_PULLDOWN));

void setup() {
    b.begin();

    b.rainbow(4);
    b.allLedsOff();

    Serial.begin(9600);
}

void loop() {
    // So the value is the same for the duration of the loop
    now = millis();

    // App
    handleButtonPresses();
    processPomo();
}

void handleButtonPresses() {
    if (b.buttonOn(2) && sessionEndTime == 0) {
      stSessionStarted = true;
      sessionStartTime = now;
      sessionEndTime = now + sessionLength;
    }
}

void processPomo() {

  if (stSessionStarted == true) {
    if (currentLedNum == 11) {
      stSessionEnded = true;
      stSessionStarted = false;

      Serial.println("Session ended");
    } else {
      currentLedNum = 1;

      sessionPctCompleted = ((now - sessionStartTime) / (sessionEndTime - sessionStartTime));
      sessionPercent = sessionPctCompleted * 100;

      Serial.println("Current progress @" + String(now) + " || " + String(sessionEndTime));
      Serial.println("Current progress @" + String(sessionPercent) + "%");

      if ((sessionPctCompleted * 10) >= 1) {
        currentLedNum = floor(fmod(sessionPctCompleted, 10) * 10) + 1;
      }

      b.ledOn(currentLedNum, 0, 255, 0);
    }
  } else if (stSessionEnded == true) {
      stSessionEnded = false;
      stSessionStarted = false;
      sessionEndTime = 0;
      sessionStartTime = 0;
      currentLedNum = 1;

      b.rainbow(5);
      delay(1000);
      b.allLedsOff();
  }
}
