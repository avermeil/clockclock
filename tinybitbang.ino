byte stepss[] = {
    B00010001,
    B00000001,
    B00001011,
    B00001010,
    B00011010,
    B00010000,
};

const byte pwm_resolution = 5;
const byte step_count = pwm_resolution * (sizeof(stepss) / sizeof(stepss[0]));
byte more_steps[step_count];

volatile byte counter = 0;
int pwm_step = 0;

void setup()
{
    DDRB |= B00011011;
    MCUCR |= B00000011; //watch for rising edge
    GIMSK |= B01000000; //enable external interrupt
    SREG |= B10000000;  //global interrupt enable
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
}

ISR(INT0_vect)
{
    counter++;

    if (counter >= step_count)
    {
        counter = 0;
    }
}