#include "Arduino.h"

void setup()
{
    Serial.begin(9600);
    pinMode(A2, INPUT);
    pinMode(A3, INPUT);
    pinMode(A6, INPUT);
    pinMode(A7, INPUT);
}

void loop()
{
    int sensorValue1 = analogRead(A2);
    int sensorValue2 = analogRead(A3);
    int sensorValue3 = analogRead(A6);
    int sensorValue4 = analogRead(A7);
    Serial.print(sensorValue1); // print the character
    Serial.print(",");          // print the character
    Serial.print(sensorValue2); // print the character
    Serial.print(",");          // print the character

    Serial.print(sensorValue3); // print the character
    Serial.print(",");          // print the character

    Serial.println(sensorValue4); // print the character
}
