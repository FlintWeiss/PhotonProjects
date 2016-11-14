#include "application.h"
#include "neopixel/neopixel.h"
#include "neomatrix/neomatrix.h"
#include "Adafruit_GFX/Adafruit_GFX.h"
#include "ColorDefinitions.h"
#include "SpriteBitmaps.h"

/*-------------------------------------------------------------------------
  Message Board One: an internet connected message board for use in my office.
  
  The hardware is 200 WS2811 LEDs (50 lights per strand) mounted in
  a hand-built 25x8 array (drilled 200 holes in sheet aluminum).
  
  The board runs a flashy visual sequence of various behaviors and
  also scrolls text (the message board part). The text can be updated
  remotely over the internet. 
  
  A relay is being added to control the lights allowing me to leave
  the unit plugged in all time but with the lights off. The WS2811
  light strands I got are pretty smelly when they warm up and there's
  no real need to have them running when folks aren't in the office to 
  see them.
  
  With the realy, a start-time/stop-time function can be added along 
  with remote on/off capability.
  
  At some point, this will retrieve, via webhooks, common features
  like local weather or other such "live" features.
  
  -------------------------------------------------------------------------
*/

#define PIXEL_COUNT 200

#define NUM_COL 25
#define NUM_ROW 8

// IMPORTANT: Set pixel PIN and TYPE
#define PIXEL_PIN D2
#define PIXEL_TYPE WS2811 // set this to get RGB values even though it is 400KHZ

#define LIGHT_POWER_RELAY_PIN D6

// create a reference to the whole strand, including overflow pixels not used in the matrix
// NOTE: I shouldn't have to need this but I currently do b/c I can't quite sort out some
//       oddities with how the NeoMatrix works. I believe this ends up creating a second
//       pixel buffer which might come in handy or generally cause trouble. :->
Adafruit_NeoPixel wholeStrip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

// The first pixel within is at the top left.  The matrices use zig-zag line ordering.
// There's only one matrix here, so it doesn't matter if we declare it in row
// or column order.  

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(NUM_COL, NUM_ROW, 1, 1, PIXEL_PIN,
  NEO_TILE_TOP   + NEO_TILE_LEFT   + NEO_TILE_ROWS   + NEO_TILE_PROGRESSIVE +
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  PIXEL_TYPE);


// stores message board text
String webText = "Digital Accounting and Royalty Technologies";

// stores weather message text
String webWeather = "Sunny Seattle";

// stores message board on/off
int lightUpTheBoard = 1;

//-------------------------------------------------------------------------------
void setup() {
   // turn off all the pixels in the strip, even the overflow pixels that 
   // aren't used in the matrix. Sometimes they turn on solid white
   // note that I'm not using the NeoMatrix for this part because problems
   wholeStrip.begin();    wholeStrip.show(); 

   Serial.begin(9600);
   matrix.begin();
   matrix.setTextWrap(FALSE);
   matrix.setBrightness(150);
   //matrix.setBrightness(75);
   
   pinMode(LIGHT_POWER_RELAY_PIN, OUTPUT);
  
   Particle.function("passText", handleText);
   Particle.variable("messageText", &webText, STRING);

   Particle.function("passWeather", handleWeather);
   Particle.variable("weatherText", &webWeather, STRING);

   Particle.function("boardOn", handleBoardOn);  // expects 1 for on and 0 for off
   Particle.variable("boardOnSt", &lightUpTheBoard, INT);

} // end setup


//--------------------------------------------------------------------------------
// HANDLER: passText function
int handleText(String actualText) {
    Serial.print("Message received:"); Serial.println(actualText);
    webText = actualText;
    // put back to the cloud for remote read
    Particle.variable("messageText", &webText, STRING);
    return 0;
}
//--------------------------------------------------------------------------------
// HANDLER: passWeather function
int handleWeather(String actualText) {
    Serial.print("Message received:"); Serial.println(actualText);
    webWeather = actualText;
    
    // put back to the cloud for remote read
    Particle.variable("weatherText", &webWeather, STRING);
    return 0;
}
//--------------------------------------------------------------------------------
// HANDLER: boardOn function: Expects 1 for ON and 0 for OFF
int handleBoardOn(String actualText) {
    Serial.print("Message received:"); Serial.println(actualText);
    lightUpTheBoard = actualText.toInt();

    powerLights(lightUpTheBoard);

    // put back to the cloud for remote read
    Particle.variable("boardOnSt", &lightUpTheBoard, INT);
    return 0;
}


