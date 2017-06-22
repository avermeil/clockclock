#include "Arduino.h"
#include "Motor.h"
// int steps[4] = {
//     0b1100,
//     0b0110,
//     0b0011,
//     0b1001
// };


int steps[8] = {
    0b1000,
    0b1001,
    0b0001,
    0b0011,
    0b0010,
    0b0110,
    0b0100,
    0b1100
};


int steps_per_rotation = 1440;

Motor::Motor(){
    steps_since_start = 0;
    steps_left = 0;
    pin_offset = 0;
    current_step = 0;
    current_position = 0;
    step_mask = 0b0000;
    speed_mod = 1;
    time = micros();
}

int Motor::getStepMask(){

    time = micros();

    if(last_step_time >= (time - step_delay)){
        return step_mask;
    }

    int previous_steps_left = steps_left;

    if(steps_left == 0){
        steps_since_start = 0;
        return step_mask;
    }

    current_step ++;
    if(current_step > 7){
        current_step = 0;
    }

    current_position++;

    if(current_position >= steps_per_rotation){
        current_position = 0;
    }

    step_mask = steps[current_step];

    steps_left--;
    steps_since_start++;

    if(steps_left == 0 && previous_steps_left != 0){
       // Serial.println(current_position);
    }

    //step_delay=2000;

    int distance_from_edge = sqrt(min(steps_since_start,  steps_left));

    step_delay = max(1750, 5000 - distance_from_edge * 170) * speed_mod;

    last_step_time = time;

    return step_mask;
}

int Motor::moveBy(int steps_to_move){
    steps_left += steps_to_move;
    return 0;
}
