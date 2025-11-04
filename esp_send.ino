// ====== ESP32 MQTT Publisher: uptime ======
#include <WiFi.h>
#include <PubSubClient.h>

// ---- CONFIG ----
const char* WIFI_SSID     = "YOUR_SSID";
const char* WIFI_PASSWORD = "YOUR_PASS";

const char* MQTT_HOST = "192.168.1.10"; // ou "mqtt.seudominio.com"
const uint16_t MQTT_PORT = 1883;
const char* MQTT_USER = "";             // opcional
const char* MQTT_PASS = "";             // opcional

const char* TOPIC_UPTIME = "lab/esp32/uptime";
const uint32_t PUBLISH_INTERVAL_MS = 5000;

// ---- GLOBALS ----
WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);
unsigned long lastPub = 0;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(300); }
}

void connectMQTT() {
  mqtt.setServer(MQTT_HOST, MQTT_PORT);
  while (!mqtt.connected()) {
    String cid = "esp32-pub-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    bool ok = (MQTT_USER[0] == '\0')
      ? mqtt.connect(cid.c_str())
      : mqtt.connect(cid.c_str(), MQTT_USER, MQTT_PASS);
    if (!ok) { delay(1000); }
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

  unsigned long now = millis();
  if (now - lastPub >= PUBLISH_INTERVAL_MS) {
    lastPub = now;
    char payload[64];
    // tempo ligado em segundos
    snprintf(payload, sizeof(payload), "{\"uptime_s\": %lu}", now / 1000UL);
    mqtt.publish(TOPIC_UPTIME, payload, true); // retain opcional
  }
}
