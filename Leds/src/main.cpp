#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#define NUM_LEDS 120
#include "../include/WiFiHandler.h"
#define LED_PIN 5
#define BRIGHTNESS 75
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB


void task1code(void* params)
{
  for (;;)
  {
    //Serial.println("Inside task1");
    ws1.cleanupClients();
    if (g_CurrentEffect)
        g_CurrentEffect->Draw();
    else
      vTaskSuspend(nullptr);
  }
}

void setup()
{
    Serial.begin(9600);
    ConnectToWiFi();
    initWebSocket();
    pinMode(LED_PIN, OUTPUT);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(g_Leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    //FastLED.setMaxPowerInMilliWatts(900);
    xTaskCreatePinnedToCore(
      task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      2,  /* Priority of the task */
      &g_task1,  /* Task handle. */
      1); /* Core where the task should run */


    // Just a fun way to indicate that the leds are ready to work after chip boot
    fill_solid(g_Leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
    delay(250);
    FastLED.clear(true);
    delay(250);

    fill_solid(g_Leds, NUM_LEDS, CRGB::Green);
    FastLED.show();
    delay(250);
    FastLED.clear(true);
    delay(250);

    fill_solid(g_Leds, NUM_LEDS, CRGB::Blue);
    FastLED.show();
    delay(250);
    FastLED.clear(true);
}

void loop() {}
