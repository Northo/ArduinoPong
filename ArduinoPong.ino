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
int POINTS_TO_WIN = 3;
float BAR_SPEED = 1.8;

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

int points_1 = 0;
int points_2 = 0;

void reset_pong() {
  reset();

  bar1 = 16;
  bar2 = 16;

  points_1 = 0;
  points_2 = 0;
}

void control_bar(float &bar) {
  bar = min(HEIGHT-bar_height, bar);
  bar = max(bar_height, bar);
}

void game_won() {
  matrix.fillScreen(0);
  for (int i = 0; i < WIDTH/2; i++) {
    matrix.fillCircle(WIDTH/2, HEIGHT/2, i, matrix.Color333(7,0,0));
    delay(50);
  }
  
  matrix.setCursor(3, 6);
  matrix.setTextSize(0.5);
  matrix.setTextColor(matrix.Color333(7,7,7));
  matrix.print("Game over!");
  delay(3000);
}

void reset() {
  // Resets game
  // Sets balls velocity in x-direction
  // to between 50 and 100 percent of BALL_SPEED
  // sets y to be such that total speed is BALL_SPEED
  x = int(WIDTH/2);
  y = int(HEIGHT/2);
  vx = random(50, 100)*BALL_SPEED/100.0;
  vy = sqrt(sq(BALL_SPEED)-sq(vx));
  int signx = random(-10, 10) > 0? -1 : 1;
  int signy = random(-10, 10) > 0? -1 : 1;
  vx *= signx;
  vy *= signy;
  draw();
}

void draw() {

  matrix.fillScreen(matrix.Color333(0, 0, 0));
  matrix.drawPixel((int) x, (int) y, matrix.Color333(4, 0, 0));
  matrix.drawLine(bar1_x, int(bar1) - bar_height, bar1_x, int(bar1) + bar_height, matrix.Color333(4, 4, 4));
  matrix.drawLine(bar2_x, int(bar2) - bar_height, bar2_x, int(bar2) + bar_height, matrix.Color333(4, 4, 4));

  matrix.setCursor(10, 0);
  matrix.setTextSize(0.5);
  matrix.setTextColor(matrix.Color333(0,7,0));
  matrix.print(points_1);
  matrix.setCursor(20, 0);   // start at top left, with one pixel of spacing
  matrix.print(points_2);

}

void pong() {
  reset_pong();
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

    float delta_bar1 = (2.5 - voltage1)/2.5;
    float delta_bar2 = (2.5 - voltage2)/2.5;
    bar1 += abs(delta_bar1)<0.3 ? 0 : delta_bar1*BAR_SPEED;
    bar2 += abs(delta_bar2)<0.3 ? 0 : delta_bar2*BAR_SPEED;
    control_bar(bar1);
    control_bar(bar2);

    // Check collision with bar
    if (int(x) <= bar1_x and bar1 + bar_height >= y and bar1 - bar_height <= y) {
      vx = abs(vx);
    }

    if (int(x) >= bar2_x and bar2 + bar_height >= y and bar2 - bar_height <= y) {
      vx = -abs(vx);
    }

    // Check collison with top or bottom wall, reflect ball
    if (y <= 0 or y >= HEIGHT) {
      vy = -vy;
    }

    // Check collision with end walls, add points
    if (x <= 0){
      points_2++;
      reset();
      delay(400);
    }
    if(x >= WIDTH){
      points_1++;
      reset();
    }

    x += vx;
    y += vy;

    if(points_1==POINTS_TO_WIN or points_2==POINTS_TO_WIN){
      game_won();
      break;
    }

    delay(rate);
  }
}

void setup() {
  matrix.begin();
}

void loop() {
  matrix.fillScreen(0);
  matrix.setCursor(6, 0);
  matrix.print("Menu");

  matrix.fillRect(6, 15, 4, 4, matrix.Color333(4,4,4));
  matrix.fillCircle(20, 15, 3, matrix.Color333(4,4,4));
  delay(3000);
  pong();
}
