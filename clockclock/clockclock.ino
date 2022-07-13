#include <Wire.h>
#include <EEPROM.h>
#include <arduino-timer.h>
#include "./Motor.h"

// tester board:
// Motor stepper1(6, 7, A0, true, 1270);
// Motor stepper2(4, 5, A1, false, 1230);
// Motor stepper3(3, 2, A2, false, 0);
// Motor stepper4(8, 9, A2, false, 0);

// Board 1
// Motor steppers[4] = {
//    Motor(5, 4, A3, false, 500, false),
//    Motor(7, 6, A2, true, 511, false),
//    Motor(3, 2, A7, true, 522, false),
//    Motor(9, 8, A6, false, 524, false)};

// Board 1 pcb
Motor steppers[4] = {
    Motor(5, 4, A3, false, 504, false),
    Motor(7, 6, A2, true, 495, false),
    Motor(3, 2, A7, true, 510, false),
    Motor(9, 8, A6, false, 515, false)};

// Board 2
// Motor steppers[4] = {
//     Motor(5, 4, A3, false, 524, false),
//     Motor(7, 6, A2, true, 476, false),
//     Motor(3, 2, A7, true, 500, false),
//     Motor(9, 8, A6, false, 515, false)};

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
        Serial.print(F("saved calibration: "));
        Serial.println(savedCalibration);
        if (savedCalibration == -1)
        {
            savedCalibration = 1300; // set a reasonable default
            EEPROM.put(flashAddress, savedCalibration);
        }

        steppers[i].init(savedCalibration);
    }

    Serial.println(F("done setup()"));
}

void loop()
{
    steppers[0].loop();
    steppers[1].loop();
    steppers[2].loop();
    steppers[3].loop();

    // if (Serial.available() > 0)
    // {
    //     String command = Serial.readStringUntil('\n');
    //     int pos = command.substring(0, 1).toInt();
    //     bool clockwise = !command.substring(1).toInt();

    //     // stepper1.setTargetPos(pos * 540, 0, clockwise);
    //     // stepper2.setTargetPos(pos * 540, 0, clockwise);
    // }
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
    // Serial.println(F("--------- NEW COMMAND:"));

    byte command = Wire.read();
    byte hand = Wire.read();
    // Serial.print(F("command:"));
    // Serial.println(command);
    // Serial.print(F("hand:"));
    // Serial.println(hand);

    if (command == 0)
    {
        byte lowHallPos = Wire.read();
        byte highHallPos = Wire.read();
        int hallPos = bytesToInt(lowHallPos, highHallPos);
        // Serial.print(F("hallPos:"));
        // Serial.println(hallPos);

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
        int pos = bytesToInt(lowPos, highPos);
        int speed = bytesToInt(speedLowPos, speedHighPos);
        steppers[hand].setTargetPos(pos, extraTurns, clockwise, speed);
    }
}

int bytesToInt(byte low, byte high)
{
    return ((high & 0xFF) << 8) | (low & 0xFF);
}
// void showZero()
// {
//     clock_1.setHands(bottom, right);
//     clock_2.setHands(bottom, left);
//     clock_3.setHands(top, bottom);
//     clock_4.setHands(top, bottom);
//     clock_5.setHands(top, right);
//     clock_6.setHands(top, left);
// }

// void showOne()
// {
//     clock_1.setHands(out, out);
//     clock_2.setHands(bottom, bottom);
//     clock_3.setHands(out, out);
//     clock_4.setHands(top, bottom);
//     clock_5.setHands(out, out);
//     clock_6.setHands(top, top);
// }
// void showTwo()
// {
//     clock_1.setHands(right, right);
//     clock_2.setHands(left, bottom);
//     clock_3.setHands(bottom, right);
//     clock_4.setHands(top, left);
//     clock_5.setHands(top, right);
//     clock_6.setHands(left, left);
// }

// void showThree()
// {
//     clock_1.setHands(right, right);
//     clock_2.setHands(left, bottom);
//     clock_3.setHands(right, right);
//     clock_4.setHands(top, left);
//     clock_5.setHands(right, right);
//     clock_6.setHands(top, left);
// }

// void showFour()
// {
//     clock_1.setHands(bottom, bottom);
//     clock_2.setHands(bottom, bottom);
//     clock_3.setHands(top, right);
//     clock_4.setHands(top, bottom);
//     clock_5.setHands(out, out);
//     clock_6.setHands(top, top);
// }

// void showFive()
// {
//     clock_1.setHands(right, bottom);
//     clock_2.setHands(left, left);
//     clock_3.setHands(top, right);
//     clock_4.setHands(left, bottom);
//     clock_5.setHands(right, right);
//     clock_6.setHands(top, left);
// }

// void showSix()
// {
//     clock_1.setHands(right, bottom);
//     clock_2.setHands(left, left);
//     clock_3.setHands(top, bottom);
//     clock_4.setHands(bottom, left);
//     clock_5.setHands(right, top);
//     clock_6.setHands(top, left);
// }

// void showSeven()
// {
//     clock_1.setHands(right, right);
//     clock_2.setHands(left, bottom);
//     clock_3.setHands(out, out);
//     clock_4.setHands(top, bottom);
//     clock_5.setHands(out, out);
//     clock_6.setHands(top, top);
// }

// void showEight()
// {
//     clock_1.setHands(right, bottom);
//     clock_2.setHands(left, bottom);
//     clock_3.setHands(right, bottom);
//     clock_4.setHands(left, bottom);
//     clock_5.setHands(right, top);
//     clock_6.setHands(top, left);
// }

// void showNine()
// {
//     clock_1.setHands(right, bottom);
//     clock_2.setHands(left, bottom);
//     clock_3.setHands(top, right);
//     clock_4.setHands(top, bottom);
//     clock_5.setHands(right, right);
//     clock_6.setHands(top, left);
// }

// void showNull()
// {
//     clock_1.setHands(bottom, bottom);
//     clock_2.setHands(bottom, bottom);
//     clock_3.setHands(bottom, bottom);
//     clock_4.setHands(bottom, bottom);
//     clock_5.setHands(bottom, bottom);
//     clock_6.setHands(bottom, bottom);
// }
