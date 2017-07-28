#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6
#define SIG 5
Adafruit_NeoPixel strip = Adafruit_NeoPixel(115, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel sig = Adafruit_NeoPixel(2, SIG, NEO_GRB + NEO_KHZ800);

//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 15;

//the amount of milliseconds the sensor has to be low
//before we assume all motion has stopped
unsigned int secsAtHigh = 15;
long unsigned int pause = secsAtHigh * 1000;

boolean lockLowPrint = true;
int timeSinceOn;
int pirPin = 3;    //the digital pin connected to the PIR sensor's output
int ledPin = 9;
int brightness = 0;
bool triggered = false;

void setup() {
  // put your setup code here, to run once:
  strip.begin();
  strip.show();
  sig.begin();
  sig.show();

  Serial.begin(9600);
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(pirPin, LOW);

  //give the sensor some time to calibrate
  Serial.println();
  Serial.print("calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  sig.setPixelColor(1, 255, 0, 0);
  sig.show();
  delay(50);

}

void loop() {
  // put your main code here, to run repeatedly:
  if(triggered == false && digitalRead(pirPin) == LOW){
    sig.setPixelColor(1, 255, 0, 0);
  }

  if (digitalRead(pirPin) == HIGH) {
    if (triggered == false) {
      Serial.println("---");
      Serial.print("motion detected at ");
      Serial.print(millis() / 1000);
      Serial.println(" sec");
      sig.setPixelColor(1, 0, 255, 0);
      sig.show();
      fadeIn(127, 127, 127, 20);
      white(20);
      triggered = true;
    }
    if (lockLowPrint) {
      lockLowPrint = false;
    }
  }
  ////////////////////////////////////////end of HIGH///////////////////////////////////////////

  if (digitalRead(pirPin) == LOW) {
    //if the sensor is low for more than the given pause,
    //we assume that no more motion is going to happen
    if (triggered == true) {
      if (lockLowPrint == false) {
        //makes sure this block of code is only executed again after
        //a new motion sequence has been detected
        Serial.print("motion ended at ");      //output
        Serial.print((millis()) / 1000);
        Serial.println(" sec");
        delay(50);
        lockLowPrint = true;
      }

      for (int i = 1; i <= secsAtHigh; i++) {
        Serial.print(i);
        delay(1000);
        if(i == (secsAtHigh - 1)){
          Serial.println();
          for (int j = 0; j <= secsAtHigh; j++) {
            Serial.print(j);
            if (j % 2 == 0) {
              sig.setPixelColor(0, 255, 0, 0);
            }
            else {
              sig.setPixelColor(0, 0, 0, 0);
            }
            sig.show();
            delay(250);
          }
          sig.setPixelColor(0, 0, 0, 0);
          sig.show();
        }
        if (i == secsAtHigh) {

          Serial.println(" [fading now]");
          sig.setPixelColor(1, 255, 0, 0);
          sig.show();
          fadeOut(127, 127, 127, 20);
          black(20);
          delay(500);
          triggered = false;
          break;
        }
        
        if (digitalRead(pirPin) == HIGH) {
          i = 0;
          Serial.println();
        }
        
      }

    }
    
  }//end of LOW
  
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////FUNCTIONS/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void white(uint8_t wait) {
  uint16_t i;
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 127, 127, 127);
    delay(50);
  }
  strip.show();
  delay(wait);

}

void fadeIn(uint8_t red, uint8_t green, uint8_t blue, uint8_t wait) {
  for (uint8_t b = 0; b < 255; b++) {
    for (uint8_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, red * b / 255, green * b / 255, blue * b / 255);
    }
    strip.show();
    delay(wait);
  }
}

void fadeOut(uint8_t red, uint8_t green, uint8_t blue, uint8_t wait) {
  for (uint8_t b = 255; b > 0; b--) {
    for (uint8_t i = 0; i < strip.numPixels(); i++) { 
      strip.setPixelColor(i, red * b / 255, green * b / 255, blue * b / 255);
      // Serial.print("fading");
    }
    strip.show();
    delay(wait);
  }
}

void black(uint8_t wait) {
  uint16_t i;
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 1, 1, 1);
    delay(50);
  }
  strip.show();
  delay(20);
}

void fadeInAndOut(uint8_t red, uint8_t green, uint8_t blue, uint8_t wait) {

  for (uint8_t b = 0; b < 255; b++) {
    for (uint8_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, red * b / 255, green * b / 255, blue * b / 255);
    }
    strip.show();
    delay(wait);
  }

  for (uint8_t b = 255; b > 0; b--) {
    for (uint8_t i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, red * b / 255, green * b / 255, blue * b / 255);
    }
    strip.show();
    delay(wait);
  }
}
// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
