#include "InternetButton/InternetButton.h"
#include <math.h>

SYSTEM_MODE(AUTOMATIC);

InternetButton b = InternetButton();

// States
bool stSessionStarted = false;
bool stSessionEnded = false;
bool stPeriodChanged = false;

// Status
int projectedLedNum = 1;
int priorLedNum = 1;
float currentLedPercentage = 0;

float sessionPctCompleted = 0;
float periodPctCompleted = 0;

// Time
float sessionStartTime = 0;
float sessionEndTime = 0;
float now = 0;
float sessionLength = (1 * 60 * 1000); // minutes - to - seconds
float periodLength = sessionLength / 10;
float periodStartTime = 0;
float periodEndTime = 0;


STARTUP(pinMode(D3, INPUT_PULLDOWN));

void setup() {
    b.begin();

    b.rainbow(4);
    b.allLedsOff();

    /*Serial.begin(9600);*/
}

void loop() {
    // So the value is the same for the duration of the loop
    now = millis();

    // Time tracking
    if (stSessionStarted == true) {
      sessionPctCompleted = ((now - sessionStartTime) / (sessionEndTime - sessionStartTime));
      periodPctCompleted = ((now - periodStartTime) / (periodEndTime - periodStartTime));
    }

    // App
    handleButtonPresses();
    processPomo();
}

void handleButtonPresses() {
    if (b.buttonOn(2) && sessionEndTime == 0) {
      stSessionStarted = true;
      sessionStartTime = now;
      sessionEndTime = now + sessionLength;
      periodStartTime = now;
      periodEndTime = now + periodLength;
    }
}

void processPomo() {

  if (stSessionStarted == true) {
    if (sessionPctCompleted >= 1) {
      stSessionEnded = true;
      stSessionStarted = false;

      /*Serial.println("Session ended");*/
    } else {

      // Project which led should be lit, based on progress
      projectedLedNum = floor(fmod(sessionPctCompleted, 10) * 10);
      if (projectedLedNum < 1) projectedLedNum = 1; else projectedLedNum++;

      // The led has changed since the last tick
      stPeriodChanged = (projectedLedNum != priorLedNum);
      if (stPeriodChanged == true) {
        // Reset
        periodEndTime = now + periodLength;
        periodStartTime = now;

        currentLedPercentage = 0;
      } else {
        currentLedPercentage = periodPctCompleted * 255;
      }

      /*Serial.println("Current progress @" + String(now) + " || " + String(sessionEndTime));
      Serial.println("Current progress @" + String(sessionPctCompleted) + "%");
      Serial.println("Period progress @" + String(periodPctCompleted));
      Serial.println("Projected LED@" + String(projectedLedNum));*/

      if (projectedLedNum >= 7) {
        b.ledOn(projectedLedNum, currentLedPercentage, 0, 0);
      } else {
        b.ledOn(projectedLedNum, 0, 0, currentLedPercentage);
      }

      // Track prior status
      priorLedNum = projectedLedNum;
    }
  } else if (stSessionEnded == true) {
      stSessionEnded = false;
      stSessionStarted = false;
      sessionEndTime = 0;
      sessionStartTime = 0;
      sessionPctCompleted = 0;
      periodPctCompleted = 0;
      periodEndTime = 0;
      periodStartTime = 0;
      projectedLedNum = 1;

      b.rainbow(5);
      delay(1000);
      b.allLedsOff();
  }
}
