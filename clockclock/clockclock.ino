#include <Wire.h>
#include <EEPROM.h>
#include <arduino-timer.h>
#include <Ewma.h>
#include "./Motor.h"

Motor steppers[4] = {
    Motor(3, 2, A7, true, 510, false),
    Motor(9, 8, A6, false, 515, false),
    Motor(5, 4, A3, false, 504, false),
    Motor(7, 6, A2, true, 495, false)};

Ewma filters[4] = {
    Ewma(0.01),
    Ewma(0.01),
    Ewma(0.01),
    Ewma(0.01)};

int I2C_ADDRESS = 1;

void setup()
{
    CLKPR = 0x80; // (1000 0000) enable change in clock frequency
    CLKPR = 0x01; // (0000 0001) use clock division factor 2 to reduce the frequency from 16 MHz to 8 MHz

    randomSeed(analogRead(0));
    Wire.begin(I2C_ADDRESS);

    Wire.onReceive(receiveEvent);
    Wire.onRequest(requestEvent);

    Serial.begin(115200);

    // delay for random amount of time
    int randomNum = random(500);
    delay(randomNum);

    for (byte i = 0; i < 4; i++)
    {
        int flashAddress = i * 2; // two bytes per int
        int savedCalibration = 0;
        EEPROM.get(flashAddress, savedCalibration);

        if (savedCalibration == -1)
        {
            savedCalibration = 1300; // set a reasonable default
            EEPROM.put(flashAddress, savedCalibration);
        }

        steppers[i].init(savedCalibration);
    }
}

void loop()
{
    for (byte i = 0; i < 4; i++)
    {
        if (i == 0 && !steppers[1].initialised)
        {
            continue;
        }
        if (i == 2 && !steppers[3].initialised)
        {
            continue;
        }

        bool stepped = steppers[i].stepper.processMovementAlt();

        if (steppers[i].initialised)
        {
            filters[i].reset();
        }
        else if (stepped)
        {

            float filtered = filters[i].filter(analogRead(steppers[i].hallPin));
            steppers[i].calibratePosition(filtered);
        }
    }
}

void requestEvent()
{
    for (byte i = 0; i < 4; i++)
    {
        // Hall position calibration
        Wire.write(lowByte(steppers[i].hallPosition));
        Wire.write(highByte(steppers[i].hallPosition));

        // Hand position
        Wire.write(lowByte(steppers[i].getCalibratedPos()));
        Wire.write(highByte(steppers[i].getCalibratedPos()));

        // isClockwise
        Wire.write(steppers[i].isClockwise);
    }
}

void receiveEvent(int howMany)
{

    byte command = Wire.read();
    byte hand = Wire.read();

    if (command == 0)
    {
        byte lowHallPos = Wire.read();
        byte highHallPos = Wire.read();
        int hallPos = bytesToInt(lowHallPos, highHallPos);

        EEPROM.put(hand * 2, hallPos);

        steppers[hand].init(hallPos);
    }

    if (command == 1)
    {
        byte lowPos = Wire.read();
        byte highPos = Wire.read();
        byte extraTurns = Wire.read();
        byte clockwise = Wire.read();
        byte speedLowPos = Wire.read();
        byte speedHighPos = Wire.read();
        byte accelLowPos = Wire.read();
        byte accelHighPos = Wire.read();
        int pos = bytesToInt(lowPos, highPos);
        int speed = bytesToInt(speedLowPos, speedHighPos);
        int accel = bytesToInt(accelLowPos, accelHighPos);

        for (byte i = 0; i < 4; i++)
        {
            if (!steppers[i].initialised)
            {
                return;
            }
        }

        steppers[hand].setTargetPos(pos, extraTurns, clockwise, speed, accel);
    }
}

int bytesToInt(byte low, byte high)
{
    return ((high & 0xFF) << 8) | (low & 0xFF);
}
