#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include "FastLED.h"
#include <avr/power.h>
 
// Fixed definitions
#define DATA_PIN 6                                        
#define CLOCK_PIN 11                                            
#define COLOR_ORDER RGB                                      
#define LED_TYPE NEOPIXEL                                      
#define NUM_LEDS  30  

CRGB leds[NUM_LEDS];
 
// our RGB -> eye-recognized gamma color
byte gammatable[256];
 
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
 
void setup() {
  
  Serial.begin(9600);
  Serial.println("Color View Test!");
  
  // To make it work w/ an 8 Mhz proc (example : Lilypad Arduino)
  clock_prescale_set(clock_div_2);
  
  // Use this for WS2801 Leds
  //  FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  // Use this for NEOPIXEL
  FastLED.addLeds<LED_TYPE, DATA_PIN>(leds, NUM_LEDS); 
  
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }
  
  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
      
    gammatable[i] = x;      
    //Serial.println(gammatable[i]);
  }
  
  for (int i=0; i<3; i++){ //this sequence flashes the first three pixel as a countdown to the color reading.
    leds[i].setRGB(188, 188, 188); //white, but dimmer-- 255 for all three values makes it blinding!
    FastLED.show();
    delay(1000);
    leds[i].setRGB(0, 0, 0);
    FastLED.show();
    delay(500);
  }
  
  uint16_t clear, red, green, blue;
 
  tcs.setInterrupt(false);      // turn on LED
 
  delay(60);  // takes 50ms to read 
  
  tcs.getRawData(&red, &green, &blue, &clear);
 
  tcs.setInterrupt(true);  // turn off LED
  
  Serial.print("C:\t"); Serial.print(clear);
  Serial.print("\tR:\t"); Serial.print(red);
  Serial.print("\tG:\t"); Serial.print(green);
  Serial.print("\tB:\t"); Serial.print(blue);
 
  // Figure out some basic hex code for visualization
  uint32_t sum = red;
  sum += green;
  sum += blue;
  sum += clear;
  float r, g, b;
  r = red; r /= sum;
  g = green; g /= sum;
  b = blue; b /= sum;
  r *= 256; g *= 256; b *= 256;
  Serial.print("\t");
  Serial.print((int)r, HEX); Serial.print((int)g, HEX); Serial.print((int)b, HEX);
  Serial.println();
 
  Serial.print((int)r ); Serial.print(" "); Serial.print((int)g);Serial.print(" ");  Serial.println((int)b );
  
  for(int i = 0; i < NUM_LEDS; i++) {
      leds[i].setRGB(gammatable[(int)r], gammatable[(int)g], gammatable[(int)b]);
      FastLED.show();
      delay(1000);
  }
  
}
 
void loop() {
  
  //loop is empty because it only takes the color reading once on power up! Turn the project off and on again to change the color.
    
}