//================================================================================
//=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
//================================================================================
void loop() {
   lightsOff(); 
  
   // Color Wipe in pixel order
   colorWipe(wholeStrip.Color(255,0,0), 3, 1);
   colorWipe(wholeStrip.Color(0,255,0), 3, 0);
   colorWipe(wholeStrip.Color(0,0,255), 3, 1);  
   
   delay(250);  lightsOff();
  
   // color wipe rows left to right and top to bottom
   testMatrixWipe(); lightsOff(); delay(250);

   rainbowDART(TRUE, 20, 2); lightsOff(); delay(250);

   // Draw a bunch of rectangles "going down"
   matrix.drawRect(0, 0, NUM_COL, NUM_ROW, matrix.Color(255, 0, 0)); matrix.show(); delay(500); 
   matrix.drawRect(1, 1, NUM_COL-2, NUM_ROW-2, matrix.Color(0, 255, 0)); matrix.show(); delay(500); 
   matrix.drawRect(2, 2, NUM_COL-4, NUM_ROW-4, matrix.Color(0, 0, 255)); matrix.show(); delay(500); 
   matrix.drawRect(3, 3, NUM_COL-6, NUM_ROW-6, matrix.Color(128, 128, 128)); matrix.show(); delay(500); 
   delay(1000);    
  
   // black out the dispaly similarly
   matrix.drawRect(3, 3, NUM_COL-6, NUM_ROW-6, matrix.Color(0, 0, 0)); matrix.show(); delay(500); 
   matrix.drawRect(2, 2, NUM_COL-4, NUM_ROW-4, matrix.Color(0, 0, 0)); matrix.show(); delay(500); 
   matrix.drawRect(1, 1, NUM_COL-2, NUM_ROW-2, matrix.Color(0, 0, 0)); matrix.show(); delay(500); 
   matrix.drawRect(0, 0, NUM_COL, NUM_ROW, matrix.Color(0, 0, 0)); matrix.show(); delay(500); 
   
   scrollText(webText); lightsOff(); delay(250); // we do this twice to get more visibility on the message
   
   // glow the whole display through the rainbow
   rainbow(20, 1); lightsOff(); delay(250);

   pacman();

   // fill the screen with a light color (background)
   matrix.fillRect(0, 0, NUM_COL, NUM_ROW, matrix.Color(30, 30, 30)); matrix.show(); delay(1000); 
   
   // draw a yellow outline
   // drawRect parameters: x,y starting cooridnates, then width and height, then color
   matrix.drawRect(0, 0, NUM_COL, NUM_ROW, matrix.Color(200, 200, 0)); matrix.show(); delay(1000); 
   
   // draw an 3 'X' which each X in a different color one after the other
   // Use the number of rows to force a "square X"
   int xHeight = NUM_ROW - 1;    int xWidth  = NUM_ROW - 1;    int xStart  = 0;
   matrix.drawLine(xStart, 0, xWidth, xHeight, matrix.Color(255, 0, 0)); matrix.drawLine(xHeight, 0, xStart, xWidth, matrix.Color(255, 0, 0));
   matrix.show(); delay(250);
   
   xStart  = xWidth + 1;
   matrix.drawLine(xStart, 0, xStart+xWidth, xHeight, matrix.Color(0, 255, 0)); matrix.drawLine(xStart+xWidth, 0, xStart, xHeight, matrix.Color(0, 255, 0)); 
   matrix.show(); delay(250);

   xStart  = xStart + xWidth + 1;
   matrix.drawLine(xStart, 0, xStart+xWidth, xHeight, matrix.Color(0, 0, 255)); matrix.drawLine(xStart+xWidth, 0, xStart, xHeight, matrix.Color(0, 0, 255)); 
   matrix.show(); delay(250);
   
   delay(1500); 
   
   lightsOff(); delay(250); 
    
   // draw a blue circle 
   // drawCircle parameters: x,y center point, then radius and color
   /*matrix.drawCircle(5, 4, 3, matrix.Color(0, 0, 70)); matrix.show();
   delay(500); 
   matrix.fillScreen(0); delay(500); */
    
   // fill a violet circle 
   /* matrix.fillCircle(5, 4, 4, matrix.Color(70, 0, 70)); matrix.show();
   delay(500); 
   matrix.fillScreen(0); delay(500); */

   // Send a theater pixel chase in...
   theaterChase(wholeStrip.Color(127, 127, 127), 50); // White
   theaterChase(wholeStrip.Color(127, 0, 0), 50); // Red
   theaterChase(wholeStrip.Color(0, 0, 127), 50); // Blue

   theaterChaseRainbow(50);

   rainbowDART(TRUE, 20, 2); lightsOff(); delay(250);     
   
   boxAnimation(10); lightsOff(); delay(250);
   
   scrollText(webText); lightsOff(); delay(250);

   rainbowCycleHole(20, 2); lightsOff(); delay(250);

   delay(100);
} // end loop


