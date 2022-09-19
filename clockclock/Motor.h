
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
        int hallBaseline,
        bool _hallFlipped);
  void initCalibration(int hallPos, bool force);
  void setTargetPos(int targetPos, int extraTurns, bool clockwise, int speed = 1000, int acceleration = 500);
  int getReportedPos();
  int getCalibratedPos();
  int normalisePosition(long reported);
  void calibratePosition(float sensorValue);
  bool calibrationStillValid(float sensorValue);
  void setSpeed(int stepsPerSecond);
  void setAcceleration(int stepsPerSecondPerSecond);
  FlexyStepper stepper;
  bool initialised;
  bool reverseDirection;
  int stepPin;
  int dirPin;
  int hallPin;
  int stepsOffset;
  bool isClockwise;
  int hallPosition;
  int hallBaseline;
  bool hallFlipped;
  float minHallReading;
  int minHallReadingPosition;
  byte calibrationCount;
};

#endif