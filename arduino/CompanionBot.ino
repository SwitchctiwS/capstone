#include <SPI.h>
#include <Pixy.h>
#include <MPU9250.h>
#include "CompanionBot.h"

// Pins
// Motor
int motorPin1a = 10;
int motorPin1b = 6;
int motorPin2a = 9;
int motorPin2b = 5;

// LED
int ledPin = 13;

// Analog
int usCenterPin = 0;
int usLeftPin = 1;
int usRightPin = 2;

// Variables
Pixy pixy;
MPU9250 imu;
uint16_t blocks;

int incomingByte = 0;
int analogVal = 0;

void setup() {
    pinMode(motorPin1a, OUTPUT);
    pinMode(motorPin1b, OUTPUT);
    pinMode(motorPin2a, OUTPUT);
    pinMode(motorPin2b, OUTPUT);

    pinMode(ledPin, OUTPUT);
      
    Wire.begin();
    Serial.begin(9600);
    
    Serial.println("Starting...");

    /*IMU setup code
    // Read the WHO_AM_I register, this is a good test of communication
    byte c = imu.readByte(MPU9250_ADDRESS, WHO_AM_I_MPU9250);
    Serial.print("MPU9250 "); Serial.print("I AM "); Serial.print(c, HEX);
    Serial.print(" I should be "); Serial.println(0x71, HEX);
    if (c == 0x71) {
      Serial.println("MPU9250 is online...");

      // Start by performing self test and reporting values
      imu.MPU9250SelfTest(imu.SelfTest);
      Serial.print("x-axis self test: acceleration trim within : ");
      Serial.print(imu.SelfTest[0],1); Serial.println("% of factory value");
      Serial.print("y-axis self test: acceleration trim within : ");
      Serial.print(imu.SelfTest[1],1); Serial.println("% of factory value");
      Serial.print("z-axis self test: acceleration trim within : ");
      Serial.print(imu.SelfTest[2],1); Serial.println("% of factory value");
      Serial.print("x-axis self test: gyration trim within : ");
      Serial.print(imu.SelfTest[3],1); Serial.println("% of factory value");
      Serial.print("y-axis self test: gyration trim within : ");
      Serial.print(imu.SelfTest[4],1); Serial.println("% of factory value");
      Serial.print("z-axis self test: gyration trim within : ");
      Serial.print(imu.SelfTest[5],1); Serial.println("% of factory value");
  
      // Calibrate gyro and accelerometers, load biases in bias registers
      imu.calibrateMPU9250(imu.gyroBias, imu.accelBias);
  
      imu.initMPU9250();
      // Initialize device for active mode read of acclerometer, gyroscope, and
      // temperature
      Serial.println("MPU9250 initialized for active data mode....");
  
      // Read the WHO_AM_I register of the magnetometer, this is a good test of
      // communication
      byte d = imu.readByte(AK8963_ADDRESS, WHO_AM_I_AK8963);
      Serial.print("AK8963 "); Serial.print("I AM "); Serial.print(d, HEX);
      Serial.print(" I should be "); Serial.println(0x48, HEX);
  
      // Get magnetometer calibration from AK8963 ROM
      imu.initAK8963(imu.magCalibration);
      // Initialize device for active mode read of magnetometer
      Serial.println("AK8963 initialized for active data mode....");
      if (SERIAL_DEBUG_OUTPUT) {
        //  Serial.println("Calibration values: ");
        Serial.print("X-Axis sensitivity adjustment value ");
        Serial.println(imu.magCalibration[0], 2);
        Serial.print("Y-Axis sensitivity adjustment value ");
        Serial.println(imu.magCalibration[1], 2);
        Serial.print("Z-Axis sensitivity adjustment value ");
        Serial.println(imu.magCalibration[2], 2);
      }
    } else {
      Serial.print("Could not connect to MPU9250: 0x");
      Serial.println(c, HEX);
      while(1) ; // Loop forever if communication doesn't happen
    }
*/
    
    pixy.init();
}

void loop() {
    blocks = pixy.getBlocks();

    // Arduino reads blocks faster than Pixy can transmit, so this set it so that if the colour code isn't visable for more than 10 frames, it sets isBlockDetected = false
    static int i = 0;
    static int usCenterDistance = 0, usRightDistance = 0, usLeftDistance = 0; // Note: The Arduino and US have the SAME amount of ADC bits, so it's fairly inaccurate
    static bool isBlockDetected = false;

    if (!blocks) {
        i++;
    } else {
        i = 0;
        isBlockDetected = true;
    }

    if (i > 10) { // Can change 10 to something else. Small is better. Discovered empirically.
        isBlockDetected = false;
    }

    /*
    // Gets commands from Serial. DEBUG.
    if (Serial.available() > 0) {
         incomingByte = Serial.read();
    }
    */

    // Reads US value
    usCenterDistance = analogRead(usCenterPin);
    usLeftDistance = analogRead(usLeftPin);
    usRightDistance = analogRead(usRightPin);

    // Moving logic
    moveBot(usCenterDistance, isBlockDetected);

    delay(5); // Shorten if possible. Need to give analogWrite room to "breathe".
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

    int objectPosition = usDetect(usLeftDistance, usRightDistance);
    
    if (usDistance < US_MIN_DIST) {
        moveWheels(0, 0, 0, 0);
    } else if (!isBlockDetected) {
        moveWheels(0, 0, 0, 0);
    } else if (usDistance > US_MAX_DIST) {
        if (isBlockDetected) {
            if (pixy.blocks[0].x > PIXY_WINDOW_LOW && pixy.blocks[0].x < PIXY_WINDOW_HIGH)  {
               moveWheels(1, 1, 127, 127);
            } else if (pixy.blocks[0].x > PIXY_WINDOW_HIGH / 2) {
               moveWheels(1, 1, 127, 255);
            } else if (pixy.blocks[0].x < PIXY_WINDOW_LOW / 2) {
               moveWheels(1, 1, 255, 127);
            }
        }
    }
}

float pixyVel(int deltaPx, int deltaTime) {
   return (float)((deltaPx / deltaTime) * PX_PER_METER);
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

uint16_t getUsDist() {
    // Input capture/analog voltage/UART
    // Ideally use PWM (least noise), then analog, then UART (slow)
    return 0; // dummy value to make compiler not kms
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