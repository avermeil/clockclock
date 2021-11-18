
volatile byte counter = 0;

byte stepss[8] = {
    B10000000,
    B10010000,
    B00010000,
    B00110000,
    B00100000,
    B01100000,
    B01000000,
    B11000000,
};

const int pwm_resolution = 8;
const int step_count = pwm_resolution * 8;

byte more_steps[step_count];

int pwm_step = 0;

volatile int pwm_complete_per_step = 0;

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

    attachInterrupt(digitalPinToInterrupt(2), nextStep, RISING);
}

// the loop routine runs over and over again forever:
void loop()
{

    byte actual_counter = (counter + pwm_step);

    if (actual_counter >= step_count)
    {
        actual_counter = actual_counter - step_count;
    }

    PORTD = more_steps[actual_counter];

    pwm_step++;
    if (pwm_step == pwm_resolution)
    {
        pwm_step = 0;
        pwm_complete_per_step++;
    }
}

void nextStep()
{
    counter++;
    if (counter >= step_count)
    {
        counter = 0;
        Serial.println(pwm_complete_per_step);
    }

    pwm_complete_per_step = 0;
}