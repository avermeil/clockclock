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
#include <Wire.h>

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
    Wire.begin(); // join i2c bus (address optional for master)
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
    server.on(F("/probe"), handleProbe);
    server.on(F("/gethall"), handleGetHall);
    server.on(F("/sethall"), handleSetHall);
    server.on(F("/sethand"), handleSetHand);

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

    Wire.requestFrom(2, 8);
    while (Wire.available())
    {                                // peripheral may send less than requested
        byte lowByte = Wire.read();  // receive a byte as character
        byte highByte = Wire.read(); // receive a byte as character
        int pos = bytesToInt(lowByte, highByte);
        Serial.println("Got positions..."); // print the character
        Serial.print("pos: ");              // print the character
        Serial.println(pos);                // print the character
    }

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

void handleGetHall()
{
    char temp[BUFFER_SIZE];

    Wire.requestFrom(server.arg(0).toInt(), 8);

    int pos1 = bytesToInt(Wire.read(), Wire.read());
    int pos2 = bytesToInt(Wire.read(), Wire.read());
    int pos3 = bytesToInt(Wire.read(), Wire.read());
    int pos4 = bytesToInt(Wire.read(), Wire.read());

    snprintf(temp, BUFFER_SIZE - 1,
             "[%d,%d,%d,%d]",
             pos1, pos2, pos3, pos4);

    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.send(200, F("text/html"), temp);
}

void handleProbe()
{
    byte count = 0;
    String json = "[";
    for (byte i = 1; i < 20; i++)
    {
        Wire.beginTransmission(i);
        if (Wire.endTransmission() == 0)
        {
            if (count != 0)
            {
                json = json + ",";
            }
            json = json + String(i);

                        count++;
        }         // end of good response
        delay(5); // give devices time to recover
    }             // end of for loop

    json = json + "]";

    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.send(200, F("application/json"), json);
}

void handleSetHall()
{
    setHallPos(server.arg(0).toInt(), server.arg(1).toInt(), server.arg(2).toInt());

    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.send(200, F("application/json"), "{\"ok\":true}");
}

void handleSetHand()
{
    int count = 0;
    for (int i = 0; i < (server.args() / 6); i++)
    {
        int plus_i = count * 6;

        setHandPos(
            server.arg(0 + plus_i).toInt(),
            server.arg(1 + plus_i).toInt(),
            server.arg(2 + plus_i).toInt(),
            server.arg(3 + plus_i).toInt(),
            server.arg(4 + plus_i).toInt(),
            server.arg(5 + plus_i).toInt());

        count++;
    }

    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.send(200, F("application/json"), "{\"ok\":true}");
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

int16_t bytesToInt(byte low, byte high)
{
    return ((high & 0xFF) << 8) | (low & 0xFF);
}

void setHallPos(byte board, byte hand, int hallPos)
{
    Serial.println("SENDING CALIBRATION DATA");
    Wire.beginTransmission(board);

    Wire.write(0); // command
    Wire.write(hand);

    Wire.write(lowByte(hallPos));
    Wire.write(highByte(hallPos));

    Wire.endTransmission();
}

void setHandPos(byte board, byte hand, int handPos, byte extraTurns, bool clockwise, int speed)
{
    Serial.println("sending new handPos");
    Wire.beginTransmission(board);

    Wire.write(1); // command
    Wire.write(hand);

    Wire.write(lowByte(handPos));
    Wire.write(highByte(handPos));

    Wire.write(extraTurns);
    Wire.write(clockwise);

    Wire.write(lowByte(speed));
    Wire.write(highByte(speed));

    Wire.endTransmission();
}