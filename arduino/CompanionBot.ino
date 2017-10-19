#include <SPI.h>
#include <Pixy.h>
#include <NewPing.h>
#include "CompanionBot.h"

// Pins
// LED
int ledPin = 13;

// Motor
int motorPin1a = 10;
int motorPin1b = 6;
int motorPin2a = 9;
int motorPin2b = 5;

// Ultrasonic sensors 
int usCenterPin = 0; // Analog pin
int usLeftEchoPin = 4;
int usLeftTrigPin = 2;
int usRightEchoPin = 7;
int usRightTrigPin = 8;

// Constructors 
Pixy pixy;
uint16_t blocks;
NewPing usLeft(usLeftTrigPin, usLeftEchoPin);
NewPing usRight(usRightTrigPin, usRightEchoPin);

// for debug
int incomingByte = 0;

void setup() {
    pinMode(motorPin1a, OUTPUT);
    pinMode(motorPin1b, OUTPUT);
    pinMode(motorPin2a, OUTPUT);
    pinMode(motorPin2b, OUTPUT);

    pinMode(ledPin, OUTPUT);
      
    Serial.begin(9600);
    
    // DEBUG
    Serial.println("Starting...");

    pixy.init();
}

void loop() {
    blocks = pixy.getBlocks();
    static int usCenterDistance = 0, usRightDistance = 0, usLeftDistance = 0; 
    // Note: The Arduino and US have the SAME amount of ADC bits, so it's fairly inaccurate

    /*
    // Gets commands from Serial 
    // FOR DEBUG.
    if (Serial.available() > 0) {
         incomingByte = Serial.read();
    }
    */

    // Reads US value from EZ-something
    usCenterDistance = analogRead(usCenterPin);

    // Reads value from HC-SR04 ultrasonic sensor
    usLeftDistance = usLeft.ping_cm();
    usRightDistance = usRight.ping_cm();

    // Moving logic
    moveBot(usCenterDistance, blockDetection(), usLeftDistance, usRightDistance);


    delay(5); // Shorten if possible. Need to give analogWrite room to "breathe".
}

bool blockDetection(void) {
    // Arduino reads blocks faster than Pixy can transmit, so this set it so that if the colour code isn't visable for more than 10 frames (count value), it sets isBlockDetected = false
    static int count = 0;
    static bool isBlockDetected = false;

    if (!blocks) { // global variable
        count++;
    } else {
        count = 0;
        isBlockDetected = true;
    }

    if (count > 10) { // Can change 10 to something else. Small is better. Discovered empirically.
        isBlockDetected = false;
    }

    return isBlockDetected;
}

// Moves wheels/motors with PWM
void moveWheels(int wheelLeftDirection, int wheelRightDirection, int pwmLeft, int pwmRight) {
    // wheelXDirection:
    //   1 == Forward
    //  -1 == Reverse
    //   0 == Stop

    if (wheelLeftDirection == 1 && wheelRightDirection == 1) { 
        analogWrite(motorPin1a, pwmLeft);
        analogWrite(motorPin1b, 0);
        analogWrite(motorPin2a, pwmRight);
        analogWrite(motorPin2b, 0);
    } else if (wheelLeftDirection == -1 && wheelRightDirection == 1) {
        analogWrite(motorPin1a, 0);
        analogWrite(motorPin1b, pwmLeft);
        analogWrite(motorPin2a, 0);
        analogWrite(motorPin2b, pwmRight);
    } else if (wheelLeftDirection == 1 && wheelRightDirection == -1) {
        analogWrite(motorPin1a, pwmLeft);
        analogWrite(motorPin1b, 0);
        analogWrite(motorPin2a, 0);
        analogWrite(motorPin2b, pwmRight);
    } else if (wheelLeftDirection == -1 && wheelRightDirection == -1) {
        analogWrite(motorPin1a, 0);
        analogWrite(motorPin1b, pwmLeft);
        analogWrite(motorPin2a, 0);
        analogWrite(motorPin2b, pwmRight);
    } else if (wheelLeftDirection == 0 && wheelRightDirection == 0) {
        analogWrite(motorPin1a, 0);
        analogWrite(motorPin1b, 0);
        analogWrite(motorPin2a, 0);
        analogWrite(motorPin2b, 0);
    } else {
        Serial.println("Not a vaild value!");
    }
}

