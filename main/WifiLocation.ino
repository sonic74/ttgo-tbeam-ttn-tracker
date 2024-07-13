#ifdef ARDUINO_ARCH_SAMD
#include <WiFi101.h>
#elif defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32
#include <WiFi.h>
#include "time.h"
#else
#error Wrong platform
#endif

#include <WifiLocation.h>

const char* googleApiKey = ;

void printLocalTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

float lat, lon;
int acc;
float WifiLocation_lat() {return lat;}
float WifiLocation_lon() {return lon;}
int WifiLocation_acc() {return acc;}

bool WifiLocation_setup() {
    // Connect to WPA/WPA2 network
#ifdef ARDUINO_ARCH_ESP32
    WiFi.mode(WIFI_MODE_STA);
#endif
#ifdef ARDUINO_ARCH_ESP8266
    WiFi.mode(WIFI_STA);
#endif
    if (WiFi.status() != WL_CONNECTED) WiFi.begin();
    int tries=0;
    while (WiFi.status() != WL_CONNECTED) {
        if(tries++>20) {
          WiFi.mode(WIFI_OFF);
          return false;
        }
        screen_print("Attempting to connect: \n");
        // wait 5 seconds for connection:
        Serial.println("Status = ");
        Serial.println(WiFi.status());
        { char buffer[40]; snprintf(buffer, sizeof(buffer), "Status = %i\n", WiFi.status()); screen_print(buffer); }
        delay(1000);
    }
    configTime(0, 0, "pool.ntp.org");
    printLocalTime();
    WifiLocation location(googleApiKey);
    location_t loc = location.getGeoFromWiFi();

    { char buffer[40]; snprintf(buffer, sizeof(buffer), "Latitude:  %10.6f\n", lat=loc.lat); screen_print(buffer); }
    { char buffer[40]; snprintf(buffer, sizeof(buffer), "Longitude:  %10.6f\n", lon=loc.lon); screen_print(buffer); }
    { char buffer[40]; snprintf(buffer, sizeof(buffer), "Accuracy:  %dm\n", acc=loc.accuracy); screen_print(buffer); }
    { char buffer[40]; snprintf(buffer, sizeof(buffer), "Status:  %s\n", location.wlStatusStr(location.getStatus())); screen_print(buffer); }
WiFi.mode(WIFI_OFF);
    return loc.accuracy<40000;
}