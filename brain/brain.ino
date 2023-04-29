#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <Wire.h>
#include <NTPClient.h>
#include <arduino-timer.h>
#include "./hand.h"
#include <TimeLib.h>

#include <Timezone.h>   // https://github.com/JChristensen/Timezone

// US Eastern Time Zone (New York, Detroit)
TimeChangeRule myDST = {"BST", Last, Sun, Mar, 1, 60};    // Daylight time = UTC + 1 hours
TimeChangeRule mySTD = {"GMT", Last, Sun, Oct, 1, 0};     // Standard time = UTC + 0 hours
Timezone myTZ(myDST, mySTD);

const char ssid[] = SECRET_SSID; // your network SSID (name)
const char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)

auto timer = timer_create_default(); // create a timer with default settings

int status = WL_IDLE_STATUS;
WiFiServer server(80);

const int INTERNAL_LED = 13;

String response = "";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const int DEFAULT_SPEED = 2000;

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

byte previousMinute = 0;
bool stopClocking = false;

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

    // Wire.begin();
    // Serial.begin(9600);            // initialize serial communication
    pinMode(INTERNAL_LED, OUTPUT); // set the LED pin mode

    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE)
    {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true)
            ;
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    {
        Serial.println("Please upgrade the firmware");
    }

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to Network named: ");
        Serial.println(ssid); // print the network name (SSID);

        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
        // wait 10 seconds for connection:
        delay(3000);
    }
    server.begin(); // start the web server on port 80
    timeClient.begin();

    printWifiStatus(); // you're connected now, so print out the status

    digitalWrite(INTERNAL_LED, 1);

    randomSeed(analogRead(0));
}

void loop()
{
    handleWebServer();
    timer.tick(); // tick the timer
    timeClient.update();
    setTime(timeClient.getEpochTime());

    bool isBst = myTZ.utcIsDST(now());
    if(isBst){
      timeClient.setTimeOffset(3600);
    }

    byte minute = timeClient.getMinutes();

    if (!stopClocking && minute != previousMinute)
    {
        previousMinute = minute;

        int transition = random(8);

        if (transition == 0)
        {
            setUpWave();
        }
        else if (transition == 1)
        {
            setUpHypno();
        }
        else if (transition == 2)
        {
            setUpScissors();
        }
        else if (transition == 3)
        {
            setUpSquare();
        }
        else if (transition == 4)
        {
            setUpRandom();
        }
        else
        {
            showTime(0);
        }
    }
}

void setDigitTo(byte digit, byte symbol, bool fast = false)
{
    byte start_index = digit * 12;
    byte end_index = start_index + 12;

    for (byte hand = start_index; hand < end_index; hand++)
    {
        int dest = hands[hand].getDigitPos(symbol);
        if (fast)
        {
            hands[hand].moveTo(dest, 0, FASTEST, 3000);
        }
        else
        {
            hands[hand].moveTo(dest, 0, MAINTAIN, 7);
        }
    }
}

/*
    Transitions

    - wave (straight with time offset)
    - hypno (pointing to centre of display)
    - square (keep everything aligned at the same speed)
    - scissors (reset, then counter-rotating)
    - random
*/

// WAVE
void setUpWave()
{
    for (byte hand = 0; hand < HAND_COUNT; hand++)
    {
        int dest = hand % 2 == 0 ? TOP_RIGHT : BOTTOM_LEFT;

        hands[hand].moveTo(dest, 0, FASTEST, 1800);
    }

    timer.in(6 * 1000, setUpWaveSpin);
}

bool setUpWaveSpin(void *)
{

    for (byte j = 0; j < 8; j++)
    {
        Serial.println((String) "column:" + j);

        for (byte i = 0; i < 6; i++)
        {
            byte hand = (6 * j) + i;
            hands[hand].moveTo(BOTTOM, 5, CLOCKWISE, 1000);
            Serial.println((String) "Moving hand:" + hand);
        }
        delay(240);
    }

    timer.in(8 * 1000, showTime);
}

