#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"
#include "morley.h"
#include "RGBConverter.h"

#define DATA_PIN 5

#define HEIGHT 41
#define WIDTH 8
#define NUM_PIXELS (HEIGHT * WIDTH)
#define FRAMES_PER_SECOND 200
#define FRAME_DELAY (1000 / FRAMES_PER_SECOND)
#define BRIGHTNESS 64

#define DEBUG false

CRGB pixels[NUM_PIXELS];

const uint8_t kMatrixWidth  = WIDTH;
const uint8_t kMatrixHeight = HEIGHT;
const bool    kMatrixSerpentineLayout = true;

const uint8_t image_width = gimp_image.width;
const uint8_t image_height = gimp_image.height;

CHSV readImage(int x, int y){
  byte red =   gimp_image.pixel_data[3*(x + (y * image_width))];
  byte green = gimp_image.pixel_data[3*(x + (y * image_width)) + 1];
  byte blue =  gimp_image.pixel_data[3*(x + (y * image_width)) + 2];

  double temp_hsv[3];
  RGBConverter::rgbToHsv(red, green, blue, temp_hsv);
  CHSV output(temp_hsv[0]*255, sqrt(temp_hsv[1])*255, dim8_video(temp_hsv[2]*255));
  //CRGB output(red, green, blue);
  return output;
}

uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }

  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  
  return i;
}

int counter = 0;

void makeMorley(){
  if (counter >= image_width){
    counter = 0;
  }
  int offset = (counter < image_width ) ? counter : (counter - image_width);
  for (int j = 0; j < HEIGHT; j++){
    if (DEBUG){
      Serial.print(F("Line: "));
      Serial.print(j);
      Serial.print(F(" "));
    }
    for (int i = 0; i < WIDTH; i++){
      int index = XY(i, j);
      pixels[index] = readImage(i + offset, j);
      if (DEBUG){
        Serial.print(F("("));
        Serial.print(pixels[index].r);
        Serial.print(F(","));
        Serial.print(pixels[index].g);
        Serial.print(F(","));
        Serial.print(pixels[index].b);
        Serial.print(F(") "));
      }
    }
    if (DEBUG){
      Serial.print(F("\n"));
    }
  }
  counter++;
}

void makeRainbow(){
  for (int i = 0; i < WIDTH; i++){
    for (int j = 0; j < HEIGHT; j++){
      int index = XY(i, j);
      pixels[index] = CHSV(255*i/WIDTH, 255, BRIGHTNESS);
    }
  }
}

void setup() {
  LEDS.addLeds<WS2811, DATA_PIN, GRB>(pixels, NUM_PIXELS);
  LEDS.setBrightness(BRIGHTNESS);
  Serial.begin(115200);
}

void loop() {
  //makeRainbow();
  makeMorley();
  FastLED.show();
  delay(FRAME_DELAY);
}
