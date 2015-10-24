// code to control clock on 32x32 LED display
// written by: Sarah Hale, Renata Paramastri, Lydia Scharff,
//             and Marina Knittel
// date: 23.10.15

#include <gamma.h>
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Time.h>
#include <math.h> // Include math for trig funtions for clock hands

#define CLK 11
#define LAT 9
#define OE  10
#define A   A3
#define B   A2
#define C   A1
#define D   A0
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

#define TIME_MSG_LEN 11 // Time sync to PC is HEADER and unix time_t as ten ascii digits
#define TIME_HEADER 'T' // Header tag for serial time sync message
#define TIME_REQUEST 7 // ASCII bell character requests a time sync message

// Setup default colors
int green = matrix.Color333(0,7,0);
int blue = matrix.Color333(0,0,7);
int red = matrix.Color333(7,0,0);
int white = matrix.Color333(7,7,7);
int black = matrix.Color333(0,0,0);

// Timekeeper variables
int lastHour = 0;
int lastMin = 0;
int lastSec = 0;
int currentHour = 0;
int currentMin = 0;
int currentSec = 0;

// State variables
boolean counter = false;
boolean party = false;

void setup() {
  // Start up the matrix
  matrix.begin();
  Serial.begin(9600);

  party = true;
  setTime(1,3,00,24,10,2015); // change this

}

void loop() {
  // Keep track of the current and last times 
  lastHour = currentHour;
  lastMin = currentMin;
  lastSec = currentSec;
  currentHour = hour();
  currentMin = minute();
  currentSec = second();
  
//  Serial.print("hour ");
//  Serial.println(currentHour);
  
  // If the minute has changed, update the clock 
  if ( (currentMin > lastMin) ||
       ((currentMin == 0) && (lastMin != 0)) ) {
    // Write in this order so that we don't have overlapping or invisible lines. 
    wipe();
    minuteHand(currentMin, blue);
    hourHand(currentHour, currentMin, white);
    circle();
  }
  
}

/**
 * Set all the pixels of the matrix to be dark. 
 */
void wipe() {
  matrix.fillRect(0,0,32,32,matrix.Color333(0,0,0));
}

/**
 * Draw a circle. 
 * The circle has a thickness of about 4 pixels and a
 * diameter equal to the sidelength of the matrix square. 
 */
void circle() {
  int center = matrix.width()/2;
  int radius = 15;
  
  if (party) {
    // Draw a filled in circle if we are in party mode
    matrix.fillCircle(center,center,radius+1, matrix.Color333(0,7,0));
    
  } else {
    // Draw the outline of a circle if we are NOT in party mode
    
    // Draw multiple circles with different center points because the board 
    // does not have an exact center point. There are 4 'centers' in a square.
    matrix.drawCircle(center,center, radius, matrix.Color333(0,7,0));
    matrix.drawCircle(center-1,center, radius, matrix.Color333(0,7,0));
    matrix.drawCircle(center-1,center-1, radius, matrix.Color333(0,7,0));
    matrix.drawCircle(center,center-1, radius, matrix.Color333(0,7,0));
  }
}


void line() {
  matrix.drawLine(0,0,15,15, matrix.Color333(7,7,7));
}

void center() {
  matrix.drawPixel(15,15, matrix.Color333(7,7,7));
  matrix.drawPixel(15,16, matrix.Color333(7,0,0));
  matrix.drawPixel(16,15, matrix.Color333(0,7,0));
  matrix.drawPixel(16,16, matrix.Color333(0,0,7));
}

/**
 * Draws the hour hand on the clock face
 * @param h, the hour from 0 to 24
 * @param m, the minute
 * @param color, the color that the drawn hand should be
 */
void hourHand(int h, int m, int color) {
  // Calculate the angle of the hour hand. 
  float circleFraction = h % 12; // because there are 24 hours
  float hourAngle = 2.0*M_PI * circleFraction / 12.0; // in radians
  
  float minuteAngle = ((double) (m % 60)) * M_PI / 360.0;
  float angle = hourAngle + minuteAngle;
  
  // If we are in counter-clock mode, then the angle is reversed so 
  // that the hands progress in the opposite direction. 
  if (counter) {
    angle = -1 * angle;
  }
  
  int midX = 15;
  int midY = 15;
  int radius = 10;
  float xLen = radius * cos(angle);
  float yLen = radius * sin(angle);
  
  // Adjust the middle starting positions depending on the 
  // quadrant of the hour, for prettiness 
  if (xLen > 0) {
    midX = 16;
  }
  if (yLen > 0) {
    midY = 16;
  }
  
  int xPixels = midX + (int)xLen;
  int yPixels = midY + (int)yLen;
  
  Serial.println(angle);
  
  // Normally the hand is colored on a black background, but if we 
  // are in party mode, then the background is colored and the 
  // hand is black. 
  if (party) {
    matrix.drawLine(midX, midY, xPixels, yPixels, black);
  } else {
    matrix.drawLine(midX, midY, xPixels, yPixels, color);
  }
}

/**
 * Draws the minute hand on the clock face
 * @param m, the minute
 * @param color, the color that the drawn hand should be
 */
void minuteHand(int m, int color) {
  float angle = ((double) (m % 60)) * M_PI / 30.0;
  
  // If we are in counter-clock mode, then the angle is reversed so 
  // that the hands progress in the opposite direction. 
  if (counter) {
    angle = -1 * angle;
  }
  
  int midX = 15;
  int midY = 15;
  int radius = 15;
  float xLen = radius * cos(angle);
  float yLen = radius * sin(angle);
  
  // Adjust the middle starting positions depending on the 
  // quadrant of the hour, for prettiness 
  if (xLen > 0) {
    midX = 16;
  }
  if (yLen > 0) {
    midY = 16;
  }
  int xPixels = midX + (int)xLen;
  int yPixels = midY + (int)yLen;
  
  // Normally the hand is colored on a black background, but if we 
  // are in party mode, then the background is colored and the 
  // hand is black. 
  if (party) {
    matrix.drawLine(midX, midY, xPixels, yPixels, matrix.Color333(0,0,0));
  } else {
    matrix.drawLine(midX, midY, xPixels, yPixels, color);
  }

  void draw() {
    if (!party) {
      
    }
  }
}