// HYPNO
void setUpHypno()
{
    int sides[] = {0, 1};
    for (int side = 0; side < 2; side++)
    {
        int columns[] = {0, 1, 2, 3};

        if (side == 1)
        {
            columns[0] = 7;
            columns[1] = 6;
            columns[2] = 5;
            columns[3] = 4;
        }

        for (int columnIndex = 0; columnIndex < 4; columnIndex++)
        {
            for (int rowIndex = 0; rowIndex < 3; rowIndex++)
            {

                int row = rowIndex * 2;

                int hand = ((columns[columnIndex] * 6) + row);

                int base = 270 - (side * 180);

                int offsetMultiplier = (rowIndex - 1) * 45 * (side == 0 ? (columns[columnIndex]) : (7 - columns[columnIndex])) / 2;
                int offset = base - (offsetMultiplier) * (side == 0 ? 1 : -1);

                hands[hand].moveTo(offset * 12, 0, FASTEST, 1800);
                hands[hand + 1].moveTo(offset * 12, 0, FASTEST, 1800);
            }
        }
    }

    timer.in(6 * 1000, setUpHynoSpin);
}

bool setUpHynoSpin(void *)
{

    for (byte hand = 0; hand < HAND_COUNT; hand++)
    {
        hands[hand].moveTo(BOTTOM, 5, CLOCKWISE, 1000);
    }

    timer.in(8 * 1000, showTime);
}

// SQUARE
void setUpSquare()
{
    for (byte hand = 0; hand < HAND_COUNT; hand++)
    {
        hands[hand].moveTo(BOTTOM, 5, CLOCKWISE, 1000);
    }

    timer.in(8 * 1000, showTime);
}

// SCISSORS

void setUpScissors()
{
    for (byte hand = 0; hand < HAND_COUNT; hand++)
    {
        hands[hand].moveTo(BOTTOM_LEFT, 0, FASTEST, 1800);
    }

    timer.in(6 * 1000, setUpScissorsSpin);
}

bool setUpScissorsSpin(void *)
{
    for (byte hand = 0; hand < HAND_COUNT; hand++)
    {
        int mode = hand % 2 == 0 ? CLOCKWISE : ANTI_CLOCKWISE;
        hands[hand].moveTo(BOTTOM_LEFT, 5, mode, 1000);
    }

    timer.in(8 * 1000, showTime);
}

// RANDOM
void setUpRandom()
{
    for (byte hand = 0; hand < HAND_COUNT; hand++)
    {
        int clockwise = random(0, 2);
        int speed = random(500, 1500);
        hands[hand].moveTo(BOTTOM_LEFT, 5, clockwise, speed);
    }

    timer.in(8 * 1000, showTime);
}

// TIME
bool showTime(void *)
{
    stopClocking = false;
    setDigitTo(0, timeClient.getHours() / 10);
    setDigitTo(1, timeClient.getHours() % 10);
    setDigitTo(2, timeClient.getMinutes() / 10);
    setDigitTo(3, timeClient.getMinutes() % 10);
}

// YES

bool doYes()
{
    stopClocking = true;

    setDigitTo(3, R, true);
    setDigitTo(0, N, true);

    setDigitTo(2, O, true);
    setDigitTo(1, O, true);

    delay(5000);
    setDigitTo(1, IL, true);
    setDigitTo(2, LL, true);

    setDigitTo(3, L2, true);
    setDigitTo(0, W, true);

    delay(5000);

    setDigitTo(1, O, true);
    setDigitTo(2, U, true);

    setDigitTo(3, 10, true);
    setDigitTo(0, Y2, true);

    delay(5000);

    setDigitTo(3, Y2, true);
    setDigitTo(0, M, true);

    setDigitTo(1, A, true);
    setDigitTo(2, R, true);

    delay(5000);

    setDigitTo(0, M, true);
    setDigitTo(2, Q, true);

    setDigitTo(3, 10, true);
    setDigitTo(1, E, true);
}

/*
  Web Server Stuff
*/

