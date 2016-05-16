/*
 * This is a minimal example, see extra-examples.cpp for a version
 * with more explantory documentation, example routines, how to
 * hook up your pixels and all of the pixel types that are supported.
 *
 */

#include "application.h"
#include "neopixel/neopixel.h"

SYSTEM_MODE(AUTOMATIC);

// IMPORTANT: Set pixel COUNT, PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_COUNT 100
#define PIXEL_TYPE WS2811

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

int led = D7;


// to monitor serial from command line: C:\> particle serial monitor

void setup()
{
   pinMode(led, OUTPUT);
   Serial.begin(9600);
   
   strip.begin();
   strip.show(); // Initialize all pixels to 'off'

} // end setup


void loop()
{
  lightsOff();
  
  Serial.println("blinking LED 4 times");
  blinkLED(led, 4);
  
  Serial.println("Pixel 0");
  simpleCyclePixel(0);   delay(1000); 
  
  Serial.println("Pixel 1");
  simpleCyclePixel(1);   delay(1000);
  
  Serial.println("Pixel 2");
  simpleCyclePixel(2);   delay(1000);
  
  delay(1000);
  
  Serial.println("Color Wipe");
  // Slowly light up the strand, 1 pixel at a time
  colorWipe(strip.Color(0, 0, 50), 100); // Blue
  delay(1000);

  Serial.println("Rainbow Cycle");
  rainbowCycle(10); 
  delay(1000);
  
  Serial.println("Rainbow");
  rainbow(20);

}


/*==========================================================================
   cycles a pixel through Red, Green, and Blue with a short wait between.
   pixel: which pixel to light up 
  ==========================================================================
*/
void simpleCyclePixel(int pixel) {
  strip.setPixelColor(pixel,strip.Color(50,0,0)); strip.show(); delay(500);
  strip.setPixelColor(pixel,strip.Color(0,50,0)); strip.show(); delay(500);
  strip.setPixelColor(pixel,strip.Color(0,0,50)); strip.show(); delay(500);
  strip.setPixelColor(pixel,strip.Color(0,0,0));   strip.show(); delay(500);
} // end simpleCyclePixel

/*==========================================================================
   turn all the lights off 
  ==========================================================================
*/
void lightsOff() {
  for(int i=0; i<PIXEL_COUNT; i++) {
    strip.setPixelColor(i,strip.Color(0,0,0));
  }
  strip.show();
} // end lightsOff;

//==========================================================================
// Fill the dots one after the other with a color
//==========================================================================
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
} // end colorWipe


//==========================================================================
void blinkLED(int pin, int nTimes) {
  digitalWrite(pin, LOW);
  for(int i=0; i<nTimes; i++) {
    digitalWrite(pin, HIGH);
    delay(300);
    digitalWrite(pin, LOW);
    delay(300);
  }

} // end blinkLED


//==========================================================================
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
} // end rainbowCycle

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
