#include "Arduino.h"
#include <Wire.h>
#include "hand.h"

const long SINGLE_ROTATION_STEPS = 4320;

short _bytesToInt(byte low, byte high)
{
    return ((high & 0xFF) << 8) | (low & 0xFF);
}

Hand::Hand(byte _board, byte _hand_index)
{
    board = _board;
    hand_index = _hand_index;

    position = 0;
    calibration = 0;
    isClockwise = false;
}

void Hand::refreshData()
{
    Wire.requestFrom(board, 20);

    for (byte i = 0; i < 4; i++)
    {
        int _calibration = _bytesToInt(Wire.read(), Wire.read());
        int _position = _bytesToInt(Wire.read(), Wire.read());
        bool _isClockwise = Wire.read();

        if (i == hand_index)
        {
            position = _position;
            calibration = _calibration;
            isClockwise = _isClockwise;
        }
    }
}

void Hand::moveTo(int handPos, byte extraTurns, bool clockwise, int speed)
{
    Serial.println((String) "sending new handPos... board:" + board + ", hand_index:" + hand_index + ", handPos:" + handPos + ", extraTurns:" + extraTurns + ", clockwise:" + clockwise + ", speed:" + speed);

    Wire.beginTransmission(board);

    Wire.write(1); // command
    Wire.write(hand_index);

    Wire.write(lowByte(handPos));
    Wire.write(highByte(handPos));

    Wire.write(extraTurns);
    Wire.write(clockwise);

    Wire.write(lowByte(speed));
    Wire.write(highByte(speed));

    Wire.endTransmission();
}