void handleWebServer()
{
    WiFiClient client = server.available(); // listen for incoming clients

    if (client)
    {
        Serial.println(timeClient.getFormattedTime());
        // if you get a client,
        Serial.println("new client"); // print a message out the serial port
        String currentLine = "";      // make a String to hold incoming data from the client
                                      // make a String to hold incoming data from the client
        while (client.connected())
        { // loop while the client's connected
            if (client.available())
            {                           // if there's bytes to read from the client,
                char c = client.read(); // read a byte, then
                // Serial.write(c);        // print it out the serial monitor
                if (c == '\n')
                { // if the byte is a newline character

                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:application/json");
                        client.println("Access-Control-Allow-Origin:*");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.print(response);

                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }

                if (currentLine.endsWith("GET /probe"))
                    handleProbe();

                if (currentLine.endsWith("GET /gethall"))
                    handleGetHall();

                if (currentLine.endsWith("GET /test"))
                    handleTest();

                if (currentLine.endsWith("GET /reset"))
                    handleReset();

                if (currentLine.endsWith("GET /pause"))
                    handlePause();

                if (currentLine.endsWith("GET /spin"))
                    handleSpin();

                if (currentLine.endsWith("GET /wave"))
                    handleWave();

                if (currentLine.endsWith("GET /hypno"))
                    handleHypno();

                if (currentLine.endsWith("GET /square"))
                    handleSquare();

                if (currentLine.endsWith("GET /scissors"))
                    handleScissors();

                if (currentLine.endsWith("GET /random"))
                    handleRandom();

                if (currentLine.endsWith("GET /showtime"))
                    handleShowTime();

                if (currentLine.endsWith("GET /yes"))
                    handleYes();

                if (currentLine.endsWith(":sethand"))
                {
                    byte hand = currentLine.substring(5, 7).toInt();
                    int pos = currentLine.substring(8, 12).toInt();
                    byte extraTurns = currentLine.substring(13, 14).toInt();
                    byte clockwise = currentLine.substring(15, 16).toInt();
                    int speed = currentLine.substring(17, 21).toInt();

                    // Serial.println((String) "hand:" + hand + ", pos:" + pos + ", extraTurns:" + extraTurns + ", clockwise:" + clockwise + ", speed:" + speed);

                    hands[hand].moveTo(pos, extraTurns, clockwise, speed);
                    response += "{}";
                }

                if (currentLine.endsWith(":sethall"))
                {
                    Serial.println("substring:");
                    Serial.println(currentLine);

                    byte hand = currentLine.substring(5, 7).toInt();
                    int pos = currentLine.substring(8, 12).toInt();

                    Serial.println((String) "hand:" + hand + ", pos:" + pos);

                    hands[hand].setHallPos(pos);
                    response += "{}";
                }
            }
        }
        // close the connection:
        client.stop();
        Serial.println("client disconnected");
        response = "";
    }
}

void printWifiStatus()
{
    // print the SSID of the network you're attached to:
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.print(rssi);
    Serial.println(" dBm");
    // print where to go in a browser:
    Serial.print("To see this page in action, open a browser to http://");
    Serial.println(ip);
}

void handleGetHall()
{
    String temp = "[";

    for (byte hand = 0; hand < HAND_COUNT; hand++)
    {
        hands[hand].refreshData();
        if (hand != 0)
        {
            temp += ",";
        }
        temp += "[" + (String)hands[hand].position + "," + hands[hand].calibration + "]";
    }

    temp += "]";

    response += temp;
}

void handleProbe()
{
    Serial.println("Probing...");

    byte count = 0;
    String json = "[";
    for (byte i = 1; i < 13; i++)
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
        }          // end of good response
        delay(10); // give devices time to recover
    }              // end of for loop

    json = json + "]";

    // json = json + "1,2,3,4,5,6,7,8,9,10,11,12]";

    response += json;

    Serial.println("Done probing.");
}

void handleTest()
{
    Serial.println("doing test...");

    setUpHypno();
    response += "{}";
}

void handleSpin()
{
    Serial.println("doing spin...");

    for (byte hand = 0; hand < HAND_COUNT; hand++)
    {
        hands[hand].moveTo(BOTTOM, 5, CLOCKWISE, 1000);
    }

    response += "{}";
}

void handleReset()
{
    setDigitTo(0, 10);
    setDigitTo(1, 10);
    setDigitTo(2, 10);
    setDigitTo(3, 10);

    response += "{}";
}

void handlePause()
{
    stopClocking = true;

    response += "{}";
}

void handleWave()
{
    setUpWave();

    response += "{}";
}

void handleHypno()
{
    setUpHypno();

    response += "{}";
}

void handleSquare()
{
    setUpSquare();

    response += "{}";
}

void handleScissors()
{
    setUpScissors();

    response += "{}";
}

void handleRandom()
{
    setUpRandom();

    response += "{}";
}

void handleShowTime()
{
    showTime(0);

    response += "{}";
}

void handleYes()
{
    doYes();

    response += "{}";
}