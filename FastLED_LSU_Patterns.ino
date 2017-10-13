//FastLED-LSU-Patterns - LSU-colored patterns for NeoPixel-style (3-pin) LED strips

#include <FastLED.h> //FastLED Library required
#define NUM_LEDS 300 //number of LEDs
#define DATA_PIN 6 //Arduino pin for data line
#define BRIGHTNESS 96 //brightness
#define SECONDS_PER_PATTERN 20 //number of seconds before changing pattern
CRGB leds[NUM_LEDS]; //create LED array
CRGB ledPurple = CRGB::Indigo; //assign purple LED color
CRGB ledGold = CRGB::Yellow; //assign gold LED color
CRGBPalette256 crossfadePalette( ledGold, ledPurple, ledGold, ledPurple); //palette for crossfade
const long interval = 500; //interval for alternating colors
unsigned long previousMillis = 0; //milliseconds since last call
boolean purple = false; //false = gold, currently used in chase()
int colorChoice = 0; //switch between purple and gold in wave()


void setup() {
  // put your setup code here, to run once:
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip); 
  fill_solid( leds, NUM_LEDS, CRGB::Black );
  // set master brightness
  FastLED.setBrightness(BRIGHTNESS);
}

//Modified FastLED cycling structure. Adapted from https://gist.github.com/kriegsman/536dd5f2238413cf0ee2
typedef void (*ArgumentPattern)(CRGB arg);
typedef struct { 
  ArgumentPattern mPattern;
  CRGB mArg;
} ArgumentPatternWithArgumentValues;

ArgumentPatternWithArgumentValues gPatternsAndArguments[] = {
  {wave},
  
  {confetti},
  
  {alternating},

  {solid, ledGold},
  
  {chase},
  
  {crossfade},
  
  {solid, ledPurple}
};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

void loop() {
  // Call the current pattern function once, updating the 'leds' array
  CRGB arg = gPatternsAndArguments[ gCurrentPatternNumber ].mArg;
  ArgumentPattern pat = gPatternsAndArguments[ gCurrentPatternNumber ].mPattern;

  pat(arg);

  EVERY_N_SECONDS( SECONDS_PER_PATTERN ) { nextPattern(); } // change patterns periodically
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  const int numberOfPatterns = sizeof(gPatternsAndArguments) / sizeof( gPatternsAndArguments[0]);  
  gCurrentPatternNumber = (gCurrentPatternNumber+1) % numberOfPatterns;
}

void alternating() {
  //alternate purple and gold lights

  unsigned long currentMillis = millis(); //current milliseconds

  CRGB evenLeds; //variable to set color of even LEDs
  CRGB oddLeds; //variable to set color of odd LEDs

  //test if time to alternate lights, if so switch colors
  if (currentMillis - previousMillis >= interval) { 

    previousMillis = currentMillis; //retain milliseconds to test for next time to switch
    
    //test first LED for color
    if(leds[0] == ledPurple) { //if first LED is purple, switch even LEDs to gold and odd LEDs to purple
      evenLeds = ledGold;
      oddLeds = ledPurple;
    }
    else{ //first run or if first LED is gold, make even LEDs purple and odd LEDs gold
      evenLeds = ledPurple;
      oddLeds = ledGold;
    }

    for(int dot = 0; dot < NUM_LEDS; dot++) {
      if((dot % 2) == 0) {    //modulus of 0 indicates even LEDs
        leds[dot] = evenLeds; //set color designated for even LEDs
      }
      else {
        leds[dot] = oddLeds;  //set color for odd LEDs
      }
    }
  }
  FastLED.show();
}

void chase() {
  //send color down the entire LED string animated, then the opposite color
  
  for(int dot = 0; dot < NUM_LEDS; dot++) { 
    if(!purple){   //LEDs are currently gold and should be changed to purple
      leds[dot] = ledPurple;
    }
    else {        //LEDs are currently purple and should be changed to gold or first run
      leds[dot] = ledGold;
    }
    FastLED.show();
    FastLED.delay(30);
  }
  if(purple){
    purple = false; //gold next run
  }
  else{
    purple = true; //purple next run
  }
}

void crossfade() {
  //fade from purple to gold and back to purple across entire strip
  int paletteSteps = 255;
  for(int fadeIndex = 0; fadeIndex < paletteSteps; fadeIndex++) { //step through all iterations of color palette
    fill_solid( leds, NUM_LEDS, crossfadePalette[fadeIndex] ); //set all LEDs to current step in palette
    FastLED.show();
    FastLED.delay(100);
  }
}

void solid( CRGB color ) {
  fill_solid( leds, NUM_LEDS, color ); //set LED color
  int fadeAmount = -1;
  int localbrightness = BRIGHTNESS;
  FastLED.setBrightness(localbrightness); //set brightness to 0
  FastLED.show();
  for(int fadeIndex = 0; fadeIndex < (BRIGHTNESS * 2); fadeIndex++){
    localbrightness = localbrightness + fadeAmount;
    FastLED.setBrightness(localbrightness);
    FastLED.show();
    if(localbrightness == 0 || localbrightness == BRIGHTNESS){
      fadeAmount = -fadeAmount;
    }
    FastLED.delay(20);
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly, based on DemoReel100
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += ledPurple;
  pos = random16(NUM_LEDS);
  leds[pos] += ledGold;
  FastLED.show();
  FastLED.delay(10);
}

void wave() {
  CRGB color[2] = {ledPurple, ledGold};
  fadeToBlackBy( leds, NUM_LEDS, 20);
  uint16_t pos = beatsin16(9,0,NUM_LEDS-1);
  leds[pos] += color[colorChoice];
  if(pos == (NUM_LEDS-1)) { 
    if(colorChoice == 0) {
      colorChoice = 1;
    }
    else{
      colorChoice = 0;
    }
  }
  FastLED.show();
}
