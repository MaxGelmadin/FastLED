#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "Marquee.h"
#include "stars.h"


#define LED_PIN     5
#define NUM_LEDS    120 // 150 LEDs in the full strip
#define BRIGHTNESS  16
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

typedef struct {
    char const *name;
    void (*f) ();
} effects;

CRGB leds[NUM_LEDS] = {0};
effects fs[] = { 
        { "0 - Marquee", DrawMarquee },
        { "1 - Stars", DrawStars } 
    };


void setup() {
    pinMode(LED_PIN, OUTPUT);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}


void loop() {
    fs[0].f();
}