#include "Clock.h"

//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;

// int motor_count = 2;

// int bit_0 = 1 * 0;
// int bit_1 = pow(1, 2) * 1;
// int bit_2 = pow(2, 2) * 1;
// int bit_3 = pow(3, 2) * 1;
// int bit_4 = pow(4, 2) * 1;

int rotation_steps = 1440;

int bottom = 0;
int left =  (rotation_steps / 4) + 15;
int top = (rotation_steps  / 2) + 0;
int right =  (rotation_steps * 3 / 4) - 6;

int out = rotation_steps / 8;

int auto_mode = 0;
unsigned long refresh_interval = 15000;
unsigned long last_refresh_time = 0;
unsigned long count = 0;
int current_digit = 0;

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

    Serial.println(8888888);
    Serial.println(bottom);
    Serial.println(left);
    Serial.println(top);
    Serial.println(right);

}

void loop() {
    count ++;
    // int clock1_mask = clock_1.doStep();
    digitalWrite(latchPin, LOW);

    shift(clock_1.doStep());
    shift(clock_2.doStep());
    shift(clock_3.doStep());
    shift(clock_4.doStep());
    shift(clock_5.doStep());
    shift(clock_6.doStep());

    digitalWrite(latchPin, HIGH);

    // if(count % 100 == 0){

    //     int sensorValue = analogRead(A5);
    //     // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
    //     float voltage = sensorValue * (5.0 / 1023.0);
    //     // print out the value you read:
    //     Serial.println(voltage);
    // }


    if(auto_mode){
        if(millis() - last_refresh_time >= refresh_interval)
        {
            Serial.println(last_refresh_time);

            last_refresh_time += refresh_interval;
            showDigit(current_digit);
            current_digit++;

            if(current_digit > 9){
                current_digit = -1;
            }
        }
    }

   

    while (Serial.available() > 0) {

        int digit = Serial.parseInt();
        
        if(digit == -2){
            auto_mode = 1;
        }
        else {
            showDigit(digit);
            auto_mode = 0;
        }

    }
}

void showDigit(int digit){
    Serial.println(digit);
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
    if(digit == 5){
        showFive();
    }
    if(digit == 6){
        showSix();
    }
    if(digit == 7){
        showSeven();
    }
    if(digit == 8){
        showEight();
    }
    if(digit == 9){
        showNine();
    }
    if(digit == -1){
        Serial.println(999999999);
        showNull();
    }
}

void showZero(){
    // clock_1.setHands(bottom, right );
    clock_2.setHands(bottom, left );
    // clock_3.setHands(top, bottom );
    // clock_4.setHands(top, bottom );
    // clock_5.setHands(top, right );
    // clock_6.setHands(top, left );
}

void showOne(){
    clock_1.setHands(out, out );
    clock_2.setHands(bottom, bottom );
    clock_3.setHands(out, out );
    clock_4.setHands(top, bottom );
    clock_5.setHands(out, out );
    clock_6.setHands(top, top );
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

void showFive(){
    clock_1.setHands(right, bottom );
    clock_2.setHands(left, left );
    clock_3.setHands(top, right );
    clock_4.setHands(left, bottom );
    clock_5.setHands(right, right );
    clock_6.setHands(top, left );
}

void showSix(){
    clock_1.setHands(right, bottom );
    clock_2.setHands(left, left );
    clock_3.setHands(top, bottom );
    clock_4.setHands(bottom, left );
    clock_5.setHands(right, top );
    clock_6.setHands(top, left );
}

void showSeven(){
    clock_1.setHands(right, right );
    clock_2.setHands(left, bottom );
    clock_3.setHands(out, out );
    clock_4.setHands(top, bottom );
    clock_5.setHands(out, out );
    clock_6.setHands(top, top );
}

void showEight(){
    clock_1.setHands(right, bottom );
    clock_2.setHands(left, bottom );
    clock_3.setHands(right, bottom );
    clock_4.setHands(left, bottom );
    clock_5.setHands(right, top );
    clock_6.setHands(top, left );
}

void showNine(){
    clock_1.setHands(right, bottom );
    clock_2.setHands(left, bottom );
    clock_3.setHands(top, right );
    clock_4.setHands(top, bottom );
    clock_5.setHands(right, right );
    clock_6.setHands(top, left );
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

