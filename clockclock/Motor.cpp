#include <EEPROM.h>
#include "Arduino.h"
#include "Motor.h"
#include "FlexyStepperLocal.h"

const long SINGLE_ROTATION_STEPS = 4320;
const byte HALL_SENSITIVITY = 5; // smaller is more sensitive

Motor::Motor(
    int _stepPin,
    int _dirPin,
    int _hallPin,
    bool _reverseDirection,
    int _hallBaseline,
    bool _hallFlipped)
{
    initialised = false;
    stepsOffset = 0;
    isClockwise = true;
    hallPosition = 0;
    calibrationCount = 0;
    stepPin = _stepPin;
    dirPin = _dirPin;
    hallPin = _hallPin;
    reverseDirection = _reverseDirection;
    hallFlipped = _hallFlipped;
    hallBaseline = _hallBaseline;

    minHallReading = 999.9;
    minHallReadingPosition = 0;

    stepper.connectToPins(stepPin, dirPin, !reverseDirection);
}

void Motor::initCalibration(int hallPos, bool force)
{
    minHallReading = 999.9;
    minHallReadingPosition = 0;

    if (!force && hallPos == hallPosition)
    {
        return;
    }

    initialised = false;
    hallPosition = hallPos;

    setTargetPos(0, 3, true, 2000, 4000);
}

void Motor::setTargetPos(int targetPos, int extraTurns, bool clockwise, int speed = 1000, int acceleration = 500)
{
    setSpeed(speed);
    setAcceleration(acceleration);

    isClockwise = clockwise;

    int currentPos = getCalibratedPos();
    long stepsToMake = 0;
    long extraSteps = extraTurns * SINGLE_ROTATION_STEPS;

    if (currentPos < targetPos)
    {
        stepsToMake = targetPos - currentPos;
    }
    else if (currentPos > targetPos)
    {
        stepsToMake = SINGLE_ROTATION_STEPS + targetPos - currentPos;
    }

    stepsToMake += extraSteps;

    if (!clockwise && stepsToMake != 0)
    {
        stepsToMake = stepsToMake - SINGLE_ROTATION_STEPS - (extraSteps * 2);
    }

    stepper.setTargetPositionRelativeInSteps(stepsToMake);
}

int Motor::getReportedPos()
{
    long reportedPos = stepper.getCurrentPositionInSteps();
    return normalisePosition(reportedPos);
}

int Motor::getCalibratedPos()
{
    int reportedPos = getReportedPos();
    return normalisePosition(reportedPos - stepsOffset);
}

int Motor::normalisePosition(long reported)
{
    if (reported >= 0)
    {
        return reported % SINGLE_ROTATION_STEPS;
    }
    // 360 + (-450 % 360) = 270
    return SINGLE_ROTATION_STEPS + (reported % SINGLE_ROTATION_STEPS);
}

void Motor::calibratePosition(float sensorValue)
{
    if (!isClockwise)
    {
        return;
    }

    if (sensorValue < minHallReading)
    {
        minHallReading = sensorValue;
        minHallReadingPosition = getReportedPos();
    }

    bool isTriggered = ((minHallReading + 3.0) < sensorValue);

    if (isTriggered)
    {
        stepsOffset = minHallReadingPosition - hallPosition;
        calibrationCount++;
        initialised = true;
        setTargetPos(SINGLE_ROTATION_STEPS * 0.75, 0, false, 3000, 4000);
    }
}

bool Motor::calibrationStillValid(float sensorValue)
{
    if (!isClockwise)
    {
        return true;
    }

    int pos = getReportedPos();

    // If the hand should be over the hall sensor...
    if (pos == minHallReadingPosition)
    {
        // ... but it's several units above where it should be, then the calibration is no longer valid
        if (sensorValue > (minHallReading + 8.0))
        {
            return false;
        }
    }

    return true;
}

void Motor::setSpeed(int stepsPerSecond)
{
    stepper.setSpeedInStepsPerSecond(stepsPerSecond);
}

void Motor::setAcceleration(int stepsPerSecondPerSecond)
{
    stepper.setAccelerationInStepsPerSecondPerSecond(stepsPerSecondPerSecond);
}
