#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 1
#define LED_PIN 0
#define LED_TIMEOUT 50
#define MIN_BRIGHTNESS 0
#define MAX_BRIGHTNESS 100
#define MIC_PIN 5
#define ANIMATION_TIMEOUT 5000L

#define SAMPLE_WINDOW  10 // on attiny85 8 mhz is not working as expected
#define LOUD_MIN  0
#define LOUD_MAX 800

CRGB leds[NUM_LEDS];
uint8_t brightness = MIN_BRIGHTNESS;
uint32_t fadeDelay = 0;
uint32_t timeout = 0;
boolean fadeup = false;

unsigned int sample;

void peak()
{
  unsigned long startMillis = millis(); // Start of sample window
  unsigned int peakToPeak = 0;          // peak-to-peak level

  unsigned int signalMax = LOUD_MIN;
  unsigned int signalMin = LOUD_MAX;

  // collect data for 50 mS
  while (millis() - startMillis < SAMPLE_WINDOW)
  {
    sample = analogRead(A0);
    if (sample < LOUD_MAX) // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample; // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample; // save just the min levels
      }
    }
  }

  peakToPeak = signalMax - signalMin;           // max - min = peak-peak amplitude
  double volts = (peakToPeak * 5.0) / LOUD_MAX; // convert to volts
}

void updateLed()
{
  if (millis() > fadeDelay + LED_TIMEOUT) {
    if (brightness == MIN_BRIGHTNESS || brightness == MAX_BRIGHTNESS) {
      fadeup = !fadeup;
    }

    brightness += fadeup ? 1 : -1;

    FastLED.setBrightness(brightness);
    // FastLED.show();

    fadeDelay = millis();
  }
}

void setup()
{
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  leds[0] = CRGB::Black;
  pinMode(MIC_PIN, INPUT);
  // randomSeed(analogRead(0));
}

void loop()
{
  uint8_t sensor = digitalRead(MIC_PIN);
  uint32_t now = millis();
  if (sensor && now > timeout)
  {
    leds[0] = CHSV(random8(), 255, 255);
    // timeout = millis() + ANIMATION_TIMEOUT;
    // if (fadeup) {
    //   uint8_t offset = (MAX_BRIGHTNESS - brightness) * LED_TIMEOUT;
    //   timeout = millis() + ((ANIMATION_TIMEOUT - offset) + offset) + (MAX_BRIGHTNESS * LED_TIMEOUT);
    // } else {
      uint8_t offset = (brightness - MIN_BRIGHTNESS) * LED_TIMEOUT;
      timeout = now + ((ANIMATION_TIMEOUT - offset) + offset) + (MAX_BRIGHTNESS * LED_TIMEOUT) + 300;
    // }
  }

  if (now < timeout)
  {
    updateLed();
  }
  // else 
  // {
  //   FastLED.setBrightness(0);
  // }

  FastLED.show();
}