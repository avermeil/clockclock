#include <SPI.h>
#include <math.h>

//Pin connected to ST_CP of 74HC595
int latchPin = 8;

int pwm_step = 0;
unsigned long long last_pwm_step = 0;
const int pwm_step_time = 5;
const int pwm_resolution = 8;
int power_reduction = 0;

const int steps_per_rotation = 720 * pwm_resolution;
unsigned long long current_step = 0;

unsigned long long stop_until = 0;

int sin_table[pwm_resolution * 2];

unsigned long long last_step = 0;

long max_speed = 8000 / pwm_resolution;
long min_speed = 40000 / pwm_resolution;
long step_time = min_speed;

unsigned long long acceleration_timer = 50000;
unsigned long long last_acceleration_time = 0;
int acceleration_direction = -1;

int a_power = 0;
int b_power = 0;
int c_power = 0;
int d_power = 0;

int previous_mask = 0;

int alpha_index = 0;
int beta_index = pwm_resolution * 0.75;


void setup()

{
    Serial.begin(74880);

 
    pinMode(latchPin, OUTPUT);

    

    for(int i=0; i<=pwm_resolution * 2; i++){
        float rad = (360*i*3.14159265359/(pwm_resolution*4)) / 180;

        float t = sin(rad) * pwm_resolution;

        Serial.print(t);
        Serial.print(" , ");

        sin_table[i] = round(t);

        Serial.println(sin_table[i]);

        
        //Serial.println(sin_table[i]);
        
    }

    last_pwm_step = micros();
    last_step = step_time;

    SPI.begin();

    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
}


void loop(){
  //long current_time = micros();

   doTimers();
  
  //Serial.println(micros() -current_time);
}

inline void doTimers(){
  unsigned long long current_time = micros();

  if(last_pwm_step < current_time - pwm_step_time){
    pwm();
    last_pwm_step = current_time;
  }

  if(current_time < stop_until){
    return;
  }

  if(last_acceleration_time < current_time - acceleration_timer){
    last_acceleration_time = current_time;

    int extra = acceleration_direction * (sqrt(abs((max_speed - step_time)*20)) + 1) ;
    step_time += extra;

    if(step_time > min_speed){
      acceleration_direction = -1;
      stop_until = current_time + 1000000;
    }
    if(step_time < max_speed){
      acceleration_direction = 1;

//      int sensorValue = analogRead(A5);
//      float voltage = sensorValue * (5.0 / 1023.0);
//      Serial.println(voltage);
    }
  }


  if(last_step < current_time - step_time){
    step();
    last_step = current_time;
  }

}

inline void step(){

  if(current_step > steps_per_rotation){
    current_step = 0;
    Serial.println("rotation...");
  }

  

  int quarter_turn = steps_per_rotation / 4;

  int distance_from_qt = abs(min(current_step, steps_per_rotation / 2) - quarter_turn );

  distance_from_qt -= steps_per_rotation / 6;

  if(distance_from_qt < 0){
    power_reduction = 0;
  }
  else {
    power_reduction = 0; //distance_from_qt / 500;
  }

    current_step++;

  
  alpha_index++;
  if(alpha_index >= (pwm_resolution*4)){
    alpha_index = 0;
  }

  beta_index++;
  if(beta_index >= (pwm_resolution*4)){
    beta_index = 0;
  }

  int flattener = 0;


  if(alpha_index < (pwm_resolution*2)){
    //Serial.println(sin_table[alpha_index]);
    
    a_power = sin_table[alpha_index] + flattener;
    b_power = 0;
  } else {
    a_power = 0;
    b_power = sin_table[alpha_index - (pwm_resolution*2)] + flattener;
  }

  if(beta_index < (pwm_resolution*2)){
    c_power = sin_table[beta_index] + flattener;
    d_power = 0;
  } else {
    c_power = 0;
    d_power = sin_table[beta_index - (pwm_resolution*2)] + flattener;
  }

//    Serial.println(a_power);
//  Serial.print(",");
//  Serial.print(-b_power);
//  Serial.print(",");
//
//  Serial.print(c_power);
//  Serial.print(",");
//
//  Serial.println(-d_power);
}

inline void pwm(){

  
  
  if(pwm_step >= pwm_resolution + power_reduction){
    pwm_step = 0;
    
  }

  int mask = 0b0000;

  if(a_power >= pwm_step){
    mask += 0b0001;
  }
  if(b_power >= pwm_step){
    mask += 0b0010;
  }
  if(c_power >= pwm_step){
    mask += 0b0100;
  }
  if(d_power >= pwm_step){
    mask += 0b1000;
  }

  

  if(mask != previous_mask){    
    previous_mask = mask;
  
    digitalWriteDirectLow(latchPin);
  
    
    SPI.transfer(mask);
  
    digitalWriteDirectHigh(latchPin);
  }
  
  
  //SPI.endTransaction();
 
  pwm_step++;

}

inline void digitalWriteDirectHigh(int pin){
  //digitalWrite(pin, HIGH);
  g_APinDescription[pin].pPort -> PIO_SODR = g_APinDescription[pin].ulPin;
}

inline void digitalWriteDirectLow(int pin){
  //digitalWrite(pin, LOW);
  g_APinDescription[pin].pPort -> PIO_CODR = g_APinDescription[pin].ulPin;
}


