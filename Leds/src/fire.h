#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "ledgfx.h"
#define NUM_LEDS 120



class ClassicFire : public AbstractDraw
{
    private:

        size_t _size;               // What is the size of the led strip?
        size_t _cooling;            // How fast the fire is going to cool?
        size_t _sparks;             // How many sparks will spark the fire?
        size_t _sparks_height;      // How high you want the sparks to reach?
        size_t _sparking;           // What are the chances you want it to spark?
        bool _mirrored;             // Do you want this to be mirrored?
        bool _reversed;             // Do you want this to be reversed?

        byte *heat;                 // Array of heat values. 0 means cool, 255 means hot.

        static const size_t _blend_self = 2;
        static const size_t _blend_neighbor1 = 3;
        static const size_t _blend_neighbor2 = 2;
        static const size_t _blend_neighbor3 = 1;

        static const size_t _total_blend = (_blend_self + _blend_neighbor1 + _blend_neighbor2 + _blend_neighbor3);



    public:

        ClassicFire(size_t size, size_t cooling = 50, size_t sparks = 4, size_t sparks_height = 4, size_t sparking = 100 , bool mirrored = false, bool reversed = true)
            :   _size(size),
                _cooling(cooling),
                _sparks(sparks),
                _sparks_height(sparks_height),
                _sparking(sparking),
                _mirrored(mirrored),
                _reversed(reversed)

            {
                if (_mirrored)
                {
                    _size /= 2;
                }
                heat = new byte[_size] { 0 };
            }

        virtual ~ClassicFire()
        {
            delete[] heat;
        }

        virtual void Draw()
        {
            FastLED.clear();
            // Cool each led a little bit
            for (int i = 0; i < _size; i++)
            {
                heat[i] = max(0L, heat[i] - random(0, ((_cooling * 10) / _size) + 2));
            }

            // Diffuse the fire upwards.
            // The current modified led gets the average of the 3 leds above him, where the furthest neighbor gets the smallest weight. 
            for (int i = 0; i < _size; i++)
            {
                heat[i] = ((heat[i] * _blend_self)
                        + (heat[(i + 1) % _size] * _blend_neighbor1)
                        + (heat[(i + 2) % _size] * _blend_neighbor2)
                        + (heat[(i + 3) % _size] * _blend_neighbor3))
                        / _total_blend;
            }


            // Randomly ignite the fire
            for (int i = 0; i < _sparks; i++)
            {
                if (random(255) < _sparking)
                {
                    int pos = _size - 1 - random(_sparks_height);
                    heat[pos] += random(160, 255);
                }
            }

            // Convert fire to color
            for (int i = 0; i < _size; i++)
            {
                CRGB color_i = HeatColor(heat[i]);
                int pos = _reversed ? _size - 1 - i : i;
                DrawPixels(pos, 1, color_i);
                if (_mirrored)
                {
                    int pos_2 = !_reversed ? (2 * _size - 1 - i) : (_size + i);
                    DrawPixels(pos_2, 1, color_i);
                } 
            }
            FastLED.show();
            delay(33);
        }
};