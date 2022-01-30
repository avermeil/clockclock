#include "FlexyStepper.h"

FlexyStepper stepper2;

const int MIN_INT = -32768;
const int SINGLE_ROTATION_STEPS = 4320;
int stepsOffset = 0;
bool isClockwise = true;
int magnetPosition = 1230;

void setup()
{
    Serial.begin(115200);

    pinMode(2, OUTPUT); // B dir
    pinMode(3, OUTPUT); // B step
    pinMode(4, OUTPUT); // A step
    pinMode(5, OUTPUT); // A dir
    pinMode(6, OUTPUT); // D step
    pinMode(7, OUTPUT); // D dir
    pinMode(8, OUTPUT); // C step
    pinMode(9, OUTPUT); // C dir

    stepper2.connectToPins(4, 5);

    // go fast for initial calibration
    stepper2.setSpeedInStepsPerSecond(3000);
    stepper2.setAccelerationInStepsPerSecondPerSecond(2000);

    stepper2.setTargetPositionRelativeInSteps(SINGLE_ROTATION_STEPS);

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

void setTargetPos(int targetPos, int extraTurns, bool clockwise)
{
    isClockwise = clockwise;
    int reportedPos = getReportedPos();

    int currentPos = reportedPos - stepsOffset;
    int stepsToMake = 0;

    if (currentPos < targetPos)
    {
        stepsToMake = targetPos - currentPos;
    }
    else if (currentPos > targetPos)
    {
        stepsToMake = SINGLE_ROTATION_STEPS + targetPos - currentPos;
    }

    if (!clockwise)
    {
        stepsToMake = stepsToMake - SINGLE_ROTATION_STEPS;
    }

    Serial.print("clockwise: ");
    Serial.println(clockwise);
    Serial.print("targetPos: ");
    Serial.println(targetPos);
    Serial.print("stepsOffset: ");
    Serial.println(stepsOffset);
    Serial.print("currentPos: ");
    Serial.println(currentPos);
    Serial.print("stepsToMake: ");
    Serial.println(stepsToMake);
    stepper2.setTargetPositionRelativeInSteps(stepsToMake);
}

int getReportedPos()
{
    int reportedPos = stepper2.getCurrentPositionInSteps();
    if (reportedPos >= 0)
    {
        return stepper2.getCurrentPositionInSteps() % SINGLE_ROTATION_STEPS;
    }
    // 360 + (-450 % 360) = 270
    return SINGLE_ROTATION_STEPS + (stepper2.getCurrentPositionInSteps() % SINGLE_ROTATION_STEPS);
}

int calibrationBlockedUntilStep = MIN_INT;

void calibratePosition()
{
    int sensorValue2 = analogRead(A1);
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

void loop()
{
    stepper2.processMovement();

    calibratePosition();

    if (Serial.available() > 0)
    {
        String command = Serial.readStringUntil('\n');
        int pos = command.substring(0, 1).toInt();
        bool clockwise = !command.substring(1).toInt();

        setTargetPos(pos * 540, 0, clockwise);
    }
}
