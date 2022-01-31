#include "Arduino.h"
#include "Motor.h"
#include "FlexyStepperLocal.h"

const int MIN_INT = -32768;
const int SINGLE_ROTATION_STEPS = 4320;

Motor::Motor(int _stepPin, int _dirPin, int _hallPin, bool _reverseDirection, int _magnetPosition)
{
    stepsOffset = 0;
    isClockwise = true;
    magnetPosition = _magnetPosition; //1230;
    calibrationBlockedUntilStep = MIN_INT;
    stepPin = _stepPin;
    dirPin = _dirPin;
    hallPin = _hallPin;
    reverseDirection = _reverseDirection;
    stepper2.connectToPins(stepPin, dirPin, !reverseDirection);
}

void Motor::init()
{
    // go fast for initial calibration
    stepper2.setSpeedInStepsPerSecond(3000);
    stepper2.setAccelerationInStepsPerSecondPerSecond(2000);

    stepper2.setTargetPositionRelativeInSteps(SINGLE_ROTATION_STEPS * 1);

    while (!stepper2.motionComplete())
    {
        stepper2.processMovement();
        calibratePosition();
    }

    // set speeds
    stepper2.setSpeedInStepsPerSecond(1000);
    stepper2.setAccelerationInStepsPerSecondPerSecond(500);
    setTargetPos(0, 0, true);
}

void Motor::setTargetPos(int targetPos, int extraTurns, bool clockwise)
{
    isClockwise = clockwise;
    int reportedPos = getReportedPos();

    int currentPos = reportedPos - stepsOffset;
    int stepsToMake = 0;
    int extraSteps = extraTurns * SINGLE_ROTATION_STEPS;

    if (currentPos < targetPos)
    {
        stepsToMake = targetPos - currentPos + extraSteps;
    }
    else if (currentPos > targetPos)
    {
        stepsToMake = SINGLE_ROTATION_STEPS + targetPos - currentPos + extraSteps;
    }

    if (!clockwise)
    {
        stepsToMake = stepsToMake - SINGLE_ROTATION_STEPS - (extraSteps * 2);
    }

    // Serial.print("clockwise: ");
    // Serial.println(clockwise);
    // Serial.print("targetPos: ");
    // Serial.println(targetPos);
    // Serial.print("stepsOffset: ");
    // Serial.println(stepsOffset);
    // Serial.print("currentPos: ");
    // Serial.println(currentPos);
    // Serial.print("stepsToMake: ");
    // Serial.println(stepsToMake);
    stepper2.setTargetPositionRelativeInSteps(stepsToMake);
}

int Motor::getReportedPos()
{
    int reportedPos = stepper2.getCurrentPositionInSteps();
    if (reportedPos >= 0)
    {
        return stepper2.getCurrentPositionInSteps() % SINGLE_ROTATION_STEPS;
    }
    // 360 + (-450 % 360) = 270
    return SINGLE_ROTATION_STEPS + (stepper2.getCurrentPositionInSteps() % SINGLE_ROTATION_STEPS);
}

void Motor::calibratePosition()
{
    int sensorValue2 = analogRead(hallPin);
    int currentPos = getReportedPos();
    if (calibrationBlockedUntilStep < currentPos)
    {
        if (sensorValue2 < 500 && isClockwise)
        {
            calibrationBlockedUntilStep = currentPos + 500;

            stepsOffset = currentPos - magnetPosition;

            // Serial.print("entered the zone at ");
            // Serial.print(currentPos);
            // Serial.print(" and blocking until ");
            // Serial.println(calibrationBlockedUntilStep);
        }
        else
        {
            // Serial.print("blocked at ");
            // Serial.println(currentPos);
            calibrationBlockedUntilStep = MIN_INT;
        }
    }
}

void Motor::loop()
{
    stepper2.processMovement();
    calibratePosition();
}