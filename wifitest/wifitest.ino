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
#include <NTPClient.h>
#include <arduino-timer.h>
#include "./hand.h"

/*
    Wifi stuff
*/

#define BUFFER_SIZE 512
const char SSID[] = SECRET_SSID;
const char PASS[] = SECRET_PASS;

int status = WL_IDLE_STATUS; // the Wifi radio's status
int INTERNAL_LED = 13;

WiFiUDP ntpUDP;

WiFiWebServer server(80);

/*
    NTP stuff
*/

const long utcOffsetInSeconds = 3600;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

/*
    Clock stuff
*/

const int DEFAULT_SPEED = 2000;
const int SINGLE_ROTATION_STEPS = 4320;
const int bottom = 0;
const int bottom_left = SINGLE_ROTATION_STEPS * 1 / 8;
const int left = SINGLE_ROTATION_STEPS * 2 / 8;
const int top_left = SINGLE_ROTATION_STEPS * 3 / 8;
const int top = SINGLE_ROTATION_STEPS * 4 / 8;
const int top_right = SINGLE_ROTATION_STEPS * 5 / 8;
const int right = SINGLE_ROTATION_STEPS * 6 / 8;
const int bottom_right = SINGLE_ROTATION_STEPS * 7 / 8;

auto timer = timer_create_default(); // create a timer with default settings

const byte HAND_COUNT = 48;
Hand hands[HAND_COUNT] = {
    // row 1
    Hand(1, 0),
    Hand(1, 1),
    Hand(2, 0),
    Hand(2, 1),
    Hand(3, 0),
    Hand(3, 1),

    // row 2
    Hand(1, 2),
    Hand(1, 3),
    Hand(2, 2),
    Hand(2, 3),
    Hand(3, 2),
    Hand(3, 3),

    // row 3
    Hand(4, 0),
    Hand(4, 1),
    Hand(5, 0),
    Hand(5, 1),
    Hand(6, 0),
    Hand(6, 1),

    // row 4
    Hand(4, 2),
    Hand(4, 3),
    Hand(5, 2),
    Hand(5, 3),
    Hand(6, 2),
    Hand(6, 3),

    // row 5
    Hand(7, 0),
    Hand(7, 1),
    Hand(8, 0),
    Hand(8, 1),
    Hand(9, 0),
    Hand(9, 1),

    // row 6
    Hand(7, 2),
    Hand(7, 3),
    Hand(8, 2),
    Hand(8, 3),
    Hand(9, 2),
    Hand(9, 3),

    // row 7
    Hand(10, 0),
    Hand(10, 1),
    Hand(11, 0),
    Hand(11, 1),
    Hand(12, 0),
    Hand(12, 1),

    // row 8
    Hand(10, 2),
    Hand(10, 3),
    Hand(11, 2),
    Hand(11, 3),
    Hand(12, 2),
    Hand(12, 3)};

void setup()
{

    // start of crazy clock reduction
    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(12) | // Divide the 48MHz clock source by divisor 12: 48MHz/12=4MHz
                       GCLK_GENDIV_ID(3);    // Select Generic Clock (GCLK) 3
    while (GCLK->STATUS.bit.SYNCBUSY)
        ; // Wait for synchronization

    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |         // Set the duty cycle to 50/50 HIGH/LOW
                        GCLK_GENCTRL_GENEN |       // Enable GCLK3
                        GCLK_GENCTRL_SRC_DFLL48M | // Set the 48MHz clock source
                        GCLK_GENCTRL_ID(3);        // Select GCLK3
    while (GCLK->STATUS.bit.SYNCBUSY)
        ; // Wait for synchronization

    SerialUSB.begin(115200); // Set-up the native USB port

    delay(1000);
    // while (!SerialUSB)
    //     ; // Wait until the native USB port is ready

    Wire.begin();                                     // Set-up the I2C port
    sercom4.disableWIRE();                            // Disable the I2C SERCOM
    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |          // Enable GCLK3 as clock soruce to SERCOM3
                        GCLK_CLKCTRL_GEN_GCLK3 |      // Select GCLK3
                        GCLK_CLKCTRL_ID_SERCOM4_CORE; // Feed the GCLK3 to SERCOM3
    while (GCLK->STATUS.bit.SYNCBUSY)
        ;                                                    // Wait for synchronization
    SERCOM4->I2CM.BAUD.bit.BAUD = 4000000 / (2 * 10000) - 1; // Set the I2C clock rate to 10kHz
    sercom4.enableWIRE();                                    // Enable the I2C SERCOM

    // end of cray section

    pinMode(13, OUTPUT);

    // Serial.begin(115200);
    // Wire.begin();

    // Wire.setClock(1000); // doesn't work?

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

    connectWifi();
    // status = WiFi.begin(SECRET_SSID, SECRET_PASS);

    // delay(1000);

    // // attempt to connect to WiFi network
    // while (status != WL_CONNECTED)
    // {
    //     // flash the LED while connecting
    //     digitalWrite(INTERNAL_LED, 1);
    //     delay(100);
    //     digitalWrite(INTERNAL_LED, 0);

    //     delay(400);

    //     // Connect to WPA/WPA2 network
    //     status = WiFi.status();
    // }

    server.on(F("/"), handleRoot);
    server.on(F("/probe"), handleProbe);
    server.on(F("/gethall"), handleGetHall);
    server.on(F("/sethall"), handleSetHall);
    server.on(F("/sethand"), handleSetHand);
    server.on(F("/test"), handleTest);
    server.onNotFound(handleNotFound);

    server.begin();

    Serial.print(F("HTTP server started @ "));
    Serial.println(WiFi.localIP());

    timeClient.begin();

    digitalWrite(INTERNAL_LED, 1);

    timer.every(1000, moveSingleHand);
}

