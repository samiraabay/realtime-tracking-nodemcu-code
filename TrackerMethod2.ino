#include "NMEAGPS.h"
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

SoftwareSerial mySerial(4, 5); // RX, TX//

NMEAGPS     gps;
gps_fix     fix;
uint8_t     GPSupdates = 0;
String latitude="", longitude="", speedString="";

const char* ssid = "RedmiS2";
const char* password = "Samrox123";

String trackerId = "18-1425";
String url = "https://geozilla.onrender.com/location/set-location?trackerId=";

void setup()  
{
  Serial.begin(9600);
  mySerial.begin(9600);

  Serial.print("Connecting");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("Sizeof(gps) = "); 
  Serial.println(sizeof(gps));
  Serial.println();
}

bool sendLocation(String LongitudeString, String LatitudeString, String speedString)
{
    WiFiClientSecure client;
    client.setInsecure();
    
    HTTPClient https;
    
    String fullUrl = url + trackerId + "&longitude=" + LongitudeString + "&latitude=" + LatitudeString + "&speed=" + speedString;
    
    Serial.println("Requesting " + fullUrl);
    if (https.begin(client, fullUrl)) {
      int httpCode = https.POST("");
      Serial.println("============== Response code: " + String(httpCode));
      if (httpCode > 0) {
        Serial.println(https.getString());
      }
      https.end();
      return true;
    } 
    else {
      Serial.printf("[HTTPS] Unable to connect\n");
      return false;
    }
      Serial.println(trackerId);
      Serial.println(LongitudeString);
      Serial.println(LatitudeString);
}

void gpsdump()
{
  Serial.print("Lat/Long(float): ");
  if (fix.valid.location) {
    latitude=String(fix.latitude(), 5);
    Serial.print(latitude); 
    Serial.print(", "); 
    longitude=String(fix.longitude(), 5);
    Serial.print(longitude);
  }

  Serial.print(" (kmph): ");
  if (fix.valid.speed)
  {
    speedString=String(fix.speed_kph());
    Serial.print(speedString);
    sendLocation(longitude, latitude, speedString);
  }
  Serial.println();
}

void loop()
{
  while (gps.available( mySerial )) {
    fix = gps.read();

    if (fix.valid.speed && (fix.speed_mkn() < 1000)) {
      // Too slow, zero out the speed
      fix.spd.whole = 0;
      fix.spd.frac  = 0;
    }
    GPSupdates++;
  }

  if (GPSupdates >= 1) {// Print once every second
    Serial.println("Acquired Data");
    Serial.println("-------------");
    gpsdump();
    Serial.println("-------------");
    Serial.println();
    GPSupdates = 0;
  }
}
