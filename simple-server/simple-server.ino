#include "DHT.h"
#define DHTPIN 13
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>


ESP8266WebServer server ( 80 );

const char *ssid = "wifi_name";
const char *password = "wifi_pwd";

void handleRoot() {
  char html[500];
  
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  

  if (isnan(h) || isnan(t) || isnan(f)) {
    snprintf (html, 500,
    "<html>\
      <head>\
        <meta http-equiv='refresh' content='120'/>\
        <title>ESP8266 Temperature Demo</title>\
        <style>\
          body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
        </style>\
      </head>\
      <body>\
        <h1>Hello from ESP8266!</h1>\
        <p>Failed to read from DHT sensor!</p>\
      </body>\
    </html>"
    );
  } else {
    char hB[10];
    dtostrf(h, 4, 2, hB);
    char tB[10];
    dtostrf(t, 4, 2, tB);
    char fB[10];
    dtostrf(f, 4, 2, fB);
    
    float hif = dht.computeHeatIndex(f, h);
    char hifB[10];
    dtostrf(hif, 4, 2, hifB);
    
    float hic = dht.computeHeatIndex(t, h, false);
    char hicB[10];
    dtostrf(hic, 4, 2, hicB);
    
    snprintf (html, 500,
    "<html>\
      <head>\
        <meta http-equiv='refresh' content='120'/>\
        <title>ESP8266 Temperature Demo</title>\
        <style>\
          body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
        </style>\
      </head>\
      <body>\
        <h1>Hello from ESP8266!</h1>\
        <p>Humidity: %s %</p>\
        <p>Temperature: %s  &deg;C %s  &deg;F</p>\
        <p>Heat index: %s  &deg;C %s  &deg;F</p>\
      </body>\
    </html>",
  
      hB, tB, fB, hicB, hifB 
    );
  }

  

  
  
  server.send ( 200, "text/html", html );
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );
}

void setup ( void ) {
  Serial.begin (9600);
  WiFi.begin ( ssid, password );
  WiFi.mode(WIFI_STA);
  dht.begin();
  Serial.println ( "" );

  // Wait for connection
  while ( WiFi.status() != WL_CONNECTED ) {
    delay (500);
    Serial.print ( "." );
  }

  Serial.println ( "" );
  Serial.print ( "Connected to " );
  Serial.println ( ssid );
  Serial.print ( "IP address: " );
  Serial.println ( WiFi.localIP() );


  server.on ( "/", handleRoot );
  server.onNotFound ( handleNotFound );
  server.begin();
  Serial.println ( "HTTP server started" );
}

void loop (void) {
  server.handleClient();
}
