
#ifndef Motor_h
#define Motor_h

#include "Arduino.h"
#include "FlexyStepperLocal.h"

class Motor
{
public:
  Motor(int _stepPin,
        int _dirPin,
        int _hallPin,
        bool _reverseDirection,
        int _magnetBaseline,
        bool _magnetFlipped);
  void init();
  void setTargetPos(int targetPos, int extraTurns, bool clockwise);
  int getReportedPos();
  int normalisePosition(int reported);
  void calibratePosition();
  void setSpeed(int stepsPerSecond);
  void loop();
  FlexyStepper stepper;
  bool reverseDirection;
  int stepPin;
  int dirPin;
  int hallPin;
  int stepsOffset;
  bool isClockwise;
  int magnetPosition;
  int hallBaseline;
  bool hallFlipped;
  int calibrationBlockedUntilStep;
};

#endif