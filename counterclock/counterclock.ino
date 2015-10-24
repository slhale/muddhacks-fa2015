// code to control clock on 32x32 LED display
// written by: Sarah Hale, Renata Paramastri, Lydia Scharff,
//             and Marina Knittel
// date: 23.10.15


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

void setup() {
  int x, y;
  
}

void loop() {
  
}

