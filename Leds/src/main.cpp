#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "abstract.h"
#include "stars.h"
#include "comet.h"
#include "bounce.h"
#include "fire.h"
#include "Marquee.h"
#include "WiFi.h"
#include <regex>
#include <string.h>

using namespace std;

#define LED_PIN 5
#define NUM_LEDS 120
#define BRIGHTNESS 255
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB


const char *SSID = "YOUR_WIFI_NAME";
const char *WiFiPassword = "WIFI_PASS";
WiFiServer server(80);

void ConnectToWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, WiFiPassword);
    Serial.print("Connecting to ");
    Serial.println(SSID);

    uint8_t i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(500);

        if ((++i % 16) == 0)
        {
            Serial.println(F(" still trying to connect"));
        }
    }

    Serial.print(F("Connected. My IP address is: "));
    Serial.println(WiFi.localIP());
    server.begin();
}


AbstractDraw* current_effect = nullptr;
CRGB leds[NUM_LEDS] = { 0 };
regex str_reg("[a-zA-Z0-9]*GET /[A-Z]");

// ---------- Global variables for all the effects ----------
//Comet comet(0);
Comet clashingComets(1);
Stars stars(-1);
Marquee marquee(-1);
BouncingBalls bouncingEffect(NUM_LEDS, 2, 100, true);
ClassicFire fireEffect1(NUM_LEDS, 30, 2, 3, 100, false, true);       // Outwards from Middle
ClassicFire fireEffect2(NUM_LEDS, 30, 2, 3, 100, true, true);        // Inwards toward Middle
ClassicFire fireEffect3(NUM_LEDS, 20, 4, 3, 100, true, false);       // Outwards from Zero
ClassicFire fireEffect4(NUM_LEDS, 20, 4, 3, 100, false, false);      // Inwards from End
ClassicFire fireEffect5(NUM_LEDS, 50, 12, 30, 300, true, false);     // More Intense, Extra Sparking
ClassicFire fireEffect6(NUM_LEDS, 23, 5, 15, 200, true, true);
// ------------------------------------------------------------

// Save all the effects in this array for easy access when operating between different effects.
AbstractDraw* effects[] = {
    &marquee,
    &stars,
    //&comet,
    &clashingComets,
    &bouncingEffect,
    &fireEffect1,
    &fireEffect2,
    &fireEffect3,
    &fireEffect4,
    &fireEffect5,
    &fireEffect6
};


void setup()
{
    Serial.begin(9600);
    ConnectToWiFi();
    pinMode(LED_PIN, OUTPUT);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.setMaxPowerInMilliWatts(900);
}

void loop()
{
    if (current_effect != nullptr)
        current_effect->Draw();
    // -------------------------------------------------
    WiFiClient client = server.available(); // listen for incoming clients

    if (client)
    {                                  // if you get a client,
        //Serial.println("New Client."); // print a message out the serial port
        std::string currentLine("");       // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                Serial.write(c);        // print it out the serial monitor
                if (c == '\n')
                { // if the byte is a newline character

                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.print("Click <a href=\"/A\">here</a> to turn `Marquee` on.<br>");
                        client.print("Click <a href=\"/B\">here</a> to turn `Stars` on.<br>");
                        //client.print("Click <a href=\"/C\">here</a> to turn `Comet` on.<br>");
                        client.print("Click <a href=\"/C\">here</a> to turn `Clashing Comets` on.<br>");
                        client.print("Click <a href=\"/D\">here</a> to turn `Bouncing Balls` on.<br>");
                        client.print("Click <a href=\"/E\">here</a> to turn `Fire - Middle -> Outwards` on.<br>");
                        client.print("Click <a href=\"/F\">here</a> to turn `Fire - Inwards -> Middle` on.<br>");
                        client.print("Click <a href=\"/G\">here</a> to turn `Fire - Zero -> Outwards` on.<br>");
                        client.print("Click <a href=\"/H\">here</a> to turn `Fire - End -> Inwards` on.<br>");
                        client.print("Click <a href=\"/I\">here</a> to turn `Fire - Zero -> Outwards [Stronger]` on.<br>");
                        client.print("Click <a href=\"/J\">here</a> to turn `Fire - Inwrds -> Middle [Custom]` on.<br>");
                        client.print("Click <a href=\"/Z\">here</a> to deep sleep ESP32.<br>");

                        // The HTTP response ends with another blank line:
                        client.println();
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
                if (regex_match(currentLine, str_reg))
                {
                    char temp_c = currentLine[currentLine.length() - 1];
                    if (temp_c == 'Z')                                      // Send chip to sleep.
                    {
                        FastLED.clear(true);
                        esp_deep_sleep_start();
                    }
                    else                                                    // Select the chosen effect.
                    {
                        current_effect = effects[temp_c - 'A'];
                    }
                }
            }
        // close the connection:
        }
        client.stop();
        //Serial.println("Client Disconnected.");
    }
}
