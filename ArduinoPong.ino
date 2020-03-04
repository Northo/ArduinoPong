// testshapes demo for Adafruit RGBmatrixPanel library.
// Demonstrates the drawing abilities of the RGBmatrixPanel library.
// For 32x32 RGB LED matrix:
// http://www.adafruit.com/products/607

// Written by Limor Fried/Ladyada & Phil Burgess/PaintYourDragon
// for Adafruit Industries.
// BSD license, all text above must be included in any redistribution.

#include <RGBmatrixPanel.h>

// Most of the signal pins are configurable, but the CLK pin has some
// special constraints.  On 8-bit AVR boards it must be on PORTB...
// Pin 8 works on the Arduino Uno & compatibles (e.g. Adafruit Metro),
// Pin 11 works on the Arduino Mega.  On 32-bit SAMD boards it must be
// on the same PORT as the RGB data pins (D2-D7)...
// Pin 8 works on the Adafruit Metro M0 or Arduino Zero,
// Pin A4 works on the Adafruit Metro M4 (if using the Adafruit RGB
// Matrix Shield, cut trace between CLK pads and run a wire to A4).

#define CLK  8   // USE THIS ON ARDUINO UNO, ADAFRUIT METRO M0, etc.
//#define CLK A4 // USE THIS ON METRO M4 (not M0)
//#define CLK 11 // USE THIS ON ARDUINO MEGA
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

int rate = 50;
const int WIDTH = 31;
const int HEIGHT = 31;

/*******
* PONG *
*******/
float BALL_SPEED = 1;

// NB. Initial pos 0 and positive velocity will fuck it up
// (No way that it can reach the state x=0, vx>0, so this is an impossible initial state).
float x = 1;
float y = 1;
float vx = sin(1)*BALL_SPEED;
float vy = cos(1)*BALL_SPEED;

float bar1 = 16;
float bar2 = 16;

int bar1_x = 1;
int bar2_x = 30;

int bar_height = 2;

int k=0;
int l=0;

void control_bar(float &bar) {
  bar = min(HEIGHT-bar_height, bar);
  bar = max(bar_height, bar);
}

void reset() {
  // Resets game
  x = int(WIDTH/2);
  y = int(HEIGHT/2);
  float dir = (float)random(100);
  vx = sin(dir)*BALL_SPEED;
  vy = cos(dir)*BALL_SPEED;
  draw();
  delay(400);
}

void draw() {

  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.drawPixel((int) x, (int) y, matrix.Color333(4, 0, 0));
  matrix.drawLine(bar1_x, int(bar1) - bar_height, bar1_x, int(bar1) + bar_height, matrix.Color333(4, 4, 4));
  matrix.drawLine(bar2_x, int(bar2) - bar_height, bar2_x, int(bar2) + bar_height, matrix.Color333(4, 4, 4));

  matrix.setCursor(10, 0);
  matrix.setTextSize(0.5);
  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.print(k);

  matrix.setCursor(20, 0);   // start at top left, with one pixel of spacing
  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.print(l);

}

void pong() {
  while(true) {
    /***********
     * Measure *
     ***********/
    int sensorValue1 = analogRead(A4);
    int sensorValue2 = analogRead(A5);
    float voltage1 = sensorValue1 * (5.0 / 1023.0);
    float voltage2 = sensorValue2 * (5.0 / 1023.0);

    /********
     * Draw *
     ********/
    draw();

    /**************
     * Game logic *
     **************/

    bar1 += (2.5 - voltage1)/3;
    control_bar(bar1);
    bar2 += (2.5 - voltage2)/3;
    control_bar(bar2);

    // Check collision with bar
    if (int(x) <= bar1_x and bar1 + bar_height >= y and bar1 - bar_height <= y) {
      vx = abs(vx);
    }

    if (int(x) >= bar2_x and bar2 + bar_height >= y and bar2 - bar_height <= y) {
      vx = -abs(vx);
    }

    // Check collision with wall
    if (x <= 0){
      vx = -vx;
      k++;
     // start at top left, with one pixel of spacing
      delay(100);
      reset();
      /*matrix.print("Lost!");
      vx = 0;
      vy = 0;
      while (true){}*/
    }
    if(x >= WIDTH){
      vx=-vx;
      l++;
      delay(100);
      reset();
      }
    if (y <= 0 or y >= HEIGHT) {
      vy = -vy;
    }

    x += vx;
    y += vy;
    delay(rate);

    if(k==11 or l==11){
      matrix.fillScreen(matrix.Color333(5, 0, 0));
      matrix.setCursor(10, 15);
      matrix.setTextSize(0.5);
      matrix.setTextColor(matrix.Color333(0,7,0));
      matrix.print('GAME OVER');
      delay(10000);
    }
  }
}

void setup() {
  matrix.begin();
}

void loop() {
  pong();
}
