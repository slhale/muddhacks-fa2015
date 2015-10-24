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
int orange = matrix.Color333(7,2,0);
int yellow = matrix.Color333(7,6,0);
int purple = matrix.Color333(6,0,5);
int white = matrix.Color333(7,7,7);
int black = matrix.Color333(0,0,0);

int rainbow[] = {red, orange, yellow, green, blue, purple};
char stems[] = "stems ";

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
boolean stemsParty = false;
boolean dinner = false;

// changes probability for counter-clock
// higher threshhold, higher probability
int threshhold = 50;

void setup() {
  // Start up the matrix
  matrix.begin();
  Serial.begin(9600);

  //party = true;
  //counter = true;
  //stemsParty = true;
  
  setTime(0,59,50,24,10,2015); // change this
}

void loop() {
  // Keep track of the current and last times 
  lastHour = currentHour;
  lastMin = currentMin;
  lastSec = currentSec;
  currentHour = hour();
  currentMin = minute();
  currentSec = second();

  Serial.println(currentHour); // somehow it needs the printing
    // for emotional support
  
  if (stemsParty) {
    if (currentHour % 12 == 3 && currentMin == 59) {
      drawStems();
    } else {
      stemsParty = false;
      draw();
    } 
  } else if (dinner) {
    if (currentHour == 5+12 && currentMin == 0) {
      drawDinner();
    } else {
      dinner = false;
      draw();
    } 
  } else { // aka when it's actually a clock
    // If the minute has changed, update the clock 
    if ( currentMin != lastMin ) {
      // Have a chance of switching to or from counter-clock 
      // when the hour hand is in a vertical position (12 or 
      // 6 o'clock)
      if (currentHour % 6 == 0 && currentMin == 0) {
        randomize();
      }
      
      // Check when to initiate a party
      // Parties are from midnight to 1 am
      if (currentHour == 0) {
        party = true;
      } else {
        party = false;
      }
      
      // Check when to intiate the stems party
      // Stems parties start at 3:59 am or pm 
      if (currentHour % 12 == 3 && currentMin == 59) {
        stemsParty = true;
      }
      
      // Check when to intiate the dinner alert
      // Dinner is at 5 pm 
      if (currentHour == 5+12 && currentMin == 0) {
        dinner = true;
      }
     
      draw();
    }
  }
}


void randomize() {
  int randNum = random(100);
  if (randNum < threshhold) {
    counter = true;
  } else {
    counter = false;
  }
}

/**
 * Draw all the components of the clock.
 * Components include the background, the hour hand, 
 * and the minute hand.
 */
void draw() {
  wipe();
  // Change order depending on party mode because the 
  // drawing overwrites each other. 
  if (party) {
    partyCircle();
    minuteHand(currentMin,0);
    hourHand(currentHour, currentMin, 0);
  } else {
    minuteHand(currentMin, blue);
    hourHand(currentHour, currentMin, white);
    circle(green);
  }
}

void drawStems() {
  int colorStart = 0;
  int heightStart = 0;
  boolean down = true;

  // Rainbow stems bouncing up and down
  for (int i = 0; i < 18; i ++) {
    wipe();
    writeStems(1, heightStart * 6, colorStart, false);

    colorStart ++;
      
    // Move up or down
    if (down) {
      heightStart ++;
    } else {
      heightStart --;
    }

    // Reverse directions
    if (heightStart == 0 || heightStart == 4) {
      down = !down;
    }

    // Restart color array
    if (colorStart == 6) {
      colorStart = 0;
    }

    delay(100);
  }
 
  int xStart = -32;
  colorStart = 0;

  // Slide red stems cross to center of screen
  for (int i = 0; i < 34; i ++) {
    wipe();
    writeStems(xStart + i, 12, 0, true);

    delay(100);
  }   

  delay(900);

  // Flash stems 4 times
  for (int i = 0; i < 4; i ++) {
    wipe();
    delay(1000);
    writeStems(1, 12, 0, true);
    delay(1000);
  }

  wipe();
  delay(500);
}

