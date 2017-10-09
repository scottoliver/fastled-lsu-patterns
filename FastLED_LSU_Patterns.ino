//FastLED-LSU-Patterns - LSU-colored patterns for NeoPixel-style (3-pin) LED strips

#include <FastLED.h> //FastLED Library required
#define NUM_LEDS 60 //number of LEDs
#define DATA_PIN 6 //Arduino pin for data line
#define BRIGHTNESS 96 //brightness
#define FRAMES_PER_SECOND 120 //frames per second
#define SECONDS_PER_PATTERN 10 //number of seconds before changing pattern
CRGB leds[NUM_LEDS]; //create LED array
CRGB ledPurple = CRGB::Indigo; //assign purple LED color
CRGB ledGold = CRGB::Yellow; //assign gold LED color

const long interval = 500; //interval for alternating colors

unsigned long previousMillis = 0; //milliseconds since last call

boolean purple = false; //purple or gold


void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); 
  // set master brightness
  FastLED.setBrightness(BRIGHTNESS);
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { alternating, chase };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

void loop() {
  // Call function according to the current index of the pattern array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  //FastLED.delay(1000/FRAMES_PER_SECOND); 

  EVERY_N_SECONDS( SECONDS_PER_PATTERN ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void alternating() {
  //alternate purple and gold lights

  unsigned long currentMillis = millis(); //current milliseconds

  CRGB evenLeds;
  CRGB oddLeds;

  //test if time to alternate lights, if so switch colors
  if (currentMillis - previousMillis >= interval) { 

    previousMillis = currentMillis;

    if(leds[0] == ledPurple) {
      evenLeds = ledGold;
      oddLeds = ledPurple;
    }
    else{
      evenLeds = ledPurple;
      oddLeds = ledGold;
    }

    for(int dot = 0; dot < NUM_LEDS; dot++) {
      if((dot % 2) == 0) { 
        leds[dot] = evenLeds;
      }
      else {
        leds[dot] = oddLeds;
      }
    }
  }
}

void chase() {
  //send color down the entire LED string animated, then the opposite color
  
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    if(purple){ //LEDs are currently purple
      leds[dot] = ledPurple;
    }
    else { //LEDs are currently gold or first run
      leds[dot] = ledGold;
    }
    FastLED.show();
    FastLED.delay(30);
  }
  if(purple){
      purple = false;
    }
    else{
      purple = true;
  }
}
