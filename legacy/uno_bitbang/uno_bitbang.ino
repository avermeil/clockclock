
volatile byte counter = 0;

byte stepss[] = {
    B10010000,
    B00010000,
    B01110000,
    B01100000,
    B11100000,
    B10000000,
};

/*
    forEach step
        generate 25 pwm slots
        for (range 0-25 = i)
            for each pin
                if pin power is above i
                    generate byte with pin bit =1
            sum pin bytes to get pwm step
            save pwm step to slot

        for each pin
            fill the first x (0-25) slots with value at that bit (eg B01000000)
*/
const int pwm_resolution = 5;

int sin_table[pwm_resolution * 2];

const int step_count = pwm_resolution * (sizeof(stepss) / sizeof(stepss[0]));

byte more_steps[step_count];

int pwm_step = 0;
int power_reduction_counter = 0;
int power_reduction = 2;
long long mega_counter = 0;
volatile int pwm_complete_per_step = 0;

unsigned long long last_step = 0;
long step_time = 3000;

void setup()
{
    Serial.begin(9600);

    // set pins 4,5,6,7 to output
    DDRD = DDRD | B11110000;

    for (int i = 0; i < step_count; i++)
    {
        int index = (int)floor(i / pwm_resolution);

        more_steps[i] = stepss[index];
    }

    for (int i = 0; i <= pwm_resolution * 2; i++)
    {
        float rad = (360 * i * 3.14159265359 / (pwm_resolution * 4)) / 180;

        float t = sin(rad) * pwm_resolution;

        Serial.print(t);
        Serial.print(" , ");

        sin_table[i] = round(t);

        Serial.println(sin_table[i]);

        //Serial.println(sin_table[i]);
    }

    attachInterrupt(digitalPinToInterrupt(2), nextStep, RISING);
}

// the loop routine runs over and over again forever:
void loop()
{
    mega_counter++;
    byte actual_counter = (counter + pwm_step);

    if (actual_counter >= step_count)
    {
        actual_counter = actual_counter - step_count;
    }

    pwm_step++;
    if (pwm_step == pwm_resolution)
    {
        pwm_step = 0;
        pwm_complete_per_step++;
    }

    PORTD = more_steps[actual_counter];

    long current_time = micros();
    if (last_step < current_time - step_time)
    {
        nextStep();
        last_step = current_time;
    }

    // power_reduction_counter++;
    // if (power_reduction_counter == power_reduction)
    // {
    //     power_reduction_counter = 0;
    //     PORTD = more_steps[actual_counter];
    // }

    // else
    // {

    //     PORTD = B00000000;
    // }
}

void nextStep()
{
    // if (mega_counter % power_reduction == 0)
    // {
    //     return;
    // }
    counter++;

    if (counter >= step_count)
    {
        counter = 0;
        Serial.println(pwm_complete_per_step);
    }

    pwm_complete_per_step = 0;
}