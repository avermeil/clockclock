#include "Arduino.h"
#include "Motor.h"

#include "Clock.h"

int clock_steps = 1440;

Clock::Clock( ){

	// highest step delay is 10,000
	// lowest step delay is 1,500

	
	motor_hour.speed_mod = 2;
	motor_minute.speed_mod = 1;
}

int Clock::doStep(){

    int mask_hour = motor_hour.getStepMask();
    int mask_minute = motor_minute.getStepMask();

    return mask_hour + mask_minute * 16;
}

int Clock::setHands(int hour_position, int minute_position){


	if(motor_hour.current_position != hour_position || motor_minute.current_position != minute_position){
		if(motor_hour.current_position >= hour_position){
			motor_hour.moveBy(clock_steps - motor_hour.current_position);
			motor_hour.moveBy(hour_position);
		}
		else {
			motor_hour.moveBy(hour_position - motor_hour.current_position);
		}


		if(motor_minute.current_position >= minute_position){
			motor_minute.moveBy(clock_steps - motor_minute.current_position);
			motor_minute.moveBy(minute_position + clock_steps);
		}
		else {
			motor_minute.moveBy((minute_position - motor_minute.current_position) + clock_steps);
		}
	}
}




