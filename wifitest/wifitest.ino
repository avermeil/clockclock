/****************************************************************************************************************************
  HelloServer.ino - Simple Arduino web server sample for SAMD21 running WiFiNINA shield
  For any WiFi shields, such as WiFiNINA W101, W102, W13x, or custom, such as ESP8266/ESP32-AT, Ethernet, etc

  WiFiWebServer is a library for the ESP32-based WiFi shields to run WebServer
  Based on and modified from ESP8266 https://github.com/esp8266/Arduino/releases
  Based on  and modified from Arduino WiFiNINA library https://www.arduino.cc/en/Reference/WiFiNINA
  Built by Khoi Hoang https://github.com/khoih-prog/WiFiWebServer
  Licensed under MIT license
 ***************************************************************************************************************************************/
#include "arduino_secrets.h"
#include "defines.h"

const char SSID[] = SECRET_SSID; // Network SSID (name)
const char PASS[] = SECRET_PASS; // Network password (use for WPA, or use as key for WEP)

int status = WL_IDLE_STATUS; // the Wifi radio's status
int reqCount = 0;            // number of requests received
int led = 13;
#define BUFFER_SIZE 512

WiFiWebServer server(80);

void setup()
{
    pinMode(13, OUTPUT);
    // Open serial communications and wait for port to open:
    Serial.begin(115200);
    delay(2000);

    Serial.print(F("\nStarting HelloServer on "));
    Serial.print(BOARD_NAME);
    Serial.print(F(" with "));
    Serial.println(SHIELD_TYPE);
    Serial.println(WIFI_WEBSERVER_VERSION);

    String fv = WiFi.firmwareVersion();

    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    {
        Serial.println(F("Please upgrade the firmware"));
        Serial.println(fv);
        Serial.println(WIFI_FIRMWARE_LATEST_VERSION);
    }

    Serial.print(F("Connecting to SSID: "));
    Serial.println(SECRET_SSID);

    status = WiFi.begin(SECRET_SSID, SECRET_PASS);

    delay(1000);

    // attempt to connect to WiFi network
    while (status != WL_CONNECTED)
    {
        delay(500);

        // Connect to WPA/WPA2 network
        status = WiFi.status();
    }

    // server.begin();

    server.on(F("/"), handleRoot);

    server.on(F("/inline"), []()
              { 
                  server.send(200, F("text/plain"), F("This works as well")); 
                  server.send(200, F("text/plain"), F("This works as well")); });

    server.onNotFound(handleNotFound);

    server.begin();

    Serial.print(F("HTTP server started @ "));
    Serial.println(WiFi.localIP());

    digitalWrite(led, 1);
}

void loop()
{
    server.handleClient();
}

void handleRoot()
{
    char temp[BUFFER_SIZE];
    int sec = millis() / 1000;
    int min = sec / 60;
    int hr = min / 60;
    int day = hr / 24;

    snprintf(temp, BUFFER_SIZE - 1,
             "<html>\
<head>\
<title>%s</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
</head>\
<body>\
<h1>Hello from %s</h1>\
<h3>running WiFiWebServer</h3>\ 
<h3>on %s</h3>\
<p>Uptime: %d d %02d:%02d:%02d</p>\
</body>\
</html>",
             BOARD_NAME, BOARD_NAME, SHIELD_TYPE, day, hr, min % 60, sec % 60);

    server.send(200, F("text/html"), temp);
}

void handleNotFound()
{
    String message = F("File Not Found\n\n");

    message += F("URI: ");
    message += server.uri();
    message += F("\nMethod: ");
    message += (server.method() == HTTP_GET) ? F("GET") : F("POST");
    message += F("\nArguments: ");
    message += server.args();
    message += F("\n");

    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, F("text/plain"), message);
}