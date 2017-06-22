/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef Motor_h
#define Motor_h

#include "Arduino.h"
 
class Motor
{
  public:
    Motor();
    int getStepMask();
    int moveBy(int steps_to_move);
    int pin_offset;
    int step_mask;
    int current_position;
    long steps_left;
    int steps_since_start;
    long step_delay;
    int speed_mod; //is either 1 (normal) or 2 (slow)

  private:
   // int _steps[4];
    int current_step;
    unsigned long time;
    unsigned long last_step_time;

};

#endif