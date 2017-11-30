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

    Serial.begin(9600);
    
    // DEBUG
    Serial.println("Starting...");

    pixy.init();
}

void loop() {
    blocks = pixy.getBlocks();
    static int usRightDistance = 0, usLeftDistance = 0; 

    // Reads value from HC-SR04 ultrasonic sensor
    usLeftDistance = usLeft.ping_cm();
    usRightDistance = usRight.ping_cm();


    //DEBUG
    Serial.print(usLeftDistance);
    Serial.print("\t");
    Serial.print(usRightDistance);
    Serial.print("\t");

    // Moving logic
    moveBot(usLeftDistance, usRightDistance);


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

    if (count > MAX_COUNT) { // Can change 10 to something else. Small is better. Discovered empirically.
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
void moveBot(int usLeftDistance, int usRightDistance) {
    // Move wheels until bot is in center of object 
    // (with slight offset accounting for distance from center to person's leg).
    // Distance determines how fast it should move
    // Distance of 0 is stop, negative distance is reverse

    bool isBlockDetected = blockDetection();
    bool isObstacleDetected = usObstacleDetected(usLeftDistance, usRightDistance);
    int obstaclePosition= usObstaclePosition(usLeftDistance, usRightDistance);
    
    if ((!isBlockDetected) || (isObstacleDetected)) {
        moveWheels(0, 0, 0, 0);

        //DEBUG
        Serial.println("Stopped");

    } else {
        if (pixy.blocks[0].x > PIXY_WINDOW_HIGH) {
            moveWheels(1, 1, 127, 255);

            //DEBUG
            Serial.println("Left");

        } else if (pixy.blocks[0].x < PIXY_WINDOW_LOW) {
            moveWheels(1, 1, 255, 127);

            //DEBUG
            Serial.println("Right");

        } else {
            if (obstaclePosition == -1) {
                moveWheels(1, 1, 127, 255);

                //DEBUG
                Serial.println("UsLeft");

            } else if (obstaclePosition == 1) {
                moveWheels(1, 1, 255, 127);

                //DEBUG
                Serial.println("UsRight");

            } else {
                moveWheels(1, 1, 255, 255);

                //DEBUG
                Serial.println("Forward");

            }
        }
    }
}

// Counts to remove jitter
bool usObstacleDetected(uint16_t usLeftDistance, uint16_t usRightDistance) {
    // returns:
    //  true -> good
    //  false -> bad
    static int count = 0;
    static bool isObstacleDetected = true;

    if ((usLeftDistance < US_LEFT_MIN_DIST) || (usRightDistance < US_RIGHT_MIN_DIST)) {
        count++;
    } else {
        count = 0;
        isObstacleDetected = false;
    }
    
    if (count > MAX_COUNT) {
        isObstacleDetected = true;
    }

    return isObstacleDetected;
}

// Counts to remove jitter
int usObstaclePosition (uint16_t usLeftDistance, uint16_t usRightDistance) {
    // returns:
    //   0 -> person in center
    //  -1 -> person on left
    //  +1 -> person on right

    static int leftCount = 0;
    static int rightCount = 0;
    static int middleCount = 0;
    static int usObstaclePosition = 0; 
    
    // If person is closer than the max allowed value, then execute US code
    if ((usLeftDistance < US_LEFT_MAX_DIST) || usRightDistance < US_RIGHT_MAX_DIST) {
        // if person's US distance on one sensor + a difference value (hysteresis value) is less than other US sensor, then turn the other way
        if ((usLeftDistance + US_LEFT_DIFFERENTIAL) < usRightDistance) {
            leftCount++;
            rightCount = 0;
            middleCount = 0;
        } else if ((usRightDistance + US_RIGHT_DIFFERENTIAL) < usLeftDistance) {
            rightCount++;
            leftCount = 0;
            middleCount = 0;
        } else {
            middleCount++;
            leftCount = 0;
            rightCount = 0;
        }

        if (leftCount > MAX_COUNT) {
            usObstaclePosition = -1; 
        } else if (rightCount > MAX_COUNT) {
            usObstaclePosition = 1;
        } else if (middleCount > MAX_COUNT) {
            usObstaclePosition = 0;
        }
    }

    return usObstaclePosition;
}
