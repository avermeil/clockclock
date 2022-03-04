#include <Wire.h>
#include "./Motor.h"

// tester board:
// Motor stepper1(6, 7, A0, true, 1270);
// Motor stepper2(4, 5, A1, false, 1230);
// Motor stepper3(3, 2, A2, false, 0);
// Motor stepper4(8, 9, A2, false, 0);

// First prod board
Motor steppers[4] = {
    Motor(5, 4, A3, false, 500, false),
    Motor(7, 6, A2, true, 511, true),
    Motor(3, 2, A7, true, 522, false),
    Motor(9, 8, A6, false, 524, true)};

void setup()
{
    Wire.begin(2);

    Wire.onReceive(receiveEvent);

    Serial.begin(115200);
    Serial.println(F("done setup()"));
    // steppers[0].init();
    // steppers[1].init();
    // steppers[2].init();
    // steppers[3].init();
}

void loop()
{
    for (byte i = 0; i < 4; i++)
    {
        if (!steppers[i].initialised && steppers[i].readyToInit)
        {
            steppers[i].init();
        }
    }

    steppers[0].loop();
    steppers[1].loop();
    steppers[2].loop();
    steppers[3].loop();

    if (Serial.available() > 0)
    {
        String command = Serial.readStringUntil('\n');
        int pos = command.substring(0, 1).toInt();
        bool clockwise = !command.substring(1).toInt();

        // stepper1.setTargetPos(pos * 540, 0, clockwise);
        // stepper2.setTargetPos(pos * 540, 0, clockwise);
    }
}

void receiveEvent(int howMany)
{
    Serial.println(F("--------- NEW COMMAND:"));

    byte command = Wire.read();
    byte hand = Wire.read();
    Serial.print(F("command:"));
    Serial.println(command);
    Serial.print(F("hand:"));
    Serial.println(hand);

    if (command == 0)
    {
        byte lowMagnetPos = Wire.read();
        byte highMagnetPos = Wire.read();
        int magnetPos = bytesToInt(lowMagnetPos, highMagnetPos);
        Serial.print(F("hallPos:"));
        Serial.println(magnetPos);

        //  steppers[hand].setTargetPos(0, 1, true);
        steppers[hand].magnetPosition = magnetPos;

        steppers[hand].readyToInit = true;
    }

    if (command == 1)
    {
        byte lowPos = Wire.read();
        byte highPos = Wire.read();
        byte extraTurns = Wire.read();
        byte clockwise = Wire.read();
        int pos = bytesToInt(lowPos, highPos);
        Serial.print(F("target Pos:"));
        Serial.println(pos);
        Serial.print(F("clockwise:"));
        Serial.println(clockwise);
        Serial.print(F("extraTurns:"));
        Serial.println(extraTurns);
        steppers[hand].setTargetPos(pos, extraTurns, clockwise);
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
