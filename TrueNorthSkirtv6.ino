#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Adafruit_NeoPixel.h>
#include "FastLED.h"
/*When turning on the device, ensure the sensor is facing North for correct calibration on startup */
/*True North Skirt V1 - Sophie Amin March 19 2017
  Program calibrations orientation sensor in the North direction and continuously grabs sensor data to light up the skirt in the North facing direction*/
/*colour library*/
#define BLUE    0, 0, 255
#define TEAL    0,255,255
#define PINK    255,51,153
#define DATA_PIN 22
#define OFF     0, 0, 0
#define NUM_LEDS 630

CRGB leds[NUM_LEDS];

// DRESS VARIABLES //
const int numStrips = 35; // The total # of strips on the dress
int strips [numStrips][2] = { // These ranges need to be updated whenever new strips/LEDs are added
{0,21}, //1 
{22,47}, //2
{48,67}, //3
{68,80}, //4
{81,94}, //5
{95,110}, //6
{111,125}, //7
{126,147}, //8
{148,173}, //9
{174,193}, //10
{194,206}, //11
{207,220}, //12
{221,236}, //13
{237,251}, //14
{252,273}, //15
{274,299}, //16
{300,319}, //17
{320,332}, //18
{333,346}, //19
{347,362}, //20
{363,377}, //21
{378,399}, //22
{400,425}, //23
{426,445}, //24
{446,458}, //25
{459,472}, //26
{473,488}, //27
{489,503}, //28
{504,525}, //29
{526,551}, //30
{552,571}, //31
{572,584}, //32
{585,598}, //33
{599,614}, //34
{615,629} //35
};

// OTHER DECLARATIONS //
float stripSize = 360.0/((float)numStrips);
float xPos = 0; //x position of the dress
int middleStrip, leftStrip, rightStrip;
int oldLeftStrip = -100;
int oldMiddleStrip = -100;
int oldRightStrip = -100;
float currDegrees; 
boolean first = false;



// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//Adafruit_NeoPixel strip = Adafruit_NeoPixel(630, PIN, NEO_GRB + NEO_KHZ800);
/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)



Adafruit_BNO055 bno = Adafruit_BNO055();

void setup() {
  delay(2000);
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);  
  //strip.begin();
  //strip.show(); // Initialize all pixels to 'off'  
  
// The following code lights up all the strips 1 at a time //
//  for (int j = 0; j <= (numStrips-1); j++){
//    for (int i = strips[j][0]; i <= strips[j][1]; i++) {
//      strip.setPixelColor(i, strip.Color(BLUE));
//    }
//    strip.show();
//    delay(BNO055_SAMPLERATE_DELAY_MS * 2);
//    for (int i = strips[j][0]; i <= strips[j][1]; i++) {
//      strip.setPixelColor(i, strip.Color(OFF));
//    }     
//    strip.show();
//    delay(BNO055_SAMPLERATE_DELAY_MS * 2);    
//  }
//  strip.show();
//  delay(BNO055_SAMPLERATE_DELAY_MS * 5);
//
//  // Turn off all the strips //
//  for (int j = 0; j <= (numStrips-1); j++){
//    for (int i = strips[j][0]; i <= strips[j][1]; i++) {
//      strip.setPixelColor(i, strip.Color(OFF));
//    }
//  }  
//  strip.show();  
  
  Serial.begin(9600);
  Serial.println("Orientation Sensor Raw Data Test"); Serial.println("");
   
  
    /* Initialise the sensor */
    if(!bno.begin())
    {
      /* There was a problem detecting the BNO055 ... check your connections */
      Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
      while(1);
    }
  
    delay(1000);
  
    /* Display the current temperature */
    int8_t temp = bno.getTemp();
    Serial.print("Current Temperature: ");
    Serial.print(temp);
    Serial.println(" C");
    Serial.println("");
  
    bno.setExtCrystalUse(true);
  
    Serial.println("Calibration status values: 0=uncalibrated, 3=fully calibrated");
}