/*==========================================================================
  pacman: creates a pacman animation using matrix.drawBitmap animations
  ==========================================================================
*/  
void pacman(){

  // draw yellow dots for pacman to eat
  for(int i=1; i<NUM_COL; i=i+3) {
     matrix.drawPixel(i,3, color(GRAY));
     matrix.show();
     delay(50);
  }
  delay(1000);
  
  for(int i=0; i<NUM_COL; i=i+3) {
     matrix.drawBitmap(i,0, pacmanOpen, 8, 8, color(YELLOW));  matrix.show();
     delay(30);
     matrix.drawBitmap(i,0, pacmanOpen, 8, 8, color(BLACK));   matrix.show();
     delay(20);

     matrix.drawBitmap(i,0, pacmanOpen, 8, 8, color(YELLOW));  matrix.show();
     delay(30);
     matrix.drawBitmap(i,0, pacmanOpen, 8, 8, color(BLACK));   matrix.show();
     delay(20);

     matrix.drawBitmap(i+2,0, pacmanClosed, 8, 8, color(YELLOW)); matrix.show();
     delay(50);
     matrix.drawBitmap(i+2,0, pacmanClosed, 8, 8, color(BLACK));  matrix.show();
     delay(20);
  }
  
} // end pacman


void boxAnimation(int numTimes) {
 
   // try a going through space animation, where the interior most box works its way out to the edge
   for(int i=0; i<numTimes; i++) {
      matrix.drawRect(0, 0, NUM_COL,   NUM_ROW,   color(BLACK));
      matrix.drawRect(3, 3, NUM_COL-6, NUM_ROW-6, color(GRAY)); 
      matrix.show(); delay(100);

      matrix.drawRect(3, 3, NUM_COL-6, NUM_ROW-6, color(BLACK)); 
      matrix.drawRect(2, 2, NUM_COL-4, NUM_ROW-4, color(GRAY));
      matrix.show(); delay(100);

      matrix.drawRect(2, 2, NUM_COL-4, NUM_ROW-4, color(BLACK));
      matrix.drawRect(1, 1, NUM_COL-2, NUM_ROW-2, color(GRAY)); 
      matrix.show(); delay(100);

      matrix.drawRect(1, 1, NUM_COL-2, NUM_ROW-2, color(BLACK)); 
      matrix.drawRect(0, 0, NUM_COL,   NUM_ROW,   color(GRAY));
      matrix.show(); delay(100);
   }

} // end boxAnimation



