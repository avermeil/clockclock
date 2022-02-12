
#ifndef Motor_h
#define Motor_h

#include "Arduino.h"
#include "FlexyStepperLocal.h"

class Motor
{
public:
  Motor(int _stepPin, int _dirPin, int _hallPin, bool _reverseDirection, int _magnetPosition);
  void init();
  void setTargetPos(int targetPos, int extraTurns, bool clockwise);
  int getReportedPos();
  void calibratePosition();
  void loop();
  FlexyStepper stepper2;
  bool reverseDirection;
  int stepPin;
  int dirPin;
  int hallPin;
  int stepsOffset;
  bool isClockwise;
  int magnetPosition;
  int calibrationBlockedUntilStep;
};

#endif