void drawDinner() {
  
}

/**
 * Set all the pixels of the matrix to be dark. 
 */
void wipe() {
  matrix.fillRect(0,0,32,32,black);
}

/**
 * Draw a circle. 
 * The circle has a thickness of about 4 pixels and a
 * diameter equal to the sidelength of the matrix square. 
 * @param color, the color to draw the circle as (this will
 *               be ignored in party mode)
 */
void circle(int color) {
  int center = matrix.width()/2;
  int radius = 15;
  
  if (party) {
    // Draw a filled in circle if we are in party mode
    partyCircle();
    
  } else {
    // Draw the outline of a circle if we are NOT in party mode
    
    // Draw multiple circles with different center points because the board 
    // does not have an exact center point. There are 4 'centers' in a square.
    matrix.drawCircle(center,center, radius, color);
    matrix.drawCircle(center-1,center, radius, color);
    matrix.drawCircle(center-1,center-1, radius, color);
    matrix.drawCircle(center,center-1, radius, color);
  }
}

// Copy of colorwheel from the example thing 
void partyCircle() {
  int      x, y, hue;
  float    dx, dy, d;
  uint8_t  sat, val;
  uint16_t c;
  for(y=0; y < matrix.width(); y++) {
    dy = 15.5 - (float)y;
    for(x=0; x < matrix.height(); x++) {
      dx = 15.5 - (float)x;
      d  = dx * dx + dy * dy;
      if(d <= (16.5 * 16.5)) { // Inside the circle(ish)?
        hue = (int)((atan2(-dy, dx) + PI) * 1536.0 / (PI * 2.0));
        d = sqrt(d);
        if(d > 15.5) {
          // Do a little pseudo anti-aliasing along perimeter
          sat = 255;
          val = (int)((1.0 - (d - 15.5)) * 255.0 + 0.5);
        } else
        {
          // White at center
          sat = (int)(d / 15.5 * 255.0 + 0.5);
          val = 255;
        }
        c = matrix.ColorHSV(hue, sat, val, true);
      } else {
        c = 0;
      }
      matrix.drawPixel(x, y, c);
    }
  }
}

/**
 * Draws the hour hand on the clock face
 * @param h, the hour from 0 to 24
 * @param m, the minute
 * @param color, the color that the drawn hand should be 
 *               (this will be ignored in party mode)
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
  
  // Normally the hand is colored on a black background, but if we 
  // are in party mode, then the background is colored and the 
  // hand is black. 
  if (party) {
    matrix.drawLine(midX, midY, xPixels, yPixels, matrix.Color333(0,0,0));
  } else {
    matrix.drawLine(midX, midY, xPixels, yPixels, color);
  }
}

/**
 * Draws the minute hand on the clock face
 * @param m, the minute
 * @param color, the color that the drawn hand should be
 *               (this will be ignored in party mode)
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
    matrix.drawLine(midX, midY, xPixels, yPixels, black);
  } else {
    matrix.drawLine(midX, midY, xPixels, yPixels, color);
  }
}

void writeStems(int xloc, int yloc, int startColor /* 0 = red...5 = purple */, boolean solid) {
  if (solid) {
    matrix.setTextColor(rainbow[startColor]);

    for (int i = 0; i < 5; i ++) {
      matrix.print(stems[i]);      
    }
  } else {
    matrix.setCursor(xloc, yloc);
    int colorNum = startColor;
    int stemsNum = 0;

    while (colorNum < 6) {
      matrix.setTextColor(rainbow[colorNum]);
      matrix.print(stems[stemsNum]);
      colorNum ++;
      stemsNum ++;
    }

    colorNum = 0;

    while (colorNum < startColor) {
      matrix.setTextColor(rainbow[colorNum]);
      matrix.print(stems[stemsNum]);
      colorNum ++;
      stemsNum ++;
    }
  }
}

