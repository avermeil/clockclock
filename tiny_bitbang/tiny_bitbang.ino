const byte stepss[] = {
    B00010001,
    B00000001,
    B00001011,
    B00001010,
    B00011010,
    B00010000,
};

const byte pwm_resolution = 5;
const byte step_count = pwm_resolution * 6;
byte more_steps[step_count];

volatile byte counter = 0;
int pwm_step = 0;

unsigned long long last_step = 0;
long step_time = 3000;

void setup()
{
    DDRB |= B00011011;
    // MCUCR |= B00000011; //watch for rising edge
    // GIMSK |= B01000000; //enable external interrupt
    // SREG |= B10000000;  //global interrupt enable
    for (int i = 0; i < step_count; i++)
    {
        int index = (int)floor(i / pwm_resolution);

        more_steps[i] = stepss[index];
    }
}

void loop()
{
    byte actual_counter = (counter + pwm_step);

    if (actual_counter >= step_count)
    {
        actual_counter = actual_counter - step_count;
    }

    pwm_step++;
    if (pwm_step == pwm_resolution)
    {
        pwm_step = 0;
    }
    PORTB = more_steps[actual_counter];

    long current_time = micros();
    if (last_step < current_time - step_time)
    {
        doStep();
        last_step = current_time;
    }
}

// NOTE: leaving PB0 into the arduino messes this interrupt up.
// ISR(INT0_vect)
// {
//     counter++;

//     if (counter == step_count)
//     {
//         counter = 0;
//     }
// }

void doStep()
{
    counter = counter + 1;

    if (counter == step_count)
    {
        counter = 0;
    }
}