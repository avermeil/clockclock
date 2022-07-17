
#ifndef Hand_h
#define Hand_h

#include "Arduino.h"

const int SINGLE_ROTATION_STEPS = 4320;
const int BOTTOM = 0;
const int BOTTOM_LEFT = SINGLE_ROTATION_STEPS * 1 / 8;
const int LEFT = SINGLE_ROTATION_STEPS * 2 / 8;
const int TOP_LEFT = SINGLE_ROTATION_STEPS * 3 / 8;
const int TOP = SINGLE_ROTATION_STEPS * 4 / 8;
const int TOP_RIGHT = SINGLE_ROTATION_STEPS * 5 / 8;
const int RIGHT = SINGLE_ROTATION_STEPS * 6 / 8;
const int BOTTOM_RIGHT = SINGLE_ROTATION_STEPS * 7 / 8;

const byte ANTI_CLOCKWISE = 0;
const byte CLOCKWISE = 1;
const byte MAINTAIN = 2;
const byte FASTEST = 3;

class Hand
{
public:
    Hand(byte _board, byte _hand_index);
    void moveTo(int handPos, byte extraTurns, byte mode, int speed);
    void setHallPos(int hallPos);
    int getDigitPos(byte digit);
    void refreshData();
    int combo(int minute, int hour);
    byte board;
    byte handIndex;
    bool isMinute;
    byte digitIndex;
    int position;
    int calibration;
    bool isClockwise;
    bool hasLooseHourHand;
};

#endif