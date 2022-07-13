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
    stepPin = _stepPin;
    dirPin = _dirPin;
    hallPin = _hallPin;
    reverseDirection = _reverseDirection;
    hallFlipped = _hallFlipped;
    hallBaseline = _hallBaseline;
    stepper.connectToPins(stepPin, dirPin, !reverseDirection);
    setSpeed(1000);
}

void Motor::init(int hallPos)
{
    unsigned long sens = 0;
    int samples = 5;
    for (int i = 0; i < samples; i++)
    {
        delay(10);
        sens += analogRead(hallPin);
    }
    hallBaseline = round(sens / samples);

    if (hallPos == hallPosition)
    {
        return;
    }

    initialised = false;
    hallPosition = hallPos;

    setTargetPos(0, 3, true);
}

void Motor::setTargetPos(int targetPos, int extraTurns, bool clockwise, int speed = 1000)
{
    setSpeed(speed);
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

    if (!clockwise)
    {
        stepsToMake = stepsToMake - SINGLE_ROTATION_STEPS - (extraSteps * 2);
    }

    // Serial.println(F("===== called setTargetPos ====="));

    // Serial.print(F("clockwise: "));
    // Serial.println(clockwise);
    // Serial.print(F("targetPos: "));
    // Serial.println(targetPos);
    // Serial.print(F("stepsOffset: "));
    // Serial.println(stepsOffset);
    // Serial.print(F("reportedPos: "));
    // Serial.println(reportedPos);
    // Serial.print(F("currentPos: "));
    // Serial.println(currentPos);
    // Serial.print(F("stepsToMake: "));
    // Serial.println(stepsToMake);
    // Serial.print(F("hallPosition: "));
    // Serial.println(hallPosition);

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

void Motor::calibratePosition()
{
    int sensorValue = analogRead(hallPin);

    bool isTriggered = hallFlipped
                           ? sensorValue > (hallBaseline + HALL_SENSITIVITY)
                           : sensorValue < (hallBaseline - HALL_SENSITIVITY);

    if (isTriggered && isClockwise)
    {
        int currentPos = getReportedPos();
        stepsOffset = currentPos - hallPosition;

        // Serial.print(F("entered the zone, stepsOffset is "));
        // Serial.println(stepsOffset);
        initialised = true;
        setTargetPos(SINGLE_ROTATION_STEPS * 0.75, 0, false, 2000);
    }
}

void Motor::setSpeed(int stepsPerSecond)
{
    stepper.setSpeedInStepsPerSecond(stepsPerSecond);
    stepper.setAccelerationInStepsPerSecondPerSecond(stepsPerSecond / 2);
}

void Motor::loop()
{
    stepper.processMovement();
    if (!initialised)
    {
        calibratePosition();
    }
}