void connectWifi()
{
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);

    delay(500);

    // attempt to connect to WiFi network
    while (status != WL_CONNECTED)
    {
        // flash the LED while connecting
        digitalWrite(INTERNAL_LED, 1);
        delay(50);
        digitalWrite(INTERNAL_LED, 0);
        delay(50);

        // Connect to WPA/WPA2 network
        status = WiFi.status();
    }
}

void loop()
{
    server.handleClient();
    status = WiFi.status();

    timeClient.update();

    timer.tick(); // tick the timer

    byte flashes = 0;

    if (status == WL_CONNECT_FAILED)
        flashes = 1;
    if (status == WL_CONNECTION_LOST)
        flashes = 2;
    if (status == WL_DISCONNECTED)
        flashes = 3;
    if (status == WL_NO_SHIELD)
        flashes = 4;
    if (status == WL_IDLE_STATUS)
    {
        flashes = 5;
        // Serial.print(F("Encountered WL_IDLE_STATUS, reconnecting... "));
        // connectWifi();
    }
    if (status == WL_NO_SSID_AVAIL)
        flashes = 6;
    if (status == WL_SCAN_COMPLETED)
        flashes = 7;

    if (flashes > 0)
    {
        for (byte i = 0; i < flashes; i++)
        {
            digitalWrite(INTERNAL_LED, 1);
            delay(100);
            digitalWrite(INTERNAL_LED, 0);
            delay(200);
        }

        delay(1000);
    }
}

void setHallPos(byte board, byte hand, int hallPos)
{
    // Serial.println("SENDING CALIBRATION DATA");
    Wire.beginTransmission(board);

    Wire.write(0); // command
    Wire.write(hand);

    Wire.write(lowByte(hallPos));
    Wire.write(highByte(hallPos));

    Wire.endTransmission();
}

void setHandPos(byte board, byte hand, int handPos, byte extraTurns, bool clockwise, int speed)
{
    Serial.println((String) "sending new handPos... board:" + board + ", hand:" + hand + ", handPos:" + handPos + ", extraTurns:" + extraTurns + ", clockwise:" + clockwise + ", speed:" + speed);
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

void setUpWave()
{
    for (byte hand = 0; hand < HAND_COUNT; hand++)
    {
        int dest = hand % 2 == 0 ? top_right : bottom_left;

        hands[hand].moveTo(dest, 0, 1, DEFAULT_SPEED);
    }

    // set up timer for setUpSpin in 5s
    timer.in(8 * 1000, setUpSpin);
}

bool setUpSpin(void *)
{

    for (byte j = 0; j < 8; j++)
    {
        Serial.println((String) "column:" + j);

        for (byte i = 0; i < 6; i++)
        {
            byte hand = (6 * j) + i;
            hands[hand].moveTo(bottom, 5, 1, 1000);
            Serial.println((String) "Moving hand:" + hand);
        }
        delay(300);
    }
}

bool moveSingleHand(void *)
{
    // Serial.println((String) "1s");
    Serial.println(timeClient.getFormattedTime());

    // hands[1].moveTo(bottom, 5, 1, 1000);
}

// bool spin(void *)
// {
//     setHandPos(board, hand, bottom, 5, 1, DEFAULT_SPEED);

//     for (byte board = 1; board <= 12; board++)
//     {
//         for (byte hand = 0; hand < 4; hand++)
//         {
//         }
//     }
// }

/*
   HTTP SERVER FUNCTIONS
*/

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

    int hand = server.arg(0).toInt();

    hands[hand].refreshData();

    snprintf(temp, BUFFER_SIZE - 1,
             "[%d,%d,%d]",
             hands[hand].calibration, hands[hand].position, hands[hand].isClockwise);

    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.send(200, F("text/html"), temp);
}

void handleProbe()
{
    Serial.println("Probing..."); // print the character

    byte count = 0;
    String json = "[";
    // for (byte i = 1; i < 13; i++)
    // {
    //     Wire.beginTransmission(i);
    //     if (Wire.endTransmission() == 0)
    //     {
    //         if (count != 0)
    //         {
    //             json = json + ",";
    //         }
    //         json = json + String(i);

    //         count++;
    //     }          // end of good response
    //     delay(10); // give devices time to recover
    // }              // end of for loop

    // json = json + "]";

    json = json + "1,2,3,4,5,6,7,8,9,10,11,12]";

    Serial.println("Done probing."); // print the character

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
    for (int i = 0; i < (server.args() / 5); i++)
    {
        int plus_i = count * 6;
        hands[server.arg(0 + plus_i).toInt()].moveTo(server.arg(1 + plus_i).toInt(),
                                                     server.arg(2 + plus_i).toInt(),
                                                     server.arg(3 + plus_i).toInt(),
                                                     server.arg(4 + plus_i).toInt());

        count++;
    }

    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.send(200, F("application/json"), "{\"ok\":true}");
}

void handleTest()
{
    Serial.println("doing test...");

    setUpWave();

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

/*
    UTILITY FUNCTIONS
*/

int16_t bytesToInt(byte low, byte high)
{
    return ((high & 0xFF) << 8) | (low & 0xFF);
}