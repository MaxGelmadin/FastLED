#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "../include/WiFiHandler.h"

#define LED_PIN 5
#define NUM_LEDS 120
#define BRIGHTNESS 255
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

using namespace std;



void task1code(void* params)
{
  for (;;)
  {
    ws1.cleanupClients();
    if (g_CurrentEffect)
        g_CurrentEffect->Draw();
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
    FastLED.setMaxPowerInMilliWatts(900);
    xTaskCreatePinnedToCore(
      task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      2,  /* Priority of the task */
      &g_task1,  /* Task handle. */
      1); /* Core where the task should run */
}

void loop() { }
