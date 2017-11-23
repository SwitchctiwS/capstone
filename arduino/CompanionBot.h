// Pixy
#define PIXY_WINDOW_LOW 106
#define PIXY_WINDOW_HIGH 213

// Ultrasonic
#define US_MIN_DIST 30  // In cm, found through testing
#define US_MAX_DIST 40 // "    "

#define US_RIGHT_MIN_DIST 40
#define US_RIGHT_MAX_DIST 100

#define US_LEFT_MIN_DIST 40
#define US_LEFT_MAX_DIST 100

#define US_RIGHT_DIFFERENTIAL 20
#define US_LEFT_DIFFERENTIAL 20

// Counts
#define MAX_COUNT 10

#define DIST_HYST 10 // +-Value determined through testing because Pixy is less accurate than US
                     
// Movement
#define WHEEL_DISTANCE 30 // Distance from center of each wheel in cm
#define WHEEL_RADIUS 5    // Radius of wheel in cm

// Conversion
#define PX_PER_METER 30

// Math
#define PI 3.14159