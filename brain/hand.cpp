#include "Arduino.h"
#include <Wire.h>
#include "hand.h"

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

    hasLooseHourHand = (board == 3 && handIndex == 3) || (board == 11 && handIndex == 1);
}

void Hand::refreshData()
{

    Wire.requestFrom(board, 20);

    for (byte i = 0; i < 4; i++)
    {
        int _calibration = _bytesToInt(Wire.read(), Wire.read());
        int _position = _bytesToInt(Wire.read(), Wire.read());
        bool _isClockwise = Wire.read();

        if (i == handIndex)
        {
            position = _position;
            calibration = _calibration;
            isClockwise = _isClockwise;
        }
    }
}

void Hand::moveTo(int handPos, byte extraTurns, byte mode, int speed)
{
    // Serial.println((String) "sending new handPos... board:" + board + ", handIndex:" + handIndex + ", handPos:" + handPos + ", extraTurns:" + extraTurns + ", mode:" + mode + ", speed:" + speed);

    if (speed <= 20 || mode == FASTEST)
    {
        // In these cases, we need to know where the hand is currently.
        refreshData();
    }

    bool clockwise = false;

    if (mode == CLOCKWISE)
    {
        clockwise = true;
    }
    else if (mode == ANTI_CLOCKWISE)
    {
        clockwise = false;
    }
    else if (mode == MAINTAIN)
    {
        clockwise = isClockwise;
    }
    else if (mode == FASTEST)
    {
        int clockwiseStepsToMake = 0;
        int antiClockwiseStepsToMake = 0;

        int currentPos = position;
        int targetPos = handPos;

        if (currentPos < targetPos)
        {
            clockwiseStepsToMake = targetPos - currentPos;
        }
        else if (currentPos > targetPos)
        {
            clockwiseStepsToMake = SINGLE_ROTATION_STEPS + targetPos - currentPos;
        }

        if (clockwiseStepsToMake == 0)
        {
            return;
        }

        antiClockwiseStepsToMake = clockwiseStepsToMake - SINGLE_ROTATION_STEPS;

        if (-antiClockwiseStepsToMake > clockwiseStepsToMake)
        {
            clockwise = true;
        }
    }

    // If the speed is less than 20, then interpret as number of seconds of movement wanted.
    if (speed <= 20)
    {
        int stepsToMake = 0;

        int currentPos = position;
        int targetPos = handPos;

        if (currentPos < targetPos)
        {
            stepsToMake = targetPos - currentPos;
        }
        else if (currentPos > targetPos)
        {
            stepsToMake = SINGLE_ROTATION_STEPS + targetPos - currentPos;
        }

        if (!clockwise && stepsToMake != 0)
        {
            stepsToMake = stepsToMake - SINGLE_ROTATION_STEPS;
        }

        if (stepsToMake == 0)
        {
            return;
        }

        int minSteps = 2000;

        // if we only need to move a little bit, add an extra turn
        if (clockwise && stepsToMake < minSteps)
        {
            stepsToMake += SINGLE_ROTATION_STEPS;
            extraTurns += 1;
        }

        if (!clockwise && stepsToMake > -minSteps)
        {
            stepsToMake -= SINGLE_ROTATION_STEPS;
            extraTurns += 1;
        }

        int seconds = speed - 2; // 2 extra consumed by acceleration and deceleration.

        speed = abs(stepsToMake) / seconds;
    }

    if (!isMinute)
    {
        // Some hands are super loose and will "fall" ahead of their position while going down.
        // We need to compensate for this.
        if (hasLooseHourHand)
        {
            if (clockwise && (handPos == RIGHT || handPos == BOTTOM_RIGHT || handPos == TOP_RIGHT))
            {
                handPos -= 30;
            }
            if (!clockwise && (handPos == RIGHT || handPos == BOTTOM_LEFT || handPos == TOP_LEFT))
            {
                handPos += 30;
            }
        }
        // we need to compensate for the play in the gears for the non-loose minute hands
        else if (clockwise)
        {
            handPos += 30;

            if (handPos >= SINGLE_ROTATION_STEPS)
            {
                handPos -= SINGLE_ROTATION_STEPS;
            }
        }
    }

    int acceleration = speed / 2;

    if (acceleration < 300)
    {
        acceleration = 300;
    }

    /*
        Todo:
        - when going from a high speed to a lower speed, keep acceleration relatively high to allow the hand to slow down in time.

        - when in seconds-mode, add an extra turn if the hand doesn't have time to decelerate to the target position.

        - if the hand is already moving, don't set a very low acceleration.

    */

    Serial.println((String) "sending new handPos... board:" + board + ", handIndex:" + handIndex + ", handPos:" + handPos + ", extraTurns:" + extraTurns + ", mode:" + mode + ", clockwise:" + clockwise + ", speed:" + speed);

    Wire.beginTransmission(board);

    Wire.write(1); // command
    Wire.write(handIndex);

    Wire.write(lowByte(handPos));
    Wire.write(highByte(handPos));

    Wire.write(extraTurns);
    Wire.write(clockwise);

    Wire.write(lowByte(speed));
    Wire.write(highByte(speed));

    Wire.write(lowByte(acceleration));
    Wire.write(highByte(acceleration));

    Wire.endTransmission();

    isClockwise = clockwise;
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

    if (symbol == W)
    {
        if (segment == 1)
            return combo(BOTTOM, BOTTOM);
        if (segment == 2)
            return combo(BOTTOM, BOTTOM);
        if (segment == 3)
            return combo(BOTTOM, TOP);
        if (segment == 4)
            return combo(BOTTOM, TOP);
        if (segment == 5)
            return combo(TOP, TOP_RIGHT);
        if (segment == 6)
            return combo(TOP, TOP_LEFT);
    }

    if (symbol == I)
    {
        if (segment == 2)
            return BOTTOM;
        if (segment == 4)
            return combo(TOP, BOTTOM);
        if (segment == 6)
            return TOP;
    }

    if (symbol == L)
    {
        if (segment == 1)
            return combo(BOTTOM, BOTTOM);
        if (segment == 3)
            return combo(BOTTOM, TOP);
        if (segment == 5)
            return combo(TOP, RIGHT);
        if (segment == 6)
            return combo(LEFT, LEFT);
    }

    if (symbol == IL)
    {
        if (segment == 1)
            return combo(BOTTOM, BOTTOM);
        if (segment == 2)
            return combo(BOTTOM, BOTTOM);
        if (segment == 3)
            return combo(BOTTOM, TOP);
        if (segment == 4)
            return combo(BOTTOM, TOP);
        if (segment == 5)
            return combo(TOP, TOP);
        if (segment == 6)
            return combo(TOP, RIGHT);
    }

    if (symbol == LL)
    {
        if (segment == 2)
            return combo(BOTTOM, BOTTOM);
        if (segment == 4)
            return combo(BOTTOM, TOP);
        if (segment == 5)
            return combo(LEFT, LEFT);
        if (segment == 6)
            return combo(TOP, RIGHT);
    }

    if (symbol == L2)
    {
        if (segment == 5)
            return combo(LEFT, LEFT);
    }

    if (symbol == Y)
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

    if (symbol == Y2)
    {
        if (segment == 1)
            return BOTTOM_RIGHT;
        if (segment == 2)
            return BOTTOM;
        if (segment == 4)
            return combo(TOP_LEFT, BOTTOM);
        if (segment == 6)
            return TOP;
    }

    if (symbol == O)
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

    if (symbol == U)
    {
        if (segment == 1)
            return combo(BOTTOM, BOTTOM);
        if (segment == 2)
            return combo(BOTTOM, BOTTOM);
        if (segment == 3)
            return combo(BOTTOM, TOP);
        if (segment == 4)
            return combo(BOTTOM, TOP);
        if (segment == 5)
            return combo(TOP, RIGHT);
        if (segment == 6)
            return combo(TOP, LEFT);
    }

    if (symbol == M)
    {
        if (segment == 1)
            return combo(BOTTOM, BOTTOM_RIGHT);
        if (segment == 2)
            return combo(BOTTOM, BOTTOM_LEFT);
        if (segment == 3)
            return combo(BOTTOM, TOP);
        if (segment == 4)
            return combo(BOTTOM, TOP);
        if (segment == 5)
            return combo(TOP, TOP);
        if (segment == 6)
            return combo(TOP, TOP);
    }

    if (symbol == A)
    {
        if (segment == 1)
            return combo(BOTTOM, RIGHT);
        if (segment == 2)
            return combo(BOTTOM, LEFT);
        if (segment == 3)
            return combo(BOTTOM, RIGHT);
        if (segment == 4)
            return combo(BOTTOM, LEFT);
        if (segment == 5)
            return combo(TOP, TOP);
        if (segment == 6)
            return combo(TOP, TOP);
    }

    if (symbol == R)
    {
        if (segment == 1)
            return combo(BOTTOM, RIGHT);
        if (segment == 2)
            return combo(BOTTOM, LEFT);
        if (segment == 3)
            return combo(BOTTOM, TOP);
        if (segment == 4)
            return combo(BOTTOM, TOP_LEFT);
        if (segment == 5)
            return combo(TOP, TOP);
        if (segment == 6)
            return combo(TOP, TOP);
    }

    if (symbol == E)
    {
        if (segment == 1)
            return combo(BOTTOM, RIGHT);
        if (segment == 2)
            return combo(LEFT, LEFT);
        if (segment == 3)
            return combo(TOP, RIGHT);
        if (segment == 4)
            return combo(LEFT, LEFT);
        if (segment == 5)
            return combo(TOP, RIGHT);
        if (segment == 6)
            return combo(LEFT, LEFT);
    }

    if (symbol == Q)
    {
        if (segment == 1)
            return combo(RIGHT, RIGHT);
        if (segment == 2)
            return combo(BOTTOM, LEFT);
        if (segment == 4)
            return combo(TOP, BOTTOM_LEFT);
        if (segment == 6)
            return combo(LEFT, LEFT);
    }

    if (symbol == N)
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
            return combo(TOP, TOP);
        if (segment == 6)
            return combo(TOP, TOP);
    }

    return BOTTOM_LEFT;
}