// May want to make a pot that adjusts the max PWM for each motor (connected to analog pins)
void moveBot(int usCenterDistance, int isBlockDetected, int usLeftDistance, int usRightDistance) {
    // Move wheels until bot is in center of object 
    // (with slight offset accounting for distance from center to person's leg).
    // Distance determines how fast it should move
    // Distance of 0 is stop, negative distance is reverse

    int obstaclePosition = usDetect(usLeftDistance, usRightDistance);
    
    if ((!isBlockDetected) || (usCenterDistance < US_MIN_DIST) || (usLeftDistance < US_MIN_DIST) || (usRightDistance < US_MIN_DIST)) {
        moveWheels(0, 0, 0, 0);
    } else if (usCenterDistance > US_MAX_DIST) {
        if (pixy.blocks[0].x > PIXY_WINDOW_LOW && pixy.blocks[0].x < PIXY_WINDOW_HIGH)  {
            if (obstaclePosition == -1) {
                moveWheels(1, 1, 127, 255);
            } else if (obstaclePosition == 1) {
                moveWheels(1, 1, 255, 127);
            } else {
                moveWheels(1, 1, 255, 255);
            }
        } else if (pixy.blocks[0].x > PIXY_WINDOW_HIGH) {
            moveWheels(1, 1, 127, 255);
        } else if (pixy.blocks[0].x < PIXY_WINDOW_LOW) {
            moveWheels(1, 1, 255, 127);
        }
    } 
}

int usDetect(uint16_t usLeftDistance, uint16_t usRightDistance) {
    // returns:
    //   0 -> person in center
    //  -1 -> person on left
    //  +1 -> person on right
    
    if (usLeftDistance < usRightDistance) {
        return -1;
    } else if (usRightDistance < usLeftDistance) {
        return 1;
    } else {
        return 0;
    }
}

// Estimate based on area of CC
uint16_t getPixyDist() {
    // Use global Pixy variable
    
    // Make a lookup table:
    // At xx distance CC is xxx pixels^2; at yy distance, CC is yyy pixels^2; at zzz...
    // Do this for a bunch of values to determine the curve, then plot function accordingly.
    // Use math function to make this function.
    return 0; // dummy value to make compiler not kms
}

void acclerometerStuff() {
    // Does something???
}

/* cosPhi
|   x   |  y  |
===============
| 00.00 | 1.0 |
| 25.84 | 0.9 |
| 36.87 | 0.8 |
| 45.57 | 0.7 |
| 53.13 | 0.6 |
| 60.00 | 0.5 |
| 66.42 | 0.4 |
| 72.54 | 0.3 |
| 78.46 | 0.2 |
| 84.26 | 0.1 |
| 90.00 | 0.0 |
===============*/
float cosPhi(int16_t angle) {
    float ratio;
    bool negative = false;

    angle %= 360;
    if (angle < 0) {
        angle += 360;
    }

    if (angle > 269) {
        angle = 90 - (angle - 270);
    } else if (angle > 179) {
        angle -= 180;
        negative = true;
    } else if (angle > 89) { 
        angle = 90 - (angle - 90);
        negative = true;
    }

    if (angle < 26) {
        ratio = 1.0;
    } else if (angle < 37) {
        ratio = 0.9;
    } else if (angle < 46) {
        ratio = 0.8;
    } else if (angle < 53) {
        ratio = 0.7;
    } else if (angle < 60) {
        ratio = 0.6;
    } else if (angle < 66) {
        ratio = 0.5;
    } else if (angle < 73) {
        ratio = 0.4;
    } else if (angle < 78) {
        ratio = 0.3;
    } else if (angle < 84) {
        ratio = 0.2;
    } else if (angle < 90) {
        ratio = 0.1;
    } else { // angle == 90
        ratio = 0.0;
    }
    
    if (negative && ratio != 0.0) {
        ratio *= -1.0;
    }
    
    return ratio;
}