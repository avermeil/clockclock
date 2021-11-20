// #include "SpeedyStepper.h"

// SpeedyStepper stepper1;

// // In Setup(), assign IO pins used for Step and Direction:

// int sensorPin = A0;  // select the input pin for the potentiometer
// int ledPin = 2;      // select the pin for the LED
// int sensorValue = 0; // variable to store the value coming from the sensor

// void setup()
// {
//     // declare the ledPin as an OUTPUT:
//     pinMode(ledPin, OUTPUT);
//     stepper1.connectToPins(2, 11);
//     stepper1.setStepsPerRevolution(720 * 2 * 5);
//     stepper1.setSpeedInRevolutionsPerSecond(0.4);
//     stepper1.setAccelerationInRevolutionsPerSecondPerSecond(0.05);
//     Serial.begin(9600);
// }

// void loop()
// {
//     // stepper1.setupRelativeMoveInRevolutions(-1.5);

//     // while (!stepper1.motionComplete())
//     // {
//     //     stepper1.processMovement();
//     // }
//     // delay(500);
//     // read the value from the sensor :
//     sensorValue = analogRead(sensorPin);
//     int delayTime = sensorValue;
//     Serial.println(delayTime);
//     // turn the ledPin on
//     digitalWrite(ledPin, HIGH);
//     digitalWrite(LED_BUILTIN, HIGH);

//     // stop the program for <sensorValue> milliseconds:

//     delay(delayTime);
//     // turn the ledPin off:
//     digitalWrite(ledPin, LOW);
//     digitalWrite(LED_BUILTIN, LOW);
//     // stop the program for for <sensorValue> milliseconds:
//     delay(delayTime);
// }
