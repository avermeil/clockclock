
#ifndef Hand_h
#define Hand_h

#include "Arduino.h"

class Hand
{
public:
    Hand(byte _board, byte _hand_index);
    void moveTo(int handPos, byte extraTurns, bool clockwise, int speed);
    void refreshData();
    byte board;
    byte hand_index;
    int position;
    int calibration;
    bool isClockwise;
};

#endif