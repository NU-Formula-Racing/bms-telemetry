#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <DNSServer.h>
#include "virtualTimer.h"
#include "BMSInterface.h"
#include "esp_can.h"

const byte DNS_PORT = 53;
const char *ssid = "FormulaBMS";
const char *password = "formulabmspass";
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const uint8_t kNumSeries{140};
const uint8_t kNumThermistors{112};

VirtualTimerGroup timer_group;

ESPCAN can_interface{};

BMS bms{can_interface};

void onRootRequest(AsyncWebServerRequest *request)
{
  request->send(SPIFFS, "/index.html", "text/html");
}

void initWebServer()
{
  server.on("/", onRootRequest);
  server.serveStatic("/", SPIFFS, "/");
  server.begin();
}

void notifyClients()
{
  static StaticJsonDocument<10000> doc;
  doc["state"] = bms.GetState();
  doc["current"] = bms.GetCurrent();
  static JsonObject cell_voltages = doc.createNestedObject("voltages");
  static JsonObject cell_temperatures = doc.createNestedObject("temperatures");
  for (int i = 0; i < kNumSeries; i++)
  {
    cell_voltages[String(i)] = bms.GetVoltage(i);
  }
  for (int i = 0; i < kNumThermistors; i++)
  {
    cell_temperatures[String(i)] = bms.GetTemperature(i);
  }

  static char data[10000];
  size_t len = serializeJson(doc, data);
  ws.textAll(data, len);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {

    const uint8_t size = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<size> json;
    DeserializationError err = deserializeJson(json, data);
    if (err)
    {
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(err.c_str());
      return;
    }

    const char *action = json["action"];
    if (strcmp(action, "estop") == 0)
    {
      bms.Shutdown();
    }
    else if (strcmp(action, "enable") == 0)
    {
      bms.PrechargeAndCloseContactors();
    }
  }
}

void onEvent(AsyncWebSocket *server,
             AsyncWebSocketClient *client,
             AwsEventType type,
             void *arg,
             uint8_t *data,
             size_t len)
{

  switch (type)
  {
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

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void initSPIFFS()
{
  if (!SPIFFS.begin())
  {
    Serial.println("Cannot mount SPIFFS volume...");
    while (1)
    {
      // onboard_led.on = millis() % 200 < 50;
      // onboard_led.update();
    }
  }
}

void setup()
{
  Serial.begin(9600);
  WiFi.softAP(ssid, password);
  delay(100);
  initSPIFFS();
  initWebServer();
  initWebSocket();
  dnsServer.start(DNS_PORT, "*", apIP);
  timer_group.AddTimer(100, notifyClients);
  /* timer_group.AddTimer(10, []()
                       { can_interface.Tick(); }); */
}

void loop()
{
  ws.cleanupClients();
  timer_group.Tick(millis());
}