/*
  PIR Motion Sensor and Stepper Motor Control with State Machine
  States: BASE (90°), LEFT (0°), RIGHT (180°)
  Transitions based on PIR motion detection with checks and delays.
*/

#include <Stepper.h>
#include <SoftwareSerial.h>
#include <DFPlayerMini.h>

// Define the pins
int pirPinL = 2;    // Input pin for the PIR sensor
int pirPinR = 3;    // Input pin for the PIR sensor
int val1;              // Variable to read the sensor value
int val2;
Stepper myStepper(200, 8, 9, 10, 11);  // Stepper motor with 200 steps per revolution
int currentAngle = 90;  // Track current angle

// DFPlayer Mini
SoftwareSerial mySerial(4, 5);  // RX, TX
DFPlayerMini myDFPlayer;

// State machine enum
enum State { BASE, LEFT, RIGHT };
State currentState = BASE;

// Counters for 3-check requirement
int checkCounterL = 0;
int checkCounterR = 0;

// Timers for delays
unsigned long lastCheckTime = 0;
unsigned long baseDelayStart = 0;
const unsigned long delayTime = 1000;  // 1 second debounce delay
const unsigned long baseDelay = 5000;  // 5 seconds delay after returning to BASE

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  myStepper.setSpeed(60);  // Set stepper speed to 60 RPM
  // Move to initial position (90°)
  int steps = round(90.0 * 200.0 / 360.0);
  myStepper.step(steps);

  // Initialize DFPlayer Mini
  mySerial.begin(9600);
  if (myDFPlayer.begin(mySerial)) {
    Serial.println("DFPlayer Mini online.");
  } else {
    Serial.println("DFPlayer Mini not found.");
  }
  myDFPlayer.volume(30);  // Set volume (0-30)
}

void loop() {
  unsigned long currentTime = millis();

  // Update stepper position based on state
  int targetAngle;
  switch (currentState) {
    case BASE:
      targetAngle = 90;
      break;
    case LEFT:
      targetAngle = 0;
      break;
    case RIGHT:
      targetAngle = 180;
      break;
  }
  int steps = round((targetAngle - currentAngle) * 200.0 / 360.0);
  if (steps != 0) {
    myStepper.step(steps);
    currentAngle = targetAngle;
  }

  // State machine logic
  switch (currentState) {
    case BASE:
      // Apply delay after returning to BASE
      if (baseDelayStart > 0 && currentTime - baseDelayStart < baseDelay) {
        // Still in delay, do nothing
        Serial.println("In BASE delay...");
        return;
      }
      baseDelayStart = 0;  // Reset delay

      // Check PIRs periodically
      if (currentTime - lastCheckTime >= delayTime) {
        lastCheckTime = currentTime;
        val1 = digitalRead(pirPinL);
        val2 = digitalRead(pirPinR);

        if (val1 == HIGH && val2 == LOW) {
          checkCounterL++;
          checkCounterR = 0;
          Serial.println("Motion detected on Left side! Counter L: " + String(checkCounterL));
          myDFPlayer.play(1);  // Play sound for left motion
          if (checkCounterL >= 3) {
            currentState = LEFT;
            checkCounterL = 0;
            Serial.println("Transition to LEFT state.");
          }
        } else if (val1 == LOW && val2 == HIGH) {
          checkCounterR++;
          checkCounterL = 0;
          Serial.println("Motion detected on Right side! Counter R: " + String(checkCounterR));
          myDFPlayer.play(1);  // Play sound for right motion
          if (checkCounterR >= 3) {
            currentState = RIGHT;
            checkCounterR = 0;
            Serial.println("Transition to RIGHT state.");
          }
        } else {
          // No motion or both
          checkCounterL = 0;
          checkCounterR = 0;
          Serial.println("No motion or both detected. Counters reset.");
        }
      }
      break;

    case LEFT:
      val1 = digitalRead(pirPinL);
      val2 = digitalRead(pirPinR);
      if (val1 == HIGH || (val1 == HIGH && val2 == HIGH)) {
        // Stay in LEFT
        Serial.println("Staying in LEFT: Motion on Left or both.");
      } else if (val2 == HIGH) {
        // Opposite detected, go to BASE
        currentState = BASE;
        baseDelayStart = currentTime;
        Serial.println("Transition to BASE from LEFT due to Right motion.");
      } else {
        // No motion, stay
        Serial.println("No motion in LEFT state.");
      }
      break;

    case RIGHT:
      val1 = digitalRead(pirPinL);
      val2 = digitalRead(pirPinR);
      if (val2 == HIGH || (val1 == HIGH && val2 == HIGH)) {
        // Stay in RIGHT
        Serial.println("Staying in RIGHT: Motion on Right or both.");
      } else if (val1 == HIGH) {
        // Opposite detected, go to BASE
        currentState = BASE;
        baseDelayStart = currentTime;
        Serial.println("Transition to BASE from RIGHT due to Left motion.");
      } else {
        // No motion, stay
        Serial.println("No motion in RIGHT state.");
      }
      break;
  }
}
