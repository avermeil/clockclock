#include <Clock.h>

//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;

int motor_count = 2;

int bit_0 = 1 * 0;
int bit_1 = pow(1, 2) * 1;
int bit_2 = pow(2, 2) * 1;
int bit_3 = pow(3, 2) * 1;
int bit_4 = pow(4, 2) * 1;

int top = 720;
int right = 1080;
int bottom = 0;
int left = 360;
int out = 180;

Clock clock_1;
Clock clock_2;
Clock clock_3;
Clock clock_4;
Clock clock_5;
Clock clock_6;


void setup() {
    Serial.begin(9600);
    pinMode(latchPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, OUTPUT);

    // analogWrite(5, 255);

}

void loop() {
    // Serial.println(99999);
    // int clock1_mask = clock_1.doStep();
    digitalWrite(latchPin, LOW);

    shift(clock_1.doStep());
    shift(clock_2.doStep());
    shift(clock_3.doStep());
    shift(clock_4.doStep());
    shift(clock_5.doStep());
    shift(clock_6.doStep());

    digitalWrite(latchPin, HIGH);

    while (Serial.available() > 0) {
        // look for the next valid integer in the incoming serial stream:
        //int move = Serial.parseInt();

        //clock_1.setHands(move, move );
        // clock_2.setHands(move, move );
        // clock_3.setHands(move, move );
        // clock_4.setHands(move, move );
        // clock_5.setHands(move, move );
        // clock_6.setHands(move, move );

        int digit = Serial.parseInt();
        // Serial.println(digit);

        if(digit == 0){
            showZero();
        }
        if(digit == 1){
            showOne();
        }
        if(digit == 2){
            showTwo();
        }
        if(digit == 3){
            showThree();
        }
        if(digit == 4){
            showFour();
        }
        if(digit == -1){
            Serial.println(999999999);
            showNull();
        }

    }
}

void showZero(){
    clock_1.setHands(bottom, right );
    clock_2.setHands(bottom, left );
    clock_3.setHands(top, bottom );
    clock_4.setHands(top, bottom );
    clock_5.setHands(top, right );
    clock_6.setHands(top, left );
}

void showOne(){
    clock_1.setHands(out, out );
    clock_2.setHands(left, bottom );
    clock_3.setHands(out, out );
    clock_4.setHands(top, bottom );
    clock_5.setHands(out, out );
    clock_6.setHands(top, bottom );
}
void showTwo(){
    clock_1.setHands(right, right );
    clock_2.setHands(left, bottom );
    clock_3.setHands(bottom, right );
    clock_4.setHands(top, left );
    clock_5.setHands(top, right );
    clock_6.setHands(left, left );
}

void showThree(){
    clock_1.setHands(right, right );
    clock_2.setHands(left, bottom );
    clock_3.setHands(right, right );
    clock_4.setHands(top, left );
    clock_5.setHands(right, right );
    clock_6.setHands(top, left );
}

void showFour(){
    clock_1.setHands(bottom, bottom );
    clock_2.setHands(bottom, bottom );
    clock_3.setHands(top, right );
    clock_4.setHands(top, bottom );
    clock_5.setHands(out, out );
    clock_6.setHands(top, top );
}

void showNull(){
    clock_1.setHands(bottom, bottom );
    clock_2.setHands(bottom, bottom );
    clock_3.setHands(bottom, bottom );
    clock_4.setHands(bottom, bottom );
    clock_5.setHands(bottom, bottom );
    clock_6.setHands(bottom, bottom );
}

void shift(int mask) {
    shiftOut(dataPin, clockPin, MSBFIRST, mask); 
}

