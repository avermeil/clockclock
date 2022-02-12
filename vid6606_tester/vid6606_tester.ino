

void setup()
{
    pinMode(A0, OUTPUT); // B dir
    pinMode(A1, OUTPUT); // B step
    pinMode(A2, OUTPUT); // A step
    pinMode(A3, OUTPUT); // A dir
}

void loop()
{
    digitalWrite(A0, HIGH);
    digitalWrite(A1, HIGH);
    digitalWrite(A2, HIGH);
    digitalWrite(A3, HIGH);

    delay(1);
    digitalWrite(A0, LOW);
    digitalWrite(A1, LOW);
    digitalWrite(A2, LOW);
    digitalWrite(A3, LOW);

    delay(1);
}