/*==========================================================================
  rainbowDART: print DART" and cycle through the rainbow 
  - clearScreen: clear the screen before proceeding?
  - wait: how slow to cycle through colors: bigger number, slower cycle
  - numTimes: how many times to cycle through the colors
  ==========================================================================
*/
void rainbowDART(bool clearScreen, int wait, int numTimes) {
   String text = "DART";

   if(clearScreen) { matrix.fillScreen(0); }
   
   matrix.setCursor(1, 0); 
   // color all letters white except for an oragne A
   matrix.setTextColor( matrix.Color(255, 255, 255) ); matrix.print('D'); matrix.show(); delay(500);
   matrix.setTextColor( matrix.Color(251, 79,  20 ) ); matrix.print('A'); matrix.show(); delay(500);
   matrix.setTextColor( matrix.Color(255, 255, 255) ); matrix.print('R'); matrix.show(); delay(500);
   matrix.setTextColor( matrix.Color(255, 255, 255) ); matrix.print('T'); matrix.show(); delay(500);
   
   delay(2000);
   
   for(int j=0; j<(256*numTimes); j++) {
     matrix.setCursor(1, 0);
     matrix.setTextColor( matrixWheel(j&255) );
     matrix.print(text);
     matrix.show();
     delay(wait);
   } // end for

} // end rainbowDART

/*==========================================================================
  Scroll text: print text starting at the right edge, clear the matrix, 
    then print again starting at 1 column to the left, rinse and repeat
  ==========================================================================
*/
void scrollText(String text) {
   
   matrix.setTextColor(matrix.Color(0,255,0));

   int w = matrix.width();  // scroll left: start at right edge
   
   text.concat("   "); // add some spaces to make the scroll better
   
   int len = text.length();
   int iterations = (len+2) * 6; // font is 6 wide x 8 tall

   matrix.fillScreen(0); // clear screen
   matrix.setCursor(0, 0);

   for(int i=0; i<=iterations; i++) {
      matrix.fillScreen(0);
      matrix.setCursor(w, 0);
      matrix.print(text);
      --w; // decrement w to scroll left

      matrix.show();   delay(30);
   } // end for
  
  
} // end scrollText

/*==========================================================================
  rainbow: the pixels glow their way through the colors of the rainbow
  - wait: number of ms to wait on each cycle. larger number slows the cycle
  - numTimes: how many times to cycle through the colors
  ==========================================================================
*/
void rainbow(int wait, int numTimes) {
  int i, j;

  //Serial.println("-rainbow");
  for(j=0; j<(256*numTimes); j++) {
    for(i=0; i<wholeStrip.numPixels(); i++) {
      wholeStrip.setPixelColor(i, Wheel((j) & 255));
    }
    wholeStrip.show();
    delay(wait);
  }
} // end rainbow

/*==========================================================================
  matrixRainbow: the pixels glow their way through the colors of the rainbow
  - wait: number of ms to wait on each cycle. larger number slows the cycle
  - numTimes: how many times to cycle through the colors
  NOTE: using the Matrix interface produces a much choppier color transition
    than than using the simpler NeoPixel interface. The choppiness is 
    noticable even with half of the delay (20ms) of the NeoPixel (40ms)
  ==========================================================================
*/
void matrixRainbow(int wait, int numTimes) {
  int j;

  for(j=0; j<(256*numTimes); j++) {
    matrix.fillRect(0, 0, NUM_COL, NUM_ROW, matrixWheel( (j) & 255) );
    matrix.show();
    delay(wait);
  }
} // end rainbow


/*==========================================================================
  rainbowCycle - makes a rainbow equally distributed throughout the strand
  - wait: number of ms to wait on each cycle. larger number slows the cycle
  - numTimes: how many times to cycle through the colors
  ==========================================================================
*/
void rainbowCycle(int wait, int numTimes) {
  int i, j;

  //Serial.println("-rainbowCycle");
  for(j=0; j<256*numTimes; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< wholeStrip.numPixels(); i++) {
      wholeStrip.setPixelColor(i, Wheel( ( (i * 256 / wholeStrip.numPixels() ) + j) & 255) );
    }
    wholeStrip.show();
    delay(wait);
  }
} // end rainbowCycle

