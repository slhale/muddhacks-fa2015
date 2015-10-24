// code to control clock on 32x32 LED display
// written by: Sarah Hale, Renata Paramastri, Lydia Scharff,
//             and Marina Knittel
// date: 23.10.15

#include <gamma.h>
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library
#include <Time.h>
#include <math.h> // Include math for trig funtions for clock hands
#include "RTClib.h"

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
RTC_DS1307 RTC;

// Setup default colors
int green = matrix.Color333(0,7,0);
int blue = matrix.Color333(0,0,7);
int red = matrix.Color333(7,0,0);
int white = matrix.Color333(7,7,7);

int lastHour = 0;
int lastMin = 0;
int lastSec = 0;
int currentHour = 0;
int currentMin = 0;
int currentSec = 0;

void setup() {
  // Start up the matrix
  matrix.begin();
  Serial.begin(9600);
  setTime(00,00,00,24,10,2015);
  
  // Draw stuff 
//  delay(1000);
//  wipe();
  //center();
  //matrix.drawLine(16,15,31,0, white);
//  hourHand(12, 0, white);
  hourHand(4,0, blue);
  hourHand(4,20, green);
  hourHand(5,0,white);
//  hourHand(9,0,red);
  //matrix.drawPixel(16,16,matrix.Color333(0,7,7));

  minuteHand(50,red);


  circle();
}

void loop() {
  lastHour = currentHour;
  lastMin = currentMin;
  lastSec = currentSec;
  currentHour = hour();
  currentMin = minute();
  currentSec = second();
  
  Serial.print("hour ");
  Serial.println(currentHour);
  
  if ( (currentSec > lastSec) ||
       ((currentSec == 0) && (lastSec != 0)) ){
    wipe();
    hourHand(currentHour, currentMin, white);
    minuteHand(currentMin, blue);
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
  
  // Draw multiple circles with different center points because the board 
  // does not have an exact center point. There are 4 'centers' in a square.
  matrix.drawCircle(center,center, radius, matrix.Color333(0,7,0));
  matrix.drawCircle(center-1,center, radius, matrix.Color333(0,7,0));
  matrix.drawCircle(center-1,center-1, radius, matrix.Color333(0,7,0));
  matrix.drawCircle(center,center-1, radius, matrix.Color333(0,7,0));
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

  /*
  Serial.print('angle');
  Serial.println(angle);
  */
  
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
  
//  Serial.print('x pixels');
//  Serial.println(xPixels);
//  Serial.print('y pixels');
//  Serial.println(yPixels);


  Serial.println(angle);

/*
  Serial.println(xLen);
  Serial.println(yLen);

  Serial.println(yPixels);
  Serial.println(midY);
*/
  
  matrix.drawLine(midX, midY, xPixels, yPixels, color);
}

/**
 * Draws the minute hand on the clock face
 * @param m, the minute
 * @param color, the color that the drawn hand should be
 */
void minuteHand(int m, int color) {
  float angle = ((double) (m % 60)) * M_PI / 30.0;

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
  
  matrix.drawLine(midX, midY, xPixels, yPixels, color); 
}
/*
void getPCtime() {
  // If time available from serial port, sync the DateTime library
  while (Serial1.available() >= TIME_MSG_LEN) { // Time message
    if (Serial1.read() == TIME_HEADER) {
      time_t pctime = 0;
      
      for (int i = 0; i < TIME_MSG_LEN - 1; i ++) {
        char c = Serial.read();
        
        if (c >= '0' && c <= '9') {
          pctime = (10 * pctime) + (c - '0'); // Converts digits to a number
        }
          
          setTime(pctime); // Sync DateTime clock to the time received on the serial port
      }
    }
  }
}
*/


void getTime() {
  time_t t;
  int c;

  do {
    Serial.println("?T");
    t = 0;

    while ('T' != getb());
    while (('0' <= (c = getb())) && (c <= '9')) {
      t = 10 * t + (c & 0x0F);
    }

    while (c != '!');
      set_time(ss(t));
}

static int getb(void) {
  while (!(Serial.available() > 0));
    return Serial.read();
}

void updateTime() {
  RTC.adjust(DateTime(__DATE__, __TIME__));
  DateTime now = RTC.now();

  Serial.print(now.hour(), DEC);
  Serial.print(now.minute(), DEC);
}
