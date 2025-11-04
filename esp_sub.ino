// ====== ESP32 MQTT Subscriber: blink on message ======
#include <WiFi.h>
#include <PubSubClient.h>

// ---- CONFIG ----
const char* WIFI_SSID     = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASS";

const char* MQTT_HOST = "192.168.1.10";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASS = "";

const char* TOPIC_UPTIME = "lab/esp32/uptime";

// LED: usa LED_BUILTIN se existir, senão GPIO2
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif

// ---- GLOBALS ----
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

void blinkOnce(uint16_t ms = 200) {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(ms);
  digitalWrite(LED_BUILTIN, LOW);
}

void onMessage(char* topic, byte* payload, unsigned int length) {
  // qualquer mensagem no tópico alvo → pisca
  blinkOnce();
}

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  while (WiFi.status() != WL_CONNECTED) { delay(300); }
}

void connectMQTT() {
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  mqtt.setCallback(onMessage);
  while (!mqtt.connected()) {
    String cid = "esp32-sub-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    bool ok = (MQTT_USER[0] == '\0')
      ? mqtt.connect(cid.c_str())
      : mqtt.connect(cid.c_str(), MQTT_USER, MQTT_PASS);
    if (ok) {
      mqtt.subscribe(TOPIC_UPTIME, 0);
    } else {
      delay(1000);
    }
  }
}

void setup() {
  connectWiFi();
  connectMQTT();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) connectWiFi();
  if (!mqtt.connected()) connectMQTT();
  mqtt.loop();
}
