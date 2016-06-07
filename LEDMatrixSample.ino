// This #include statement was automatically added by the Particle IDE.
#include "neopixel/neopixel.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_GFX/Adafruit_GFX.h"

/*-------------------------------------------------------------------------
  Spark Core library to control WS2811/WS2812 based RGB
  LED devices such as Adafruit NeoPixel strips and matrices.
  Currently handles 800 KHz and 400kHz bitstream on Spark Core,
  WS2812, WS2812B and WS2811.

  Also supports Radio Shack Tri-Color Strip with TM1803 controller
  400kHz bitstream.

  PLEASE NOTE that the NeoPixels require 5V level inputs
  and the Spark Core only has 3.3V level outputs. Level shifting is
  necessary, but will require a fast device such as one of the following:

  [SN74HCT125N]
  http://www.digikey.com/product-detail/en/SN74HCT125N/296-8386-5-ND/376860

  [SN74HCT245N]
  http://www.digikey.com/product-detail/en/SN74HCT245N/296-1612-5-ND/277258

  [TXB0108PWR]
  http://www.digikey.com/product-search/en?pv7=2&k=TXB0108PWR

  If you have a Spark Shield Shield, the TXB0108PWR 3.3V to 5V level
  shifter is built in.

  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries.
  Modified to work with Spark Core by Technobly.
  Modified for use with Matrices by delianides.
  Contributions by PJRC and other members of the open source community.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!
  --------------------------------------------------------------------*/

#include "application.h"
#include "neomatrix/neomatrix.h"

#define PIXEL_COUNT 100

// IMPORTANT: Set pixel PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_TYPE WS2811 // set this to get RGB values even though it is 400KHZ


  // create a reference to the whole strand, including overflow pixels not used in the matrix
  Adafruit_NeoPixel wholeStrip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
  
// MATRIX DECLARATION:
// Parameter 1 = width of EACH NEOPIXEL MATRIX (not total display)
// Parameter 2 = height of each matrix
// Parameter 3 = number of matrices arranged horizontally
// Parameter 4 = number of matrices arranged vertically
// Parameter 5 = pin number (most are valid)
// Parameter 6 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the FIRST MATRIX; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs WITHIN EACH MATRIX are
//     arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns WITHIN
//     EACH MATRIX proceed in the same order, or alternate lines reverse
//     direction; pick one.
//   NEO_TILE_TOP, NEO_TILE_BOTTOM, NEO_TILE_LEFT, NEO_TILE_RIGHT:
//     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
//     two, e.g. NEO_TILE_TOP + NEO_TILE_LEFT for the top-left corner.
//   NEO_TILE_ROWS, NEO_TILE_COLUMNS: the matrices in the OVERALL DISPLAY
//     are arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   NEO_TILE_PROGRESSIVE, NEO_TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
//     (tiles) in the OVERALL DISPLAY proceed in the same order for every
//     line, or alternate lines reverse direction; pick one.  When using
//     zig-zag order, the orientation of the matrices in alternate rows
//     will be rotated 180 degrees (this is normal -- simplifies wiring).
//   See example below for these values in action.
// Parameter 7 = pixel type flags: for the Photon/Electron library, we have
//   to pick pre-consolidated values and we can't use the regular Adafruit
//   library approach of just adding flag values together :( 

// Example with one 22x9 matrix.  
// The first pixel within is at the top left.  The matrices use zig-zag line ordering.
// There's only one matrix here, so it doesn't matter if we declare it in row
// or column order.  

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(11,9,1,1, PIXEL_PIN,
  NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_PROGRESSIVE +
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  PIXEL_TYPE);

const uint16_t colors[] = {
  matrix.Color(128, 25, 0), matrix.Color(5, 179, 5), matrix.Color(0, 98, 209) };

String webText = "@ the House of Fur!";

//-------------------------------------------------------------------------------
void setup() {
   // turn off all the pixels in the strip, even the overflow pixels that 
   // aren't used in the matrix. Sometimes they turn on solid white
   wholeStrip.begin();    wholeStrip.show(); 

  Serial.begin(9600);
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(20);
  matrix.setTextColor(matrix.Color(80,80,0));
  
  Particle.function("passText", handleText);
  Particle.variable("messageText", &webText, STRING);

} // end setup

