#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

extern CRGB g_Leds[];

// Gradient palette "Sunset_Real_gp", originally from
// http://soliton.vm.bytemark.co.uk/pub/cpt-city/nd/atmospheric/tn/Sunset_Real.png.index.html
// converted for FastLED with gammas (2.6, 2.2, 2.5)
// Size: 28 bytes of program space.

DEFINE_GRADIENT_PALETTE( Sunset_Real_gp ) {
    0, 120,  0,  0,
   22, 179, 22,  0,
   51, 255,104,  0,
   85, 167, 22, 18,
  135, 100,  0,103,
  198,  16,  0,130,
  255,   0,  0,160};


class Palette : public AbstractDraw
{
    private:
        CRGBPalette16 sunset;
        uint8_t colorIndex[NUM_LEDS];
        uint8_t paletteIndex = 0;

    public:
        Palette(bool a): sunset(Sunset_Real_gp)
        {
            // for (int i = 0; i < NUM_LEDS; i++)
            //     colorIndex[i] = random8();
        }


        virtual void Draw()
        {
            // for (int i = 0; i < NUM_LEDS; i++)
            //     g_Leds[i] = ColorFromPalette(sunset, colorIndex[i]);
            fill_palette(g_Leds, NUM_LEDS, paletteIndex, 255 / NUM_LEDS, sunset, 75, LINEARBLEND);

            EVERY_N_MILLISECONDS(14)
            {
                paletteIndex++;
            }

            FastLED.show();
        }
};


