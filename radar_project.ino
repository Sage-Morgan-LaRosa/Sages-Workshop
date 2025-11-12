/*
  PIR Motion Sensor and Servo Control with State Machine
  States: BASE (90°), LEFT (0°), RIGHT (180°)
  Transitions based on PIR motion detection with checks and delays.
*/

#include <Servo.h>

// Define the pins
int pirPinL = 2;    // Input pin for the PIR sensor
int pirPinR = 3;    // Input pin for the PIR sensor
int val1;              // Variable to read the sensor value
int val2;
Servo myservo;
int servoPin = 9;

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
  myservo.attach(servoPin);
  myservo.write(90);
}

void loop() {
  unsigned long currentTime = millis();

  // Update servo position based on state
  switch (currentState) {
    case BASE:
      myservo.write(90);
      break;
    case LEFT:
      myservo.write(0);
      break;
    case RIGHT:
      myservo.write(180);
      break;
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
          if (checkCounterL >= 3) {
            currentState = LEFT;
            checkCounterL = 0;
            Serial.println("Transition to LEFT state.");
          }
        } else if (val1 == LOW && val2 == HIGH) {
          checkCounterR++;
          checkCounterL = 0;
          Serial.println("Motion detected on Right side! Counter R: " + String(checkCounterR));
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
