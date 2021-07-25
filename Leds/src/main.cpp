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
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <regex>
#include <string.h>

using namespace std;

#define LED_PIN 5
#define NUM_LEDS 120
#define BRIGHTNESS 255
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB

AbstractDraw* current_effect = nullptr;
CRGB leds[NUM_LEDS] = { 0 };
regex str_reg("[a-zA-Z0-9]*GET /[A-Z]");

// ---------- Global variables for all the effects ----------
//Comet comet(0);
Comet clashingComets(1);
Stars stars(-1);
Marquee marquee(-1);
BouncingBalls bouncingEffect(NUM_LEDS, 5, 50, true);
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
    &bouncingEffect,         //sfdgsnf
    &fireEffect1,
    &fireEffect2,
    &fireEffect3,
    &fireEffect4,
    &fireEffect5,
    &fireEffect6
};


TaskHandle_t Task1, Task2;
const char *SSID = "maxim";
const char *WiFiPassword = "maxsofiron";
//WiFiServer server(80);
AsyncWebServer server(80);
AsyncWebSocket ws1("/ws");
char* nameOfCurrentEffect = nullptr;


//--------------------------------------------
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <p class="state">Current Effect: <span id="state: ">%STATE%</span></p>
      <p><button id="button1" class="button">Comet Effect</button1></p>
      <p><button id="button2" class="button">Bouncing Balls</button2></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    document.getElementById('currentEffect').innerHTML = event.data;
  }
  function onLoad(event) {
    initWebSocket();
    initButtons();
  }
  function initButtons() {
    document.getElementById('button1').addEventListener('click', comet);
    document.getElementById('button2').addEventListener('click', bouncingBalls);
  }
  function comet(){
    websocket.send('comet');
  }
  function bouncingBalls(){
    websocket.send('bouncing balls');
  }
</script>
</body>
</html>
)rawliteral";
void notifyClients() {
  ws1.textAll(nameOfCurrentEffect);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp ((char*)data, "comet") == 0)
    {
        Serial.println("Client clicked on 'comet' effect");
        current_effect = effects[0];
        nameOfCurrentEffect = "Comet";
        notifyClients();
    }
    else if (strcmp ((char*)data, "bouncing balls") == 0)
    {
        Serial.println("Client clicked on 'Bouncing Balls' effect");
        current_effect = effects[3];
        nameOfCurrentEffect = "Bouncing Balls";
        notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws1.onEvent(onEvent);
  server.addHandler(&ws1);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    return nameOfCurrentEffect;
  }
}

//---------------------------------------
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
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
    server.begin();
}

void Task1code(void* params)
{
  for (;;)
  {
    ws1.cleanupClients();
    if (current_effect)
        current_effect->Draw();
  }
}

void setup()
{
    Serial.begin(9600);
    ConnectToWiFi();
    initWebSocket();
    pinMode(LED_PIN, OUTPUT);
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.setMaxPowerInMilliWatts(900);
    xTaskCreatePinnedToCore(
      Task1code, /* Function to implement the task */
      "Task1", /* Name of the task */
      10000,  /* Stack size in words */
      NULL,  /* Task input parameter */
      2,  /* Priority of the task */
      &Task1,  /* Task handle. */
      1); /* Core where the task should run */
      
    delay(500);

    //   xTaskCreatePinnedToCore(
    //   Task2code, /* Function to implement the task */
    //   "Task2", /* Name of the task */
    //   10000,  /* Stack size in words */
    //   NULL,  /* Task input parameter */
    //   2,  /* Priority of the task */
    //   &Task2,  /* Task handle. */
    //   0); /* Core where the task should run */

      //delay(500);
}



void loop() { 
    // ws1.cleanupClients();
    // if (current_effect)
    //     current_effect->Draw();
    
}
