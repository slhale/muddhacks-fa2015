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

void setup() {
  // Start up the matrix
  matrix.begin();
  //Serial.begin(9600);
  
  // Draw stuff 
//  delay(1000);
//  wipe();
  center();
  //matrix.drawLine(16,15,31,0, white);
  hourHand(12, 0, white);
  hourHand(4,0, blue);
  hourHand(6,0, green);
  hourHand(9,0,red);
  circle();
}

void loop() {
//  int time;
}

void wipe() {
  matrix.fillRect(0,0,32,32,matrix.Color333(0,0,0));
}

void circle() {
  int center = matrix.width()/2;
  int radius = 15;
  
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

void hourHand(int h, int m, int color) {
  float circleFraction = h % 12; // because there are 24 hours
  float minuteAdjustment = (1.0 / 12.0) * ((m % 60) / 60); // Move hour hand within the hour
  float angle = 2*M_PI * ((circleFraction + minuteAdjustment)/ 12.0); // in radians
  Serial.print('angle');
  Serial.println(angle);
  
  int midX = 15;
  int midy = 15;

  if (angle < 1.5708) {
    midX = 16;
    midY = 16;
  } else if (angle < 3.1416) {
    midY = 16;
  } else if (angle > 4.7124) {
    midX = 16;
  }

  int radius = 16; // pixels
  float xLen = radius * cos(angle);
  float yLen = radius * sin(angle);
  int xPixels = midX + xLen;
  int yPixels = midY + yLen;
  Serial.print('x pixels');
  Serial.println(xPixels);
  Serial.print('y pixels');
  Serial.println(yPixels);
  
  matrix.drawLine(midX, midY, xPixels, yPixels, color);

}

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