/*==========================================================================
  rainbowCycleHole - walks a rainbow across the field excluding hole
  - wait: number of ms to wait on each cycle. larger number slows the cycle
  - numTimes: how many times to cycle through the colors
  ==========================================================================
*/
void rainbowCycleHole(int wait, int numTimes) {
  int i, j;

  for(j=0; j<256*numTimes; j++) { 
    for(i=0; i< matrix.numPixels(); i++) {
      // set color for everything
      matrix.setPixelColor(i, Wheel( ( (i * 256 / matrix.numPixels() ) + j) & 255) );
    }
    // now black out the center
    matrix.fillRect(2, 2, NUM_COL-4, NUM_ROW-4, matrix.Color(0, 0, 0)); 
    matrix.show();

    delay(wait);
  }
} // end rainbowCycle




/*==========================================================================
  Fill the dots one after the other with a color
  c - color to fill
  wait - how slow to go
  forward - should we fill from front or from the back
  ==========================================================================
*/
void colorWipe(uint32_t c, uint8_t wait, int forward) {
   if(forward == 1) {
      for(uint16_t i=0; i < wholeStrip.numPixels(); i++) {
         wholeStrip.setPixelColor(i, c);
         wholeStrip.show();
         delay(wait);
      }
   } else {
      for(uint16_t i=wholeStrip.numPixels(); i > 0; i--) {
         wholeStrip.setPixelColor(i, c);
         wholeStrip.show();
         delay(wait);
      
      } // end for
   } // end if
} // end colorWipe

//==========================================================================
// Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < wholeStrip.numPixels(); i=i+3) {
        wholeStrip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      wholeStrip.show();

      delay(wait);

      for (int i=0; i < wholeStrip.numPixels(); i=i+3) {
        wholeStrip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    } // end q loop
  } // end j loop
}

//==========================================================================
//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < wholeStrip.numPixels(); i=i+3) {
        wholeStrip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      wholeStrip.show();

      delay(wait);

      for (int i=0; i < wholeStrip.numPixels(); i=i+3) {
        wholeStrip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    } // end q loop
  } // end j loop
} // end theaterChaseRainbow


//===================================================================================
// converts a HEX color into a matrix color. makes color handling easier.
uint16_t color(unsigned long c) {
   byte red = (c & 0xFF0000) >> 16;
   byte green = ( c & 0x00FF00) >> 8;
   byte blue = (c & 0x0000FF);
   return matrix.Color(red, green, blue);
} // end color

//==========================================================================
// Wheel: Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
//==========================================================================
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return wholeStrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return wholeStrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return wholeStrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
} // end Wheel

//==========================================================================
// matrixWheel: Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
//==========================================================================
uint32_t matrixWheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return matrix.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return matrix.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return matrix.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
} // end matrixWheel


/*==========================================================================
   turn all the lights off in both buffers
  ==========================================================================
*/
void lightsOff() {
  for(int i=0; i<PIXEL_COUNT; i++) {
    wholeStrip.setPixelColor(i,wholeStrip.Color(0,0,0));
  }
  wholeStrip.show();
  matrix.fillScreen(0); matrix.show();
} // end lightsOff;

/*==========================================================================
   power the lights up or down by way of a relay wired normally open.
   We set pin LIGHT_POWER_RELAY_PIN to HIGH to power the relay to engerize
     the strand of lights. We have the relay to keep the lights physically
     powered down off hours when not in use.
   Note: This is different than "lightsOff" which just clears the  strand 
     causing it to show "black"
  ==========================================================================
*/
void powerLights(bool powerUp) {
    if(powerUp) 
      { digitalWrite(LIGHT_POWER_RELAY_PIN, HIGH); }
    else 
      { digitalWrite(LIGHT_POWER_RELAY_PIN, LOW); }
    
} // end powerLights


/*===========================================================================
  // figure ou the matrix addressing model
  //Serial.println("left to right, top to bottom wipe");
  // left to right, top to bottom wipe
*/
void testMatrixWipe(){
  int x,y;
  for(y=0; y<NUM_ROW; y++) {
     for(x=0; x<NUM_COL; x++) { 
         matrix.drawPixel(x,y,matrix.Color(0,0,255));   
         matrix.show();
         delay(5);
     } // end for x
  } // end for y
} // end testMatrixWipe
