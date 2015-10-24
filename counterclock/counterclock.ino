// code to control clock on 32x32 LED display
// written by: Sarah Hale, Renata Paramastri, Lydia Scharff,
//             and Marina Knittel
// date: 23.10.15

#include <gamma.h>
#include <Adafruit_GFX.h>   // Core graphics library
#include <RGBmatrixPanel.h> // Hardware-specific library

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
# TIME_REQUEST 7 // ASCII bell character requests a time sync message

void setup() {
  int x, y;
  Serial.print('testing, hello there');
}

void loop() {
  int time;
}

void getPCtime() {
  // If time available from serial port, sync the DateTime library
  while (Serial.available() >= TIME_MSG_LEN) { // Time message
    if (Serial.read() == TIME_HEADER) {
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
