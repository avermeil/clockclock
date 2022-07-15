#include "Arduino.h"
#include <Wire.h>
#include "hand.h"

const int SINGLE_ROTATION_STEPS = 4320;
const int BOTTOM = 0;
const int BOTTOM_LEFT = SINGLE_ROTATION_STEPS * 1 / 8;
const int LEFT = SINGLE_ROTATION_STEPS * 2 / 8;
const int TOP_LEFT = SINGLE_ROTATION_STEPS * 3 / 8;
const int TOP = SINGLE_ROTATION_STEPS * 4 / 8;
const int TOP_RIGHT = SINGLE_ROTATION_STEPS * 5 / 8;
const int RIGHT = SINGLE_ROTATION_STEPS * 6 / 8;
const int BOTTOM_RIGHT = SINGLE_ROTATION_STEPS * 7 / 8;

short _bytesToInt(byte low, byte high)
{
    return ((high & 0xFF) << 8) | (low & 0xFF);
}

Hand::Hand(byte _board, byte _handIndex)
{
    board = _board;
    handIndex = _handIndex;

    position = 0;
    calibration = 0;
    isClockwise = false;

    isMinute = handIndex % 2 == 0;

    byte boardDigitIndex = (board - 1) % 3;

    if (boardDigitIndex == 0 && (handIndex == 0 || handIndex == 1))
        digitIndex = 0;
    if (boardDigitIndex == 0 && (handIndex == 2 || handIndex == 3))
        digitIndex = 1;
    if (boardDigitIndex == 1 && (handIndex == 0 || handIndex == 1))
        digitIndex = 2;
    if (boardDigitIndex == 1 && (handIndex == 2 || handIndex == 3))
        digitIndex = 3;
    if (boardDigitIndex == 2 && (handIndex == 0 || handIndex == 1))
        digitIndex = 4;
    if (boardDigitIndex == 2 && (handIndex == 2 || handIndex == 3))
        digitIndex = 5;
}

void Hand::refreshData()
{
    // Serial.println("Refreshing data...");
    long start = micros();

    Wire.requestFrom(board, 20);
    // Serial.println("Sent request for 20 bytes");

    for (byte i = 0; i < 4; i++)
    {
        int _calibration = _bytesToInt(Wire.read(), Wire.read());
        // Serial.println((String) "got calibration for hand " + i + ": " + _calibration);

        int _position = _bytesToInt(Wire.read(), Wire.read());
        // Serial.println((String) "got position for hand " + i + ": " + _position);

        bool _isClockwise = Wire.read();
        // Serial.println((String) "got clockwise for hand " + i + ": " + _isClockwise);

        if (i == handIndex)
        {
            position = _position;
            calibration = _calibration;
            isClockwise = _isClockwise;
        }
    }

    long duration = micros() - start;
    // Serial.println("Took micros:");
    // Serial.println(duration);
}

void Hand::moveTo(int handPos, byte extraTurns, bool clockwise, int speed)
{
    Serial.println((String) "sending new handPos... board:" + board + ", handIndex:" + handIndex + ", handPos:" + handPos + ", extraTurns:" + extraTurns + ", clockwise:" + clockwise + ", speed:" + speed);
    Wire.beginTransmission(board);

    Wire.write(1); // command
    Wire.write(handIndex);

    Wire.write(lowByte(handPos));
    Wire.write(highByte(handPos));

    Wire.write(extraTurns);
    Wire.write(clockwise);

    Wire.write(lowByte(speed));
    Wire.write(highByte(speed));

    Wire.endTransmission();
}

void Hand::setHallPos(int hallPos)
{
    // Serial.println("SENDING CALIBRATION DATA");
    Wire.beginTransmission(board);

    Wire.write(0); // command
    Wire.write(handIndex);

    Wire.write(lowByte(hallPos));
    Wire.write(highByte(hallPos));

    Wire.endTransmission();
}

