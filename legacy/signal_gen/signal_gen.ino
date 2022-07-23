#include "FlexyStepper.h"

FlexyStepper stepper1;
FlexyStepper stepper2;
FlexyStepper stepper3;
FlexyStepper stepper4;

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

//    pinMode(13, OUTPUT); // home stop

//    digitalWrite(13, HIGH);
    
    stepper1.connectToPins(3, 2);
    stepper1.setStepsPerRevolution(720 * 6);
    stepper1.setSpeedInRevolutionsPerSecond(0.1);
    stepper1.setAccelerationInRevolutionsPerSecondPerSecond(0.02);

    stepper2.connectToPins(4, 5);
    stepper2.setStepsPerRevolution(720 * 6);
    stepper2.setSpeedInRevolutionsPerSecond(0.1);
    stepper2.setAccelerationInRevolutionsPerSecondPerSecond(0.02);

    stepper3.connectToPins(6, 7);
    stepper3.setStepsPerRevolution(720 * 6);
    stepper3.setSpeedInRevolutionsPerSecond(0.1);
    stepper3.setAccelerationInRevolutionsPerSecondPerSecond(0.02);

    stepper4.connectToPins(8, 9);
    stepper4.setStepsPerRevolution(720 * 6);
    stepper4.setSpeedInRevolutionsPerSecond(0.1);
    stepper4.setAccelerationInRevolutionsPerSecondPerSecond(0.02);
    Serial.begin(115200);
}



void loop()
{
//    stepper1.processMovement();
    stepper2.processMovement();
    stepper3.processMovement();
//    stepper4.processMovement();
    if (stepper1.motionComplete())
    {
        stepper1.setTargetPositionInRevolutions(1);
    }
    if (stepper2.motionComplete())
    {
        stepper2.setTargetPositionInRevolutions(1);
    }
    if (stepper3.motionComplete())
    {
        stepper3.setTargetPositionInRevolutions(-1);
    }
    if (stepper4.motionComplete())
    {
        stepper4.setTargetPositionInRevolutions(-1);
    }

    int sensorValue = analogRead(A0);
    int sensorValue2 = analogRead(A1);
    int sensorValue3 = analogRead(A2);
    int sensorValue4 = analogRead(A3);

    int sens1 = map(sensorValue, 500, 400, 0, 100);
    int sens2 = map(sensorValue2, 500, 460, 0, 100);

    if(sens2 > 20){
        stepper2.setCurrentPositionInRevolutions(0.315);
    }
    if(sens1 > 20){
        stepper3.setCurrentPositionInRevolutions(-0.345);
    }
//    Serial.println(stepper3.getCurrentPositionInRevolutions());
    Serial.print(",");
    Serial.print(sens1);
    Serial.print(",");
    Serial.println(sens2);

    
}
