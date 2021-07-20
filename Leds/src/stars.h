#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#define NUM_LEDS 120
extern CRGB leds[];



class Stars : public AbstractDraw
{
    public:
        Stars(int a) {}
        // Each time we pick randomly quarter of the leds, light them up with a small delay
        // between each one of them, then start all over again.     
        void Draw()
        {
            fill_solid(leds, NUM_LEDS, CRGB::Black);
            bool turnedOn[NUM_LEDS] = { false };
            CRGB c;
            byte i = 0;
            while (i < NUM_LEDS / 4) {
                int pickedLed = rand() % NUM_LEDS;
                if (!turnedOn[pickedLed]) {
                    turnedOn[pickedLed] = true;
                    leds[pickedLed] = c.setHue(rand() % 256);
                    i++;
                    FastLED.show();
                    delay(200);
                }
            }
            delay(25);
        }



};


