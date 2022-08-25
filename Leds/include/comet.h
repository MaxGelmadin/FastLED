#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

extern CRGB g_Leds[];


class Comet : public AbstractDraw 
{
    private:
        byte _comet_effect;
        void DrawComet()
        {
            const int cometSize = 5;                            // What is the comet size?
            const int deltaHue = 4;
            const byte fadeAmount = 128;

            static byte hue = HUE_BLUE;
            static int pos = 0;
            static int iDirection = 1;
        
            pos += iDirection;
            hue += deltaHue;

            if (pos == 0 || pos == (NUM_LEDS - cometSize))      // Are we moving forward or backwards?
                iDirection *= -1;

            for (int i = 0; i < cometSize; i++)
                g_Leds[i + pos].setHue(hue);

            for (int i = 0; i < NUM_LEDS; i++) 
            {
                if (random(4) == 1)
                    g_Leds[i] = g_Leds[i].fadeToBlackBy(fadeAmount);
            }
            delay(20);
            FastLED.show();
        }

        void ClashingComets() 
        {
            const int cometSize = 5;                            // What is the size of the comets?
            const byte deltaHue = 2;
            const byte fadeAmount = 128;

            static int i_pos = 0;                               // Starts at index 0
            static int j_pos = NUM_LEDS - 1;                    // Starts at index NUM_LEDS - 1
            static int direction = 1;
            static int i_hue = random(256);
            static int j_hue = random(256);

            i_pos += direction;
            j_pos -= direction;

            if (i_pos == 0 || i_pos == (NUM_LEDS - 1)) 
                direction *= -1;

            for (int i = 0; i < cometSize; i++)
            {
                g_Leds[i_pos] += g_Leds[i_pos].setHue(i_hue += deltaHue);
                g_Leds[j_pos] += g_Leds[j_pos].setHue(j_hue += deltaHue);
            }

            for (int i = 0; i < NUM_LEDS; i++) 
            {
                if (random(3) == 1)
                    g_Leds[i] = g_Leds[i].fadeToBlackBy(fadeAmount);
            } 

            delay(20);
            FastLED.show();
        }

    public:
        Comet(byte comet_effect): _comet_effect(comet_effect) {}

        virtual void Draw()
        {
            switch(_comet_effect)
            {
                case 0:
                    DrawComet();
                    break;
                case 1:
                    ClashingComets();
                    break;
            }
        }
};



// void MoreComets() 
// {
//     const int cometSize = 5;        // What is the size of the comets?
//     const byte deltaHue = 2;
//     const byte fadeAmount = 128;
//     const byte numOfComets = 1;

//     static int directions[numOfComets];
//     static int hues[numOfComets];
//     static int poss[numOfComets];

//     for (int i = 0; i < numOfComets; i++)
//     {
//         directions[i] = random(2) == 1 ? 1 : -1;
//         hues[i] = random(256);
//         poss[i] = random(NUM_LEDS);
//     }

//     for (int i = 0; i < numOfComets; i++)
//     {
//         poss[i] += directions[i];
//         if (poss[i] == 0 || poss[i] == NUM_LEDS - 1)
//             directions[i] *= -1;
//     }

//     for (int i = 0; i < cometSize; i++)
//     {
//         for (int j = 0; j < numOfComets; j++)
//         {
//             leds[poss[j]].setHue(hues[j] += deltaHue);
//         }
//     }

//     for (int i = 0; i < NUM_LEDS; i++) 
//     {
//         if (random(3) == 1)
//             leds[i] = leds[i].fadeToBlackBy(fadeAmount);
//     } 

//     delay(30);
//     FastLED.show();
// }
