#include "FlexyStepper.h"

FlexyStepper stepper2;

int stepsOffset = 0;
bool isClockwise = true;
bool just_zoned = false;
int magnetPosition = 1390;

void setup()
{
    pinMode(2, OUTPUT); // B dir
    pinMode(3, OUTPUT); // B step
    pinMode(4, OUTPUT); // A step
    pinMode(5, OUTPUT); // A dir
    pinMode(6, OUTPUT); // D step
    pinMode(7, OUTPUT); // D dir
    pinMode(8, OUTPUT); // C step
    pinMode(9, OUTPUT); // C dir

    stepper2.connectToPins(4, 5);
    stepper2.setStepsPerRevolution(720 * 6);
    stepper2.setSpeedInRevolutionsPerSecond(0.2);
    stepper2.setAccelerationInRevolutionsPerSecondPerSecond(0.2);

    Serial.begin(115200);

    stepper2.setTargetPositionRelativeInSteps(4320 * 2);

    while (!stepper2.motionComplete())
    {
        stepper2.processMovement();
        calibratePosition();
    }

    setTargetPos(0, 0, true);
}

void setTargetPos(int targetPos, int extraTurns, bool clockwise)
{
    isClockwise = clockwise;
    int reportedPos = getCurrentPos();

    int currentPos = reportedPos - stepsOffset;
    int stepsToMake = 0;

    if (currentPos < targetPos)
    {
        stepsToMake = targetPos - currentPos;
    }
    else if (currentPos > targetPos)
    {
        stepsToMake = 4320 + targetPos - currentPos;
    }

    if (!clockwise)
    {
        stepsToMake = stepsToMake - 4320;
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

// TODO: figure out why calibration sometimes is offset. Perhaps lock for longer than 1 step.
void calibratePosition()
{
    int sensorValue2 = analogRead(A1);

    if (sensorValue2 < 500 && isClockwise)
    {
        if (!just_zoned)
        {
            just_zoned = true;
            stepsOffset = getCurrentPos() - magnetPosition;
            // Serial.println("in the zone");
        }
    }
    else
    {
        just_zoned = false;
    }
}

int getCurrentPos()
{
    return stepper2.getCurrentPositionInSteps() % 4320;
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
