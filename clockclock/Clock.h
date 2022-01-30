/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef Clock_h
#define Clock_h

#include "Arduino.h"
#include "Motor.h"
 
class Clock
{
    public:
        Clock();
        int doStep();
        int setHands(int hour, int minute);
        int doSetHands(int hour, int minute);
        Motor motor_hour, motor_minute;

};

#endif