void loop() {
  if (first){
    leftStrip = (numStrips-1);
    middleStrip = 0;
    rightStrip = 1;
  
    for (int i = strips[leftStrip][0]; i <= strips[leftStrip][1]; i++){
       leds[i] = CRGB::Blue;
    }  
        
     for (int i = strips[middleStrip][0]; i <= strips[rightStrip][1]; i++){
       leds[i] = CRGB::Blue;
     }  
    FastLED.show();
    first = false;
    oldLeftStrip = leftStrip;
    oldMiddleStrip = middleStrip;
    oldRightStrip = rightStrip;
  }
else{
    imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);
    xPos = euler.x();
  
    currDegrees = 0; 
    // Loop for finding the correct strips to light up //
    for (int j = 0; j <= (numStrips-1); j++){ // Loop over every strip
      if ((xPos >= currDegrees) && (xPos < (currDegrees + stripSize))){ // If the current x position is inside of that strips "degree range" then we assign the strips to be lit up
      //  Serial.print(j);
        if (j == 0){ // Special case for the 0th strip; we need to light up the last strip, the 0th strip and the 1st strip
          leftStrip = (numStrips-1);
          middleStrip = 0;
          rightStrip = 1;
        }
        else if(j == (numStrips-1)){ // Special case for the last strip; we need to light up the last strip, the one before the last strip and the 0th strip
          leftStrip = j-1;
          middleStrip = j;
          rightStrip = 0;
        }
        else{ // Otherwise we light up the current strip, the one before it and the one after
          middleStrip = j;
          leftStrip = j-1;
          rightStrip = j+1;
        } 
      //  Serial.println("left start to right end");
     //   Serial.println(strips[leftStrip][0]);
      //  Serial.println(strips[rightStrip][1]);
        break; // Once we've found the strips we don't need to continue w/ the for loop
      }
    currDegrees += stripSize; // Increment currDegrees for each iteration of the for loop
    }
  
  
    if (((oldLeftStrip == leftStrip) && (oldMiddleStrip == middleStrip)) && (oldRightStrip == rightStrip)){
      //currDegrees = 0;
    } 
    else if ((middleStrip == oldMiddleStrip + 1) || ((middleStrip == 0) && (oldMiddleStrip == (numStrips-1)))){ // Left turn
    //  Serial.println("left!!!");

      for (int j = 0; j <= (numStrips-1); j++){
        for (int i = strips[j][0]; i <= strips[j][1]; i++) {
          leds[i] = CRGB::Black;
        }
      }     

      turnOn(leftStrip);
      turnOn(middleStrip);
      //turnOn(rightStrip);
      //turnOff(oldLeftStrip);
      fadeInOut(rightStrip, oldLeftStrip);            
      
      
    }
    else if ((middleStrip == oldMiddleStrip - 1) || ((middleStrip == (numStrips-1)) && (oldMiddleStrip == 0))){ // Right turn
     // Serial.println("right!!!");
      //fadeOut(oldRightStrip);
      //fadeIn(leftStrip);

      for (int j = 0; j <= (numStrips-1); j++){
        for (int i = strips[j][0]; i <= strips[j][1]; i++) {
          leds[i] = CRGB::Black;
        }
      }               

      turnOn(middleStrip);
      turnOn(rightStrip);
      //turnOn(leftStrip);
      //turnOff(oldRightStrip);
      fadeInOut(leftStrip, oldRightStrip);
//      fadeIn(leftStrip);      
//      fadeOut(oldRightStrip);        
    }
    else{
       // Light up the strips //
      if (middleStrip == 0){
      
//        for (int i = strips[leftStrip][0]; i <= strips[leftStrip][1]; i++){
//          leds[i] = CRGB::Blue;
//        }  
      turnOn(leftStrip);
      turnOn(middleStrip);
      turnOn(rightStrip);     
//        for (int i = strips[middleStrip][0]; i <= strips[rightStrip][1]; i++){
//          leds[i] = CRGB::Blue;
//        }
      //  Serial.println("middleStrip is 0");
      
      }
      else if (middleStrip == (numStrips-1)){
//        for (int i = strips[leftStrip][0]; i <= strips[middleStrip][1]; i++){
//          leds[i] = CRGB::Blue;
//        }  
//      
//        for (int i = strips[rightStrip][0]; i <= strips[rightStrip][1]; i++){
//          leds[i] = CRGB::Blue;
//        }
      //  Serial.println("middleStrip is last strip"); 
      turnOn(leftStrip);
      turnOn(middleStrip);
      turnOn(rightStrip);          
      }
      else{
//        for (int i = strips[leftStrip][0]; i <= strips[rightStrip][1]; i++) {
//          leds[i] = CRGB::Blue;
//        }
      turnOn(leftStrip);
      turnOn(middleStrip);
      turnOn(rightStrip);           
       // Serial.println("middleStrip is regular");
      }  
    }
    FastLED.show();  
  
  
    oldLeftStrip = leftStrip;
    oldMiddleStrip = middleStrip;
    oldRightStrip = rightStrip;
  
 //   Serial.print("X: ");
 //   Serial.print(xPos);
  //  Serial.print("\n");
  }
}

void fadeInOut(int in, int out){
  for( int colorStep=0; colorStep<256; colorStep+=32 ) {

      int r = colorStep;  // Redness starts at zero and goes up to full
      int r2 = 255-colorStep;
      int b = colorStep;  // Blue starts at full and goes down to zero
      int b2 = 255-colorStep;
      int g = colorStep;              // No green needed to go from blue to red      
      int g2 = 255-colorStep;

      // COLOUR CHANGE HERE //
//      int r = 0;  // Redness starts at zero and goes up to full
//      int r2 = 0;
//      int b = colorStep;  // Blue starts at full and goes down to zero
//      int b2 = 255-colorStep;
//      int g = 0;              // No green needed to go from blue to red      
//      int g2 = 0;      


      // Now loop though each of the LEDs and set each one to the current color

      for(int x = strips[in][0]; x <= strips[in][1]; x++){
          leds[x] = CRGB(r,g,b);
      }

      for(int x = strips[out][0]; x < strips[out][1]; x++){
          leds[x] = CRGB(r2,g2,b2);
      }


      // Display the colors we just set on the actual LEDs
      FastLED.show();
  }  
  for(int x = strips[out][0]; x < strips[out][1]; x++){
    leds[x] = CRGB::Black;
  }  
  //FastLED.show();  
}
 

void turnOn(int s){
  for (int i = strips[s][0]; i <= strips[s][1]; i++) {
    leds[i] = CRGB::White;
    // COLOUR CHANGE HERE //
    //leds[i] = CRGB::Blue;
  } 
  FastLED.show();
}

void turnOff(int s){
  for (int i = strips[s][0]; i <= strips[s][1]; i++) {
    leds[i] = CRGB::Black;
  } 
  FastLED.show();
}

 


  
//  for (int i = strips[s][0]; i <= strips[s][1]; i++) {
//    leds[i] = CRGB::Blue;
//  }  
//  for (int i = strips[s][0]; i <= strips[s][1]; i++) {
//    leds[i] %= 50;
//  } 
//  FastLED.show();      
//  delay(10);  
//  for (int i = strips[s][0]; i <= strips[s][1]; i++) {
//    leds[i] %= 200;
//  }   
//  FastLED.show();    
//}










//fuction to turn on one of 12 sections
//pass the colour of the lighting, the starting position and the ending position

