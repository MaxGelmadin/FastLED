#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
extern CRGB g_Leds[];

class Marquee : public AbstractDraw
{
    public:
        Marquee(int a) {}

        virtual void Draw() 
        {
            static bool forward = true;
            static int counter = 0;
            counter++;
            counter %= NUM_LEDS * 2;
            forward = true ? counter < NUM_LEDS : false;
            static int scroll = 0;
            static byte k = HUE_RED;

            CRGB c;
            if (forward) {
                scroll++;
                for (int i = 0; i < NUM_LEDS; i++)
                    g_Leds[i] = c.setHue(k += 2);

                for (int i = scroll % 5; i < NUM_LEDS; i += 5)
                    g_Leds[i] = CRGB::Black;
            } else {
                scroll--;
                for (int i = NUM_LEDS - 1; i >= 0; i--)
                    g_Leds[i] = c.setHue(k += 2);

                for (int i = scroll % 5; i < NUM_LEDS; i += 5)
                    g_Leds[i] = CRGB::Black;
            }
            FastLED.show();
            delay(50);
        }
};