int Hand::combo(int minute, int hour)
{
    if (isMinute)
        return minute;
    else
        return hour;
}

int Hand::getDigitPos(byte symbol)
{
    byte segment = digitIndex + 1;

    if (symbol == 0)
    {
        if (segment == 1)
            return combo(BOTTOM, RIGHT);
        if (segment == 2)
            return combo(BOTTOM, LEFT);
        if (segment == 3)
            return combo(BOTTOM, TOP);
        if (segment == 4)
            return combo(BOTTOM, TOP);
        if (segment == 5)
            return combo(TOP, RIGHT);
        if (segment == 6)
            return combo(TOP, LEFT);
    }

    if (symbol == 1)
    {
        if (segment == 2)
            return BOTTOM;
        if (segment == 4)
            return combo(TOP, BOTTOM);
        if (segment == 6)
            return TOP;
    }

    if (symbol == 2)
    {
        if (segment == 1)
            return RIGHT;
        if (segment == 2)
            return combo(BOTTOM, LEFT);
        if (segment == 3)
            return combo(BOTTOM, RIGHT);
        if (segment == 4)
            return combo(TOP, LEFT);
        if (segment == 5)
            return combo(TOP, RIGHT);
        if (segment == 6)
            return LEFT;
    }

    if (symbol == 3)
    {
        if (segment == 1)
            return RIGHT;
        if (segment == 2)
            return combo(BOTTOM, LEFT);
        if (segment == 3)
            return RIGHT;
        if (segment == 4)
            return combo(TOP, LEFT);
        if (segment == 5)
            return RIGHT;
        if (segment == 6)
            return combo(TOP, LEFT);
    }

    if (symbol == 4)
    {
        if (segment == 1)
            return BOTTOM;
        if (segment == 2)
            return BOTTOM;
        if (segment == 3)
            return combo(TOP, RIGHT);
        if (segment == 4)
            return combo(BOTTOM, TOP);
        if (segment == 6)
            return TOP;
    }

    if (symbol == 5)
    {
        if (segment == 1)
            return combo(BOTTOM, RIGHT);
        if (segment == 2)
            return LEFT;
        if (segment == 3)
            return combo(TOP, RIGHT);
        if (segment == 4)
            return combo(BOTTOM, LEFT);
        if (segment == 5)
            return RIGHT;
        if (segment == 6)
            return combo(TOP, LEFT);
    }

    if (symbol == 6)
    {
        if (segment == 1)
            return combo(BOTTOM, RIGHT);
        if (segment == 2)
            return LEFT;
        if (segment == 3)
            return combo(BOTTOM, TOP);
        if (segment == 4)
            return combo(BOTTOM, LEFT);
        if (segment == 5)
            return combo(TOP, RIGHT);
        if (segment == 6)
            return combo(TOP, LEFT);
    }

    if (symbol == 7)
    {
        if (segment == 1)
            return RIGHT;
        if (segment == 2)
            return combo(BOTTOM, LEFT);
        if (segment == 4)
            return combo(BOTTOM, TOP);
        if (segment == 6)
            return TOP;
    }

    if (symbol == 8)
    {
        if (segment == 1)
            return combo(BOTTOM, RIGHT);
        if (segment == 2)
            return combo(BOTTOM, LEFT);
        if (segment == 3)
            return combo(TOP, RIGHT);
        if (segment == 4)
            return combo(TOP, LEFT);
        if (segment == 5)
            return combo(TOP, RIGHT);
        if (segment == 6)
            return combo(TOP, LEFT);
    }

    if (symbol == 9)
    {
        if (segment == 1)
            return combo(BOTTOM, RIGHT);
        if (segment == 2)
            return combo(BOTTOM, LEFT);
        if (segment == 3)
            return combo(TOP, RIGHT);
        if (segment == 4)
            return combo(BOTTOM, TOP);
        if (segment == 5)
            return RIGHT;
        if (segment == 6)
            return combo(TOP, LEFT);
    }

    return BOTTOM_LEFT;
}