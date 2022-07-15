
#ifndef Hand_h
#define Hand_h

#include "Arduino.h"

class Hand
{
public:
    Hand(byte _board, byte _hand_index);
    void moveTo(int handPos, byte extraTurns, bool clockwise, int speed);
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
};

#endif