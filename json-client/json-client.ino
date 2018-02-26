#include "DHT.h"
#define DHTPIN 13
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "wifi_name";
const char *password = "wifi_pwd";

void setup()
{
  WiFi.begin(ssid, password);
  dht.begin();
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
  WiFi.mode(WIFI_STA);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    char params[100];

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);

    if (!isnan(h) || !isnan(t) || !isnan(f)) {
      char hB[10];
      dtostrf(h, 4, 2, hB);
      char tB[10];
      dtostrf(t, 4, 2, tB);

      float hic = dht.computeHeatIndex(t, h, false);
      char hicB[10];
      dtostrf(hic, 4, 2, hicB);

      snprintf(params, 100, "humidity=%s&temperature=%s&heat_index=%s", hB, tB, hicB);

      HTTPClient http;
      http.begin("http://192.168.0.47:8000/"); // specifie the address
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      http.POST(params);    
      http.end();
    }
  }
  ESP.deepSleep(120000000); 
}
