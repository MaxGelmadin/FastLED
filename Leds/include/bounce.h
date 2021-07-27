#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <vector>
#include <sys/time.h>
#define NUM_LEDS 120

extern CRGB g_Leds[];

using namespace std;
#define ARRAY_SIZE(x) (sizeof(x) / (sizeof(x[0])))

static const CRGB colors[] =
{
    CRGB::Green,
    CRGB::Red,
    CRGB::Blue,
    CRGB::Orange,
    CRGB::Indigo
};

class BouncingBalls : public AbstractDraw
{
    private:
        
        double InitialBallSpeed(double height)
        {
            return sqrt(-2 * Gravity * height);
        }
        size_t _length;                     // Strip length
        size_t _balls;                      // Number of balls will be simultaneously drawn on the strip
        byte _fadeRate;
        bool _mirrored;

        const double Gravity = -9.81;
        const int StartingHeight = 1;
        const double SpeedKnob = 4.0;
        const double ImpactVelocity = InitialBallSpeed(StartingHeight);

        vector<double> ClockTimeAtLastBounce, Height, BallSpeed, Dampening;
        vector<CRGB> ballColors;

        static double Time()
        {
            timeval tv = { 0 };
            gettimeofday(&tv, nullptr);
            return (double)(tv.tv_usec / 1000000.0 + (double) tv.tv_sec);
        }


    public:

        BouncingBalls(size_t length, size_t balls = 3, byte fadeRate = 0, bool mirrored = false)
            :   _length(length),
                _balls(balls),
                _fadeRate(fadeRate),
                _mirrored(mirrored),
                ClockTimeAtLastBounce(balls),
                Height(balls),
                BallSpeed(balls),
                Dampening(balls),
                ballColors(balls)

            {
                // Constructor
                for (int i = 0; i < _balls; i++)
                {
                    ClockTimeAtLastBounce[i] = Time();
                    Height[i] = StartingHeight;
                    BallSpeed[i] = InitialBallSpeed(Height[i]);
                    Dampening[i] = 0.90 - (i / pow(balls, 2));
                    ballColors[i] = colors[i % ARRAY_SIZE(colors)];
                }
            }

                virtual void Draw()
                {
                    if (_fadeRate != 0)
                        for (int i = 0; i < _length; i++)
                            g_Leds[i] = g_Leds[i].fadeToBlackBy(_fadeRate);
                    else
                        FastLED.clear();

                    for (int i = 0; i < _balls; i++)
                    {
                        double TimeSinceLastBounce = (Time() - ClockTimeAtLastBounce[i]) / SpeedKnob;

                        Height[i] = 0.5 * Gravity * pow(TimeSinceLastBounce, 2.0) + BallSpeed[i] * TimeSinceLastBounce;


                        if (Height[i] < 0)
                        {
                            Height[i] = 0;
                            ClockTimeAtLastBounce[i] = Time();
                            BallSpeed[i] *= Dampening[i];

                            if (BallSpeed[i] < 0.01)
                                BallSpeed[i] = InitialBallSpeed(StartingHeight) * Dampening[i];
                            
                        }

                        size_t position = (size_t) (Height[i] * (_length - 1) / StartingHeight);

                        g_Leds[position] += ballColors[i];
                        g_Leds[position - 1] += ballColors[i];

                        if (_mirrored)
                        {
                            g_Leds[_length - position - 1] += ballColors[i];
                            g_Leds[_length - position - 2] += ballColors[i];
                        }
                    }
                    FastLED.show();
                    delay(15);
                }
};