//--------------------------------------------------------------------------------
// handler for the passText function
// saves the text to a local var and then puts the current message back to the
// cloud for remote read
int handleText(String actualText) {
    Serial.print("Message received:");
    Serial.println(actualText);
    webText = actualText;
    Particle.variable("messageText", &webText, STRING);
    return 0;
}

//--------------------------------------------------------------------------------
void loop() {
  matrix.fillScreen(0);
  
  //Serial.println("Color Wipe in pixel order");
  colorWipe(matrix.Color(50,0,50), 25);
  delay(500);
  
  matrix.fillScreen(0);
  
  // figure ou the matrix addressing model
  //Serial.println("left to right, top to bottom wipe");
  // left to right, top to bottom wipe
  int x,y;
  for(y=0; y<9; y++) {
     for(x=0; x<11; x++) { 
         //Serial.print("X: "); Serial.print(x); Serial.print("; Y: "); Serial.println(y);
         matrix.drawPixel(x,y,matrix.Color(0,128,128));   
         matrix.show();
         delay(25);
     } 
     //Serial.println("-----------");
      
  }
  
 
  matrix.fillScreen(0); delay(500);
  
  // fix the screen with green 
   matrix.fillRect(0, 0, 11, 9, matrix.Color(0, 70, 0)); matrix.show();
   delay(500); 
   matrix.fillScreen(0); delay(500);
 
 
   // draw a box in yellow 
   // drawRect parameters: x,y starting cooridnates, then width and height, then color
   matrix.drawRect(0, 0, 11, 9, matrix.Color(70, 70, 0)); matrix.show();
   delay(500); 
   matrix.fillScreen(0); delay(500);
    
   // draw an 'X' in red 
   matrix.drawLine(1, 0, 9, 8, matrix.Color(70, 0, 0)); 
   matrix.drawLine(9, 0, 1, 8, matrix.Color(70, 0, 0)); 
   matrix.show();
   delay(500); 
   matrix.fillScreen(0); delay(500);
    
   // draw a blue circle 
   // drawCircle parameters: x,y center point, then radius and color
   matrix.drawCircle(5, 4, 3, matrix.Color(0, 0, 70)); matrix.show();
   delay(500); 
   matrix.fillScreen(0); delay(500);
    
   // fill a violet circle 
   matrix.fillCircle(5, 4, 4, matrix.Color(70, 0, 70)); matrix.show();
   delay(500); 
   matrix.fillScreen(0); delay(500);
    
    Serial.println(webText);
    scrollText(webText);

   delay(100);
} // end loop


//==========================================================================
// Scroll text
//==========================================================================
void scrollText(String& text) {
   int w    = matrix.width();
   //String message = "Hello World";

   int len = text.length();
   int iterations = (len+2) * 6; // font is 6 wide x 8 tall

    matrix.fillScreen(0);
    matrix.setCursor(0, 0);
//    matrix.print("Hyl"); matrix.show(); delay(5000);
  
  for(int i=0; i<=iterations; i++) {
    //Serial.print("Scroll Text iteration: "); Serial.println(i);
    matrix.fillScreen(0);
    matrix.setCursor(w, 0);
    matrix.print(text);
    --w; // decrement w to scroll to the left

    matrix.show();   delay(100);
  } // end for
  
  
} // end scrollText



//==========================================================================
// Fill the dots one after the other with a color
//==========================================================================
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<matrix.numPixels(); i++) {
    matrix.setPixelColor(i, c);
    matrix.show();
    delay(wait);
  }
} // end colorWipe


/*==========================================================================
   turn all the lights off 
  ==========================================================================
*/
void lightsOff() {
  for(int i=0; i<PIXEL_COUNT; i++) {
    matrix.setPixelColor(i,matrix.Color(0,0,0));
  }
  matrix.show();
} // end lightsOff;



