#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include "abstract.h"
#include "stars.h"
#include "comet.h"
#include "bounce.h"
#include "fire.h"
#include "marquee.h"
#include "WiFi.h"
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define NUM_LEDS 120

typedef struct {
  const char* name;
  AbstractDraw* effect;
} functionMapping;

AbstractDraw* g_CurrentEffect = nullptr;
const char* g_NameOfCurrentEffect = nullptr;
TaskHandle_t g_task1;
CRGB g_Leds[NUM_LEDS] = { 0 };

// ---------- Global variables for all the effects ----------
Comet comet(0);
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
functionMapping g_Effects[] = {
    {"Marquee", &marquee},
    {"Stars", &stars},
    {"Comet", &comet},
    {"Clashing Comets", &clashingComets},
    {"Bouncing Balls", &bouncingEffect},
    {"Fire1", &fireEffect1},
    {"Fire2", &fireEffect2},
    {"Fire3", &fireEffect3},
    {"Fire4", &fireEffect4},
    {"Fire5", &fireEffect5},
    {"Fire6", &fireEffect6}
};

const char *SSID = "maxim";
const char *WiFiPassword = "maxsofiron";
AsyncWebServer server(80);
AsyncWebSocket ws1("/ws");


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
  .small-btn {
    width: 50px;
    height: 21.33px;
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
   <table>
      <tr>
        <th>Brightness: <input name="brightness" type="text" maxlength="512" id="brightness" class="searchField"/></th>
        <th><button id="button98" class="small-btn">Apply</button98></th>
      </tr>
      <tr>
        <th>Static Color: <input name="static_color" type="text" maxlength="512" id="static_color" class="searchField"/></th>
        <th><button id="button97" class="small-btn">Apply</button97></th>
      </tr>
    </table>
  <div class="content">
    <div class="card">
      <p class="state">Current Effect: <span id="state: ">%STATE%</span></p>
      <p><button id="button0" class="button">Marquee</button0></p>
      <p><button id="button1" class="button">Stars</button1></p>
      <p><button id="button2" class="button">Comet</button2></p>
      <p><button id="button3" class="button">Clashing Comet</button3></p>
      <p><button id="button4" class="button">Bouncing Balls</button4></p>
      <p><button id="button5" class="button">Fire1 - Top -> Middle</button5></p>
      <p><button id="button6" class="button">Fire2 - Inwards Mirrored</button6></p>
      <p><button id="button7" class="button">Fire3 - Middle -> Outwards</button7></p>
      <p><button id="button8" class="button">Fire4 - Bottom -> Middle</button8></p>
      <p><button id="button9" class="button">Fire5 - Middle -> Outwards (Stronger)</button9></p>
      <p><button id="button10" class="button">Fire6 - Stronger Version of Fire2</button10></p>
      <p><button id="button99" class="button">Turn Off</button99></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  const numOfEffects = 11;
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
    for (let i = 0; i < numOfEffects; i++) {
      document.getElementById(`button${i}`).addEventListener('click', function() { websocket.send(i.toString()) });
    }
    document.getElementById('button99').addEventListener('click', function() { websocket.send("Z") });
    document.getElementById('button98').addEventListener('click', function() { websocket.send(`brightness${brightness.value}`) });
    document.getElementById('button97').addEventListener('click', function() { websocket.send(`static_color${static_color.value}`) });
  }
</script>
</body>
</html>
)rawliteral";
void notifyClients() {
  ws1.textAll(g_NameOfCurrentEffect);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      data[len] = 0;
      for (int i = 0; i < len; i++) {
        Serial.printf("%c", data[i]);
      }
      Serial.println();
      if (strncmp((char*) data, "brightness", 10) == 0)
      {
        uint8_t brightness = uint8_t (atoi(((char*) (data + 10))));
        Serial.printf("Brightness - %d\n", brightness);
        if (brightness >= 0 && brightness <= 255)
          FastLED.setBrightness(brightness);
          FastLED.show();
      }
      else if (strncmp((char*) data, "static_color", 12) == 0)
      {
        g_CurrentEffect = nullptr;
        //FastLED.clear(true);
        int color = strtol((char*) (data + 12), NULL, 16);
        //int color = atoi((char*) (data + 12));            // 0x__ __ __
        Serial.printf("color - %x\n", color);
        uint8_t r = (color & (0x00FF0000)) >> 16;
        uint8_t g = (color & (0x0000FF00)) >> 8;
        uint8_t b = (color & (0x000000FF)) >> 0;
        Serial.printf("r - %x, g - %x, b - %x\n", r, g, b);
        fill_solid(g_Leds, NUM_LEDS, {r, g, b});
        FastLED.show();
      }
      else if (strcmp((char*) data, "Z") == 0)
      {
        g_CurrentEffect = nullptr;
        FastLED.clear(true);
        g_NameOfCurrentEffect = "Turned Off";
        esp_deep_sleep_start();
      }
      else
      {
      int temp = atoi((char*) data);
      g_CurrentEffect = g_Effects[temp].effect;
      g_NameOfCurrentEffect = g_Effects[temp].name;
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
    return g_NameOfCurrentEffect;
  }